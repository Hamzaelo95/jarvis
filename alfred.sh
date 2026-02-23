#!/bin/bash

export LC_ALL=C
export LANG=C

########## MAIN VARIABLES ##########
DATE=$(date +'%Y-%m-%d_%H:%M:%S')
TARGET=${1:-localhost:8000}
API_KEY=${2:-}

########## TEMPORARY WORKING REPERTORY ##########
export PATH="$(pwd)/bin:$PATH"

MACHINE_ARCH=$(uname -m)
BIN_DIR="$(pwd)/bin"

case "$MACHINE_ARCH" in
    x86_64)
        ln -sf "$BIN_DIR/jq64" "$BIN_DIR/jq"
        ;;
    aarch64|arm64)
        ln -sf "$BIN_DIR/jq_arm64" "$BIN_DIR/jq"
        ;;
    *)
        echo "ERROR: This architecture not supported for the moment: ($MACHINE_ARCH)"
        exit 1
        ;;
esac

############ GRABBER #################
# --- CPU ---
CPU_MODEL=$(lscpu | grep "Model name:" | cut -d: -f2 | sed 's/^ *//')
_VENDOR=$(lscpu | grep "Vendor ID:" | cut -d: -f2 | xargs)
_FAMILY=$(lscpu | grep "CPU family:" | cut -d: -f2 | xargs)
_MODEL=$(lscpu | grep "Model:" | cut -d: -f2 | xargs)
CPU_ID="${_VENDOR} Fam ${_FAMILY} Mod ${_MODEL}"
CPU_FREQUENCY_MIN=$(lscpu | grep "CPU min MHz" | cut -d: -f2 | xargs | cut -d. -f1)
CPU_FREQUENCY_MAX=$(lscpu | grep "CPU max MHz" | cut -d: -f2 | xargs | cut -d. -f1)
CPU_FREQUENCY_CUR=$(grep "cpu MHz" /proc/cpuinfo | head -n1 | cut -d: -f2 | cut -d. -f1 | xargs)
CPU_CORES=$(lscpu | grep "^CPU(s):" | cut -d: -f2 | xargs)
CPU_THREADS=$(nproc)

# --- RAM ---
# On récupère la RAM totale via free -h
RAM_TOTAL=$(free -h | awk '/^Mem:/ {print $2}')
# On récupère les slots via inxi
RAM_SLOTS=$(inxi -m -c 0 | grep "slots:" | head -n1 | sed -E 's/.*slots: ([0-9]+).*/\1/')
if [ -z "$RAM_SLOTS" ]; then RAM_SLOTS="N/A"; fi

# --- MOTHERBOARD / GPU ---
MB_SERIAL=$(inxi -M -c 0 | grep "Mobo:" | sed -E 's/.*Mobo: (.*) model: (.*) serial: .*/\1 \2/' | xargs)
[ -z "$MB_SERIAL" ] && MB_SERIAL=$(inxi -M -c 0 | grep "Mobo:" | cut -d: -f2 | cut -d',' -f1 | xargs)
GPU_MODEL=$(inxi -G -c 0 | grep "Device-1:" | cut -d: -f2 | xargs)

# --- STORAGE ---
# Check le stockage total
TOTAL_STORAGE=$(inxi -D -c 0 | grep -o 'Total: [0-9.]* [A-Za-z]*' | sed 's/Total: //')

# Stockage des partitions
# Initialisation d'un tableau vide
mapfile -t ALL_DISKS < <(lsblk -d -n -o NAME,TYPE | awk '$2=="disk" {print $1}')

# Initialisation d'un tableau JSON vide pour stocker les disques
DISKS_JSON="[]"

for disk in "${ALL_DISKS[@]}"; do
    # On récupère les partitions du disque courant.
    DISK_PARTS=$(lsblk -J -l -o TYPE,NAME,FSTYPE,SIZE,FSUSED "/dev/$disk" 2>/dev/null | \
        jq '[.blockdevices[]? | select(.type == "part") | {name: .name, fstype: (.fstype // "N/A"), size: (.size // "N/A"), used: (.fsused // "N/A")}]')
    
    # Si le disque n'a pas de partitions, on mets un tableau vide
    if [ -z "$DISK_PARTS" ] || [ "$DISK_PARTS" == "null" ]; then
        DISK_PARTS="[]"
    fi

    # On crée l'objet parent "disque" qui contient son nom et son tableau de partitions
    DISK_OBJECTS=$(jq -n \
        --arg name "$disk" \
        --argjson parts "$DISK_PARTS" \
        '{name: $name, partitions: $parts}')
    
    # On ajoute cet objet disque au tableau principal DISKS_JSON
    DISKS_JSON=$(echo "$DISKS_JSON" | jq --argjson new_disk "$DISK_OBJECTS" '. + [$new_disk]')
done

# --- SOFTWARE ---
OS=$(lsb_release -d 2>/dev/null | cut -f2 || grep PRETTY_NAME /etc/os-release | cut -d= -f2 | tr -d '"')
ARCH=$(uname -m)
KERNEL=$(uname -r)
HOSTNAME=$(uname -n)
DEFAULT_IFACE=$(ls /sys/class/net | grep -vE '^(lo|docker|veth|br)' | head -n 1)
MAC_ADDRESS=$(cat "/sys/class/net/$DEFAULT_IFACE/address" 2>/dev/null || echo "Unknown-MAC")

##### JSON PART #####
json_pkg() { 
    json_data=$(jq -n \
        --arg motherboard "$MB_SERIAL" \
        --arg cpu_model "$CPU_MODEL" \
        --arg cpu_id "$CPU_ID" \
        --arg cpu_cores "$CPU_CORES" \
        --arg cpu_threads "$CPU_THREADS" \
        --arg cpu_frequency_min "$CPU_FREQUENCY_MIN" \
        --arg cpu_frequency_cur "$CPU_FREQUENCY_CUR" \
        --arg cpu_frequency_max "$CPU_FREQUENCY_MAX" \
        --arg total_storage "$TOTAL_STORAGE" \
        --argjson disks "$DISKS_JSON" \
        --arg gpu_model "$GPU_MODEL" \
        --arg ram_slots "$RAM_SLOTS" \
        --arg ram_total "$RAM_TOTAL" \
        --arg total_storage "$TOTAL_STORAGE" \
        --arg hostname "$HOSTNAME" \
        --arg mac_address "$MAC_ADDRESS" \
        --arg os "$OS" \
        --arg arch "$ARCH" \
        --arg desktop_env "${XDG_CURRENT_DESKTOP:-N/A}" \
        --arg window_manager "${XDG_SESSION_TYPE:-N/A}" \
        --arg kernel "$KERNEL" \
        '{
        HARDWARE: {
            motherboard: $motherboard,
            cpu_model: $cpu_model,
            cpu_id: $cpu_id,
            cpu_cores: $cpu_cores,
            cpu_threads: $cpu_threads,
            cpu_frequency_min: $cpu_frequency_min,
            cpu_frequency_cur: $cpu_frequency_cur,
            cpu_frequency_max: $cpu_frequency_max,
            gpu_model: $gpu_model,
            ram_slots: $ram_slots,
            ram_total: $ram_total,
            total_storage: $total_storage,
            disks: $disks
        },
        SOFTWARE: {
            hostname: $hostname,
            mac_address:$mac_address,
            os: $os,
            arch: $arch,
            desktop_env: $desktop_env,
            window_manager: $window_manager,
            kernel: $kernel
        }
        }'
    )
    
    curl -X POST "http://$TARGET/endpoint" \
         -H "X-API-Key: $API_KEY" \
         -H "Content-Type: application/json" \
         -d "$json_data" \
         --connect-timeout 5 || echo "[ERROR]: $TARGET server not found."
    echo ""
}

json_pkg
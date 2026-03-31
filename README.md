# Jarvis Daemon 

A lightweight, native C daemon for real-time hardware monitoring. Designed to efficiently track system health and send metrics to a remote Django API.

This v1 replaces the old bash/makeself script with a highly optimized C executable interacting directly with the Linux kernel (`/proc` and `/sys`).

## Features

* **CPU Monitoring:** Real-time usage (%) and model detection.
* **Temperature:** CPU thermal zone monitoring (C°).
* **RAM Usage:** Total, used, and dynamic percentage calculation.
* **Storage:** Root disk (`/`) space tracking using `statvfs`.
* **Network:** Automated JSON payload packaging and HTTP POST delivery via `libcurl`.

## Dependencies

To compile and run Jarvis, you need the standard C compiler and two external libraries for JSON and HTTP requests. 

Install them on Debian/Ubuntu using:
```bash
sudo apt update
sudo apt install gcc libjson-c-dev libcurl4-openssl-dev

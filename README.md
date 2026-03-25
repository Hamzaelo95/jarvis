# alfred
Working on the v1 of alfred daemon in C

## build

to use it, clone the git\
`git clone https://github.com/buchtioof/alfred.git`

then use this command\
`makeself ./alfred alfred.run ./alfred.sh`

## usage

run alfred with this command\
`./alfred.run "add ip:port of your server without double quotes ofc"`

## dependencies
- inxi (lastest release)
- jq (1.8.1 tested / arm64 and x86_64 supported, more later)

## credits
thanks to [inxi](https://github.com/smxi/inxi) and [jq](https://github.com/jqlang/jq)

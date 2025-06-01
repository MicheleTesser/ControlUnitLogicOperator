#!/bin/sh

if [[ ! -e ./main.c ]]; then
  echo "execute this script from the linux instance dir"
  exit 1
fi

root=$(pwd)

create() {
  sudo ip link add dev $1 type vcan
  sudo ip link set up $1
}

close() {
    echo "closing virtual can $1"
    sudo ip link delete $1
}

sudo modprobe gpio-mockup gpio_mockup_ranges=-1,50,-1,50
create "culo_can_0"
create "culo_can_1"
create "culo_can_2"

mkdir -p build

cd ./build
rm -rf ./*
cmake ..

make build_debug
cd debug
./main 

echo -n "Do you want to remove the virtual can bus[y/N]:"
read -t 1 -n 10000 discard 
read input

echo "closing virtual chip"
sudo modprobe -r gpio-mockup
if [[ ($input = "y") ]]; then
  close "culo_can_0"
  close "culo_can_1"
  close "culo_can_2"
fi


cd $root

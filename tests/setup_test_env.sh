#!/bin/sh

if [ $# -eq 0 ]; then
    echo "usage $0 [init,close]"
    exit 0
fi

create() {
    sudo ip link add dev $1 type vcan
    sudo ip link set up $1
}

close() {
    sudo ip link delete $1
}

if [ $1 = "init" ]; then
    sudo modprobe gpio-mockup gpio_mockup_ranges=0,50
    create "culo_can_0"
    create "culo_can_1"
    create "culo_can_2"
fi

if [ $1 = "close" ]; then
    close "culo_can_0"
    close "culo_can_1"
    close "culo_can_2"
    sudo modprobe -r gpio-mockup
fi

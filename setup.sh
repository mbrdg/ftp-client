#!/bin/sh

ifconfig eth0 down
ifconfig eth1 down

# tux 2
if [[ $1 -eq 2 ]]
    ifconfig eth0 up 172.16.41.1/24
    route add -net 172.16.40.0/24 gw 172.16.41.253
    route add default gw 172.16.41.254
fi

# tux 3
if [[ $1 -eq 3 ]]
    ifconfig eth0 up 172.16.40.1/24
    route add -net 172.16.41.0/24 gw 172.16.40.254
    route add default gw 172.16.40.254
fi

# tux 4
if [[ $1 -eq 4 ]]
    ifconfig eth0 up 172.16.40.254/24
    ifconfig eth1 up 172.16.41.253/24
    echo 1 > /proc/sys/net/ipv4/ip_forward
    echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
    route add default gw 172.16.41.254
fi


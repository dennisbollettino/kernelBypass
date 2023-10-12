#!/bin/bash

# setup iptables
iptables -I INPUT 1 -p udp --dport 4321 -j ACCEPT
iptables -t raw -I PREROUTING 1 -p udp --dport 4321 -j NOTRACK

# assign ip address
#for i in `seq 2 9`; do 
#    ip addr add 100.113.188.19$i/20 dev wlan0; 
#done



#!/bin/bash

echo "Calculating pps using tcpdump..."
start_time=$(date +%s.%N)
sudo taskset -c 0 tcpdump -i enp7s0 port 8000 -c 1000000
end_time=$(date +%s.%N) # Record end time
elapsed=$(echo "$end_time - $start_time" | bc)
pps=$(echo "1000000 / $elapsed" | bc)
echo "Packets Per Second(pps): $pps"


CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -algorithm delayA -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=VersionA && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -algorithm delayB -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=VersionB && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -algorithm delayC -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=VersionC && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -algorithm delayD -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=VersionD && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -algorithm standard_trimming -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=standard_trimming && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -fast_drop 1 -algorithm standard_trimming -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=standard_trimmingWithFastDrop && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -fast_drop 0  -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_256.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > outUEC.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=BTS && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

CMD="./htsim_ndp_entry_modern -o uec_entry -nodes 128 -cwnd 118500 -q 118500 -strat perm -linkspeed 100000 -mtu 2048 -seed 45 -hop_latency 700 -switch_latency 0 -goal incast_128_32_256.bin > uecComposite.tmp"
echo ${CMD}
eval ${CMD}
CMD="cd ../../plotting && python3 performance.py --name=NDP && cd ../sim/datacenter"
echo ${CMD}
eval ${CMD}

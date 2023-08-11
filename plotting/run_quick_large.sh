CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 112500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 0.5 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 100000 -mtu 2048 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_8_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.15 -y_gain 1 -w_gain 4 -z_gain 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=100G_8_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 112500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 0.5 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 100000 -mtu 2048 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.15 -y_gain 1 -w_gain 4 -z_gain 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=100G_32_1"
echo ${CMD}
eval ${CMD}

CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 852500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 0.5 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 800000 -mtu 2048 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_8_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.30 -y_gain 1 -w_gain 4 -z_gain 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=800G_8_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 852500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 0.5 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 800000 -mtu 2048 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.30 -y_gain 1 -w_gain 4 -z_gain 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=800G_32_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 1024 -q 852500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 0.5 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 800000 -mtu 2048 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_1024_100_600.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.30 -y_gain 1 -w_gain 4 -z_gain 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=800G_100_1"
echo ${CMD}
eval ${CMD}
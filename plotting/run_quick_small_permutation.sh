CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 112500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 1 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 100000 -mtu 4096 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal permutation_128_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.20 -y_gain 0.5 -w_gain 4 -z_gain 1 -use_super_fast_increase 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=100G_Perm_No_OS"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -algorithm delayB -nodes 64 -q 112500 -strat perm -kmin 50 -kmax 80 -target_rtt_percentage_over_base 50 -delay_gain_value_med_inc 1 -do_jitter 0 -jitter_value_med_inc 1 -use_fast_increase 1 -fast_drop 1 -do_exponential_gain 0 -gain_value_med_inc 1 -linkspeed 100000 -mtu 4096 -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal permutation_64_2000.bin -collect_data 1 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain 0.20 -y_gain 0.5 -w_gain 4 -z_gain 1 -use_super_fast_increase 1  > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --name=100G_Perm_OS"
echo ${CMD}
eval ${CMD}
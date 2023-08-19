X_GAIN=1.6
Y_GAIN=8.0
W_GAIN=1.25
Z_GAIN=1



CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 885200 -strat perm -kmin 20 -kmax 80 -target_rtt_percentage_over_base 20  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 1 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_4_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_4_1 --show_case=1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 885200 -strat perm -kmin 20 -kmax 80 -target_rtt_percentage_over_base 20  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 1 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_8_1 --show_case=1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 885200 -strat perm -kmin 20 -kmax 80 -target_rtt_percentage_over_base 20  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 1 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_16_1 --show_case=1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 128 -q 885200 -strat perm -kmin 20 -kmax 80 -target_rtt_percentage_over_base 20  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 1 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_32_1 --show_case=1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm delayB -nodes 1024 -q 885200 -strat perm -kmin 20 -kmax 80 -target_rtt_percentage_over_base 20  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 1 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_100_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 64 -q 885500 -strat perm -kmin 20 -target_rtt_percentage_over_base 20 -kmax 80  -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal permutation_64_2000.bin  -number_entropies -1 -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_Permutation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_Permutation_Small --show_case=1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 64 -q 885500 -strat perm -kmin 20 -target_rtt_percentage_over_base 20 -kmax 80  -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal permutation_64_20000.bin  -number_entropies -1 -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_Permutation_Large"
echo ${CMD}
eval ${CMD}
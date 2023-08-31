X_GAIN=0.25
Y_GAIN=1.25
W_GAIN=2
Z_GAIN=0.8


CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 115200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=100G_4_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_4_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_4_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 115200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=100G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_8_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 115200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=100G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_16_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 115200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=100G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_32_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 64 -q 118500 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ecmp_host_random2_ecn -number_entropies 256utation_64_2000.bin  -fast_drop 1 -k 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=100G_ecmp_host_random2_ecn -number_entropies 256utation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_ecmp_host_random2_ecn -number_entropies 256utation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_ecmp_host_random2_ecn -number_entropies 256_Small"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 64 -q 118500 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ecmp_host_random2_ecn -number_entropies 256utation_64_20000.bin  -fast_drop 1 -k 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > QuickExp/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=100G_ecmp_host_random2_ecn -number_entropies 256utation_Large"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=QuickExp"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=QuickExp --name=100G_ecmp_host_random2_ecn -number_entropies 256_Large"
echo ${CMD}
eval ${CMD}
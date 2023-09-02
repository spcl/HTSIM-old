X_GAIN=2
Y_GAIN=10
W_GAIN=2
Z_GAIN=0.8

FOLDER="FOLDER_800"
CMD="rm -r ${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}"
echo ${CMD}
eval ${CMD}

CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_4_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_4_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_4_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_8_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_16_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_32_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 1024 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_100_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_100_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_100_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 1024 -q 885200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_200_600.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_200_1"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_200_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_200_1"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 118500 -strat ecmp_host_random2_ecn  -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_2000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=800G_Permutation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_Permutation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_Permutation_Small"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 118500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -number_entropies 256 -goal  permutation_128_20000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_Permutation_Large"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_Permutation_Large"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 800000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_20000.bin  -fast_drop 1 -k 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=800G_Permutation_Large_NOOS"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER} --name=800G_Permutation_Large_NOOS"
echo ${CMD}
eval ${CMD}
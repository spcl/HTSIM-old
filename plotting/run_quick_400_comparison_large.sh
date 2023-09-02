X_GAIN=1
Y_GAIN=5
W_GAIN=2
Z_GAIN=0.8
FOLDER="FOLDER_400_COMPARISON_LARGE"

CMD="rm -r ${FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}"
echo ${CMD}
eval ${CMD}


SUB_FOLDER="4_1"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_4_1_SMaRTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_4_1_SMaRTT_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm ecn -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_4_1_ECN_ONLY"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm rtt -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_4_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_4_1_RTT_ONLY"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_4_1 --best_time=90.9"
echo ${CMD}
eval ${CMD}


SUB_FOLDER="8_1"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_8_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_8_1_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm ecn -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_8_1_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm rtt -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_8_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_8_1_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_8_1 --best_time=174"
echo ${CMD}
eval ${CMD}

SUB_FOLDER="16_1"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_16_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_16_1_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm ecn -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_16_1_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm rtt -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_16_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_16_1_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_16_1 --best_time=338.9"
echo ${CMD}
eval ${CMD}

SUB_FOLDER="32_1"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_32_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_32_1_Nocase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm ecn -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_32_1_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm rtt -nodes 128 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_128_32_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_32_1_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_32_1 --best_time=669.69"
echo ${CMD}
eval ${CMD}


SUB_FOLDER="100_1"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 1024 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_100_1"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 1024 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_100_1_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm ecn -nodes 1024 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_100_1_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm rtt -nodes 1024 -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_1024_100_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain ${X_GAIN}  -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -drop_value_buffer 1.0 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_100_1_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_100_1 --best_time=2071.4"
echo ${CMD}
eval ${CMD}

SUB_FOLDER="Perm_Small"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn  -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_2000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_Permutation_Small"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -disable_case_3 1 -nodes 128 -q 448500 -strat ecmp_host_random2_ecn  -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_2000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_Permutation_Small_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn  -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_2000.bin  -fast_drop 1 -algorithm ecn -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_Permutation_Small_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn  -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_2000.bin  -fast_drop 1 -algorithm rtt -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_complete.py --name=400G_Permutation_Small_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_Permutation_Small --best_time=90.88"
echo ${CMD}
eval ${CMD}

SUB_FOLDER="Perm_Large"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -number_entropies 256 -goal  permutation_128_20000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -disable_case_3 1 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -number_entropies 256 -goal  permutation_128_20000.bin  -fast_drop 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -number_entropies 256 -goal  permutation_128_20000.bin  -fast_drop 1 -algorithm ecn -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -k 4 -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -number_entropies 256 -goal  permutation_128_20000.bin  -fast_drop 1 -algorithm rtt -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_Permutation_Large --best_time=834"
echo ${CMD}
eval ${CMD}

SUB_FOLDER="Perm_Small_No_OS"
CMD="rm -r ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_20000.bin  -fast_drop 1 -k 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_NOOS"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -disable_case_3 1 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_20000.bin  -fast_drop 1 -k 1 -algorithm delayB -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_NOOS_NoCase3"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_20000.bin  -fast_drop 1 -k 1 -algorithm ecn -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_NOOS_ECN"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80  -linkspeed 400000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1  -number_entropies 256 -goal permutation_128_20000.bin  -fast_drop 1 -k 1 -algorithm rtt -x_gain ${X_GAIN} -y_gain ${Y_GAIN} -w_gain ${W_GAIN} -z_gain ${Z_GAIN} -bonus_drop 0.8 -collect_data 1 -use_fast_increase 1 -use_super_fast_increase 1 > ${FOLDER}/${SUB_FOLDER}/P"
echo ${CMD}
eval ${CMD}
CMD="python3 performance_simple.py --name=400G_Permutation_Large_NOOS_RTT"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --input_file=P --folder=${FOLDER}/${SUB_FOLDER} --name=400G_Permutation_Large_NOOS --best_time=215"
echo ${CMD}
eval ${CMD}

CMD="python3 comparison.py --folder=${FOLDER} --link_speed=400 --latency=700"
echo ${CMD}
eval ${CMD}
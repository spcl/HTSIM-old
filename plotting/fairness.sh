# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="FAIRNESS"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}

for Item in incast_128_8_2000.bin;
  do
    echo $Item
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 1 -algorithm delayB -nodes 1024 -q 4452000 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 0.25 -y_gain 1.25 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=3 --input_file=${FILE_NAME} --folder=${RES_FOLDER} --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecCompositeRTT.tmp"
    CMD="../sim/datacenter/htsim_uec_drop_entry -o uec_entry -k 1 -algorithm rtt -nodes 1024 -q 4452000 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 0.25 -y_gain 1.25 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=3 --input_file=${FILE_NAME} --folder=${RES_FOLDER}  --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecCompositeECN.tmp"
    CMD="../sim/datacenter/htsim_uec_drop_entry -o uec_entry -k 1 -algorithm ecn -nodes 1024 -q 4452000 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 0.25 -y_gain 1.25 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=3 --input_file=${FILE_NAME} --folder=${RES_FOLDER}  --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecCompositeNoFairness.tmp"
    CMD="../sim/datacenter/htsim_uec_drop_entry -o uec_entry -k 1 -algorithm delayB -disable_case_3 1 -nodes 1024 -q 4452000 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 100000 -mtu 2048 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 0.25 -y_gain 1.25 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=3 --input_file=${FILE_NAME} --folder=${RES_FOLDER}  --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
  done

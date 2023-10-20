# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="PERMUTATION_OS_800_32MiB_128_ROUTING"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}

for Item in permutation_128_33554432.bin;
  do
    echo $Item
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm delayB -nodes 128 -q 4452000 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 4096 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal permutation/${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 1.25 -y_gain 5 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=2 --input_file=${FILE_NAME} --folder=${RES_FOLDER} --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm delayB -nodes 128 -q 4452000 -strat ecmp_host -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 4096 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal permutation/${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 1.25 -y_gain 5 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=2 --input_file=${FILE_NAME} --folder=${RES_FOLDER}  --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
     FILE_NAME="uecCompositeRoute.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm delayB -nodes 128 -q 4452000 -strat ecmp_host_random_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed 800000 -mtu 4096 -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal permutation/${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain 1.25 -y_gain 5 -w_gain 2 -z_gain 0.8 -bonus_drop 0.8 -collect_data 0 -drop_value_buffer 1.0 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=2 --input_file=${FILE_NAME} --folder=${RES_FOLDER}  --degree=${Item}"
    echo ${CMD}
    eval ${CMD}
  done

CMD="python3 cdf_plot_reps.py --bdp=864000 --folder=${RES_FOLDER} --incast_degree=8 --latency=700 --link_speed=100"
echo ${CMD}
eval ${CMD}
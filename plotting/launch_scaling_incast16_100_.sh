# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="SCALING_INCAST_16_100"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}


for Item in incast_128_16_2.bin incast_128_16_4.bin incast_128_16_8.bin incast_128_16_16.bin incast_128_16_32.bin incast_128_16_64.bin incast_128_16_128.bin incast_128_16_256.bin incast_128_16_512.bin;
  do
    echo $Item
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -bonus_drop 0.8 -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 44 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal ${Item} -number_entropies -1 -fast_drop 1 -algorithm delayB -x_gain 0.2 -y_gain 1.25 -w_gain 2 -z_gain 0.8 -use_fast_increase 1 -use_super_fast_increase 1 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="ndp.tmp"
    CMD="../sim/datacenter/htsim_ndp_entry_modern -o uec_entry -nodes 128 -cwnd 118500 -q 118500 -strat perm -linkspeed 100000 -mtu 512 -seed 44 -hop_latency 700 -switch_latency 0 -goal ${Item} > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
  done

CMD="python3 scaling_normalized.py --bdp=118000 --folder=${RES_FOLDER} --incast_degree=16 --latency=700 --link_speed=100"
echo ${CMD}
eval ${CMD}

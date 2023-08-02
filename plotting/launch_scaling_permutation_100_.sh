RES_FOLDER="SCALING_PERM_100"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}


for Item in perm_across_64_2.bin perm_across_64_4.bin perm_across_64_8.bin perm_across_64_16.bin perm_across_64_32.bin perm_across_64_64.bin perm_across_64_128.bin perm_across_64_256.bin perm_across_64_512.bin perm_across_64_768.bin perm_across_64_1024.bin perm_across_64_1524.bin perm_across_64_2048.bin;
  do
    echo $Item
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecCompositeBTS.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern_os -k 4 -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
    CMD="../sim/datacenter/htsim_ndp_entry_modern_os -o uec_entry -nodes 128 -cwnd 118500 -q 118500 -strat perm -linkspeed 100000 -mtu 2048 -seed 44 -hop_latency 700 -switch_latency 0 -goal ${Item} > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
  done

CMD="python3 scaling.py --bdp=118000 --folder=${RES_FOLDER} --incast_degree=8"
echo ${CMD}
eval ${CMD}

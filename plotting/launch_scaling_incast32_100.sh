# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="SCALING_INCAST_100"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}


for Item in incast_128_32_2.bin incast_128_32_4.bin incast_128_32_8.bin incast_128_32_16.bin incast_128_32_24.bin incast_128_32_28.bin incast_128_32_32.bin incast_128_32_36.bin incast_128_32_48.bin incast_128_32_64.bin incast_128_32_128.bin incast_128_32_256.bin incast_128_32_512.bin;
  do
    echo $Item
    FILE_NAME="uecComposite.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 48 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
    FILE_NAME="uecCompositeBTS.tmp"
    CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 48 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Item} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
    echo ${CMD}
    eval ${CMD}
    CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
    echo ${CMD}
    eval ${CMD}
  done

CMD="python3 scaling.py --folder=SCALING_INCAST_100 --bdp=118000 --folder=${RES_FOLDER} --incast_degree=32"
echo ${CMD}
eval ${CMD}

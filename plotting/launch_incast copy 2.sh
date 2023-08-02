# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="INCAST_SMALL"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecComposite.tmp"

CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_100.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecCompositeBTS.tmp"
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal incast_128_32_100.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="ndp.tmp"
CMD="../sim/datacenter/htsim_ndp_entry_modern -o uec_entry -nodes 128 -cwnd 118500 -q 118500 -strat perm -linkspeed 100000 -mtu 2048 -hop_latency 700 -switch_latency 0 -goal incast_128_32_100.bin > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 performance.py --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}

CMD="python3 bw.py --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 fct.py --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="python3 events.py --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="PERM_400_OS_SMALL"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecComposite.tmp"

CMD="../sim/datacenter/htsim_uec_entry_modern_os -o uec_entry -k 4 -nodes 64 -q 432500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048 -seed 44 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_400.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecCompositeBTS.tmp"
CMD="../sim/datacenter/htsim_uec_entry_modern_os -o uec_entry -k 4 -nodes 64 -q 432500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048 -seed 44 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_400.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="ndp.tmp"
CMD="../sim/datacenter/htsim_ndp_entry_modern_os -o uec_entry -nodes 128 -cwnd 432500 -q 432500 -strat perm -linkspeed 400000 -mtu 2048 -seed 44 -hop_latency 700 -switch_latency 0 -goal perm_across_64_400.bin > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
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


# Incast MEDIUM, 100GB BW - 400NS Latency
RES_FOLDER="PERM_400_OS_LARGE"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecComposite.tmp"

CMD="../sim/datacenter/htsim_uec_entry_modern_os -o uec_entry -k 4 -nodes 64 -q 432500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048 -seed 44 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="uecCompositeBTS.tmp"
CMD="../sim/datacenter/htsim_uec_entry_modern_os -o uec_entry -k 4 -nodes 64 -q 432500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048 -seed 44 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
FILE_NAME="ndp.tmp"
CMD="../sim/datacenter/htsim_ndp_entry_modern_os -o uec_entry -nodes 128 -cwnd 432500 -q 432500 -strat perm -linkspeed 400000 -mtu 2048 -seed 44 -hop_latency 700 -switch_latency 0 -goal perm_across_64_2000.bin > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER}"
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

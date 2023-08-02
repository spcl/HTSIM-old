# Incast Small, 100GB BW - 400NS Latency
EXP_NAME="INCAST_SMALL"
CMD="mkdir ${EXP_NAME}"
CMD="./htsim_uec_entry_modern_os -o uec_entry -nodes 64 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -k 4 -number_entropies -1 > uecComposite.tmp"
eval ${PLOT_STRING}
CMD="generate_report.py --input_file=test"
CMD="./htsim_uec_entry_modern_os -o uec_entry -nodes 64 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -k 4 -number_entropies -1 > uecCompositeBTS.tmp"
CMD="generate_report.py --input_file=test"
CMD="./htsim_uec_entry_modern_os -o uec_entry -nodes 64 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal perm_across_64_2000.bin -ignore_ecn_data 1 -ignore_ecn_ack 1 -k 4 -number_entropies -1 > ndp.tmp"
CMD="generate_report.py --input_file=test"
CMD="bw.py --input=test"
CMD="bw.py --input=test"
CMD="bw.py --input=test"

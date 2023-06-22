
PROGRAM_NAME="UEC_OS_OneLink50"
eval "mkdir -p" result/$PROGRAM_NAME
for N in -1
#for N in -1 2 4 8 16 32 64 128 256 1024 2048 4096 8192 16384
do
	CMD="./htsim_uec_entry_modern_os -o uec_entry -nodes 512 -k 8 -q 168500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048 -hop_latency 400 -switch_latency 0 -reuse_entropy 1 -number_entropies $N -queue_type composite -bts_trigger 80 -goal permutation_across_512_2mb.bin > result/${PROGRAM_NAME}/${N}.tmp"
        echo ${CMD}
        eval ${CMD}
done
#BEST_TIME=(((2000000 / 2048) * (2048 + 64)) / 400)
BEST_TIME=171000
echo ${BEST_TIME}
PLOT_STRING="python3 ../../plotting/relative_perf.py ${PROGRAM_NAME} ${BEST_TIME}"
eval ${PLOT_STRING}




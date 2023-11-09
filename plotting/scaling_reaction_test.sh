# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="SCALING_PRECISION_INCAST_SMALL"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}


for Item in incast_1024_8_1048576.bin;
  do
  for Precision in 1 10 50 100 250 500 750 1000;
      do
        echo $Item
        FILE_NAME="uecComposite.tmp"
        CMD="../sim/datacenter/htsim_uec_entry_modern -bonus_drop 0.8 -o uec_entry -nodes 1024 -q 118500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80 -linkspeed 800000 -mtu 4096 -seed 44 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal incast_normal/${Item} -number_entropies 256 -fast_drop 1 -algorithm delayB -x_gain 2 -y_gain 2.5 -w_gain 2 -z_gain 0.8 -use_fast_increase 1 -use_super_fast_increase 1 -collect_data 0 -precision_ts ${Precision} > ${RES_FOLDER}/${FILE_NAME}"
        echo ${CMD}
        eval ${CMD}
        CMD="python3 generate_report.py --scaling_plot=1 --input_file=${FILE_NAME} --folder=${RES_FOLDER} --precision_reaction ${Precision}"
        echo ${CMD}
        eval ${CMD}
      done
  done

CMD="python3 scaling_precision.py --bdp=118000 --folder=${RES_FOLDER} --incast_degree=8 --latency=700 --link_speed=800"
echo ${CMD}
eval ${CMD}

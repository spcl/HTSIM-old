# Incast Small, 100GB BW - 400NS Latency
RES_FOLDER="Parameter_Analysis_Incast_64_2000_100G"
CMD="rm -r ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="mkdir ${RES_FOLDER}"
echo ${CMD}
eval ${CMD}

# Threee Incast Sizes
for Incast_Message_Size in incast_128_64_2000.bin
do
    for KMin in 20 50
    do
        for UseFastDrop in 0 1
        do
            for UseFastIncrease in 0 1
            do
                for DoExpGain in 0 1
                do
                    for ExpGainValue in 1
                    do
                        for Do_Jitter in 0 1
                        do
                            for Jitter_value in 1
                            do
                                for DelayGainvalue in 0 2 5
                                do
                                    for Version in delayA standard_trimming
                                    do
                                        FILE_NAME="UEC_${Incast_Message_Size}_Version${Version}_KMin${KMin}_UseFastDrop${UseFastDrop}_UseFastIncrease${UseFastIncrease}_DoExpGain${DoExpGain}_DoJitter${Do_Jitter}_DelayGainvalue${DelayGainvalue}"
                                        CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -algorithm ${Version} -nodes 128 -q 118500 -strat perm -kmin ${KMin} -kmax 80 -target_rtt_percentage_over_base ${KMin} -delay_gain_value_med_inc ${DelayGainvalue} -do_jitter ${Do_Jitter} -jitter_value_med_inc ${Jitter_value} -use_fast_increase ${UseFastIncrease} -fast_drop ${UseFastDrop} -do_exponential_gain ${DoExpGain} -gain_value_med_inc ${ExpGainValue} -linkspeed 100000 -mtu 2048 -seed 99 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Incast_Message_Size} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
                                        echo ${CMD}
                                        eval ${CMD}
                                        CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER} --parameter_analysis=1 --complex_name=${FILE_NAME}"
                                        echo ${CMD}
                                        eval ${CMD}
                                        #CMD="python3 performance.py --name=${FILE_NAME} --no_show=1 --output_folder=${RES_FOLDER}"
                                        echo ${CMD}
                                        eval ${CMD}
                                        CMD="rm ${RES_FOLDER}/${FILE_NAME}"
                                        echo ${CMD}
                                        eval ${CMD}
                                    done
                                done
                            done
                        done
                    done
                done
            done
        done
    done
done

FILE_NAME="uecCompositeBTS.tmp"
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 118500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 99 -queue_type composite_bts -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Incast_Message_Size} -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER} --parameter_analysis=1 --complex_name=${FILE_NAME}"
echo ${CMD}
eval ${CMD}
#CMD="python3 performance.py --name=${FILE_NAME} --no_show=1 --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="rm ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}

FILE_NAME="ndp.tmp"
CMD="../sim/datacenter/htsim_ndp_entry_modern -o uec_entry -nodes 128 -cwnd 118500 -q 118500 -strat perm -linkspeed 100000 -mtu 2048 -seed 99 -hop_latency 700 -switch_latency 0 -goal ${Incast_Message_Size} > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER} --parameter_analysis=1 --complex_name=${FILE_NAME}"
echo ${CMD}
eval ${CMD}
#CMD="python3 performance.py --name=${FILE_NAME} --no_show=1 --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="rm ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}

FILE_NAME="strack.tmp"
CMD="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -nodes 128 -q 112500 -strat perm -kmin 20 -kmax 80 -linkspeed 100000 -mtu 2048 -seed 45 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal ${Incast_Message_Size} -algorithm delayD -fast_drop 0 -ignore_ecn_data 1 -ignore_ecn_ack 1 -number_entropies -1 -fast_drop 1 > ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}
CMD="python3 generate_report.py --input_file=${FILE_NAME} --folder=${RES_FOLDER} --parameter_analysis=1 --complex_name=${FILE_NAME}"
echo ${CMD}
eval ${CMD}
#CMD="python3 performance.py --name=${FILE_NAME} --no_show=1 --output_folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
CMD="rm ${RES_FOLDER}/${FILE_NAME}"
echo ${CMD}
eval ${CMD}

CMD="python3 ranking.py --folder=${RES_FOLDER}"
echo ${CMD}
eval ${CMD}
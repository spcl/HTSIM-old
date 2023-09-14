import os

# Overall Variables
X_GAIN=0.5
Y_GAIN=2.5
W_GAIN=2
Z_GAIN=0.8
MTU_SIZE=4096
LINK_SPEED_VALUE=400000
COLLECT_DATA=0

# Test Name
FOLDER="FOLDER_400_FULL_TEST_PERMUTATION_OS"
CMD="rm -r {}".format(FOLDER)
os.system(CMD)
CMD="mkdir {}".format(FOLDER)
os.system(CMD)

# List Experiments
list_experiments = ["permutation_128_16.bin", "permutation_128_512.bin", "permutation_128_4000.bin", "permutation_128_16000.bin"]
list_algorithm = ["SMaRTT", "SMaRTT_ECN", "SMaRTT_RTT", "Swift", "EQDS"]

def getRunString(algo, exp):
    num_nodes = 128
    if ("1024" in exp):
        num_nodes = 1024

    if (algo == "SMaRTT"):
            string_to_run="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm delayB -nodes {} -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed {} -mtu {} -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal {} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain {}  -y_gain {} -w_gain {} -z_gain {} -bonus_drop 0.8 -collect_data {} -drop_value_buffer 1.0 > {}/{}/P".format(num_nodes, LINK_SPEED_VALUE, MTU_SIZE, exp, X_GAIN, Y_GAIN, W_GAIN, Z_GAIN, COLLECT_DATA, FOLDER, SUB_FOLDER)
    elif (algo == "SMaRTT_ECN"):
        string_to_run="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm ecn -nodes {} -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed {} -mtu {} -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal {} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain {}  -y_gain {} -w_gain {} -z_gain {} -bonus_drop 0.8 -collect_data {} -drop_value_buffer 1.0 > {}/{}/P".format(num_nodes, LINK_SPEED_VALUE, MTU_SIZE, exp, X_GAIN, Y_GAIN, W_GAIN, Z_GAIN, COLLECT_DATA, FOLDER, SUB_FOLDER)
    elif (algo == "SMaRTT_RTT"):
        string_to_run="../sim/datacenter/htsim_uec_entry_modern -o uec_entry -k 4 -algorithm rtt -nodes {} -q 445200 -strat ecmp_host_random2_ecn -number_entropies 256 -kmin 20 -kmax 80 -target_rtt_percentage_over_base 50  -use_fast_increase 1 -use_super_fast_increase 1 -fast_drop 1 -linkspeed {} -mtu {} -seed 919 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 1 -goal {} -ignore_ecn_data 1 -ignore_ecn_ack 1 -x_gain {}  -y_gain {} -w_gain {} -z_gain {} -bonus_drop 0.8 -collect_data {} -drop_value_buffer 1.0 > {}/{}/P".format(num_nodes, LINK_SPEED_VALUE, MTU_SIZE, exp, X_GAIN, Y_GAIN, W_GAIN, Z_GAIN, COLLECT_DATA, FOLDER, SUB_FOLDER)
    elif (algo == "Swift"):
        string_to_run="../sim/datacenter/htsim_swift_entry -k 4 -ratio_os_stage_1 1 -o uec_entry -nodes {} -q 448500 -strat ecmp_host_random2_ecn -kmin 20 -target_rtt_percentage_over_base 50 -kmax 80 -linkspeed {} -mtu {} -seed 95 -queue_type composite -hop_latency 700 -switch_latency 0 -reuse_entropy 0 -goal {} -number_entropies 256  -collect_data {}  > {}/{}/P".format(num_nodes, LINK_SPEED_VALUE, MTU_SIZE, exp, COLLECT_DATA, FOLDER, SUB_FOLDER)
    elif (algo == "EQDS"):
        string_to_run="../sim/datacenter/htsim_ndp_entry_modern -o uec_entry -nodes {} -k 4 -kmin 20 -kmax 80 -ratio_os_stage_1 1 -cwnd 448500 -q 448500 -strat ecmp_host_random2_ecn -linkspeed {} -mtu {} -seed 95 -hop_latency 700 -switch_latency 0 -goal {} -collect_data {} -number_entropies 256 > {}/{}/P".format(num_nodes, LINK_SPEED_VALUE, MTU_SIZE, exp, COLLECT_DATA, FOLDER, SUB_FOLDER)

    return  string_to_run

# No OverSub Experiments
for experiment in list_experiments:
    # Create Folder
    SUB_FOLDER=experiment.replace(".", "_" )
    CMD="rm -r {}/{}".format(FOLDER, SUB_FOLDER)
    os.system(CMD)
    CMD="mkdir {}/{}".format(FOLDER, SUB_FOLDER)
    os.system(CMD)
    for algo in list_algorithm:

        # Run Experiment
        CMD=getRunString(algo, experiment)
        os.system(CMD)
        #CMD="python3 performance_complete.py --name=400G_{}_{}".format(experiment, algo)
        #os.system(CMD)
        CMD="python3 generate_report.py --input_file=P --folder={}/{}".format(FOLDER, SUB_FOLDER)
        os.system(CMD)

    # Plot Overall Results
    CMD="python3 fct.py --input_file=P --folder={}/{} --name={}".format(FOLDER, SUB_FOLDER, experiment)
    os.system(CMD)

CMD="python3 comparison.py --folder={} --link_speed=400 --latency=700  --os=4".format(FOLDER)
print(CMD)
os.system(CMD)

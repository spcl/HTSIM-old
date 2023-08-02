import re
import numpy as np
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import argparse


list_fct = []
list_size = []
num_bts_warning = 0
num_bts_dropped = 0
num_ecn = 0
num_nack = 0
bw_speed_gbps = 0
incast_size = 0
min_bw  = 0
name = ""

parser = argparse.ArgumentParser()
parser.add_argument('--input_file', dest='input_file', type=str, help='File to parse.')
parser.add_argument('--folder', dest='folder', type=str, help='Folder to parse and save')
parser.add_argument('--scaling_plot', dest='scaling_plot', type=str, help='Scaling Plot Option', default=None)
args = parser.parse_args()

folder = args.folder
file_name = args.input_file

with open(folder + "/" + file_name) as file:
    for line in file:
        # Name
        if "Name Running: " in line:
            name = line.split(': ')[1]
        

        # BW
        result = re.search(r"Speed is (\d+)", line)
        if result:
            bw_speed_gbps = int(result.group(1)) / 1000

        # FCT
        #result = re.search(r"Host (\d+): (\d+)", line)
        result = re.search(r"Completion Time Flow is (\d+)", line)
        if result:
            fct = int(result.group(1))
            list_fct.append(round(fct / 1000))

        # BTS, ECN, NACK
        if "- Warning -" in line:
            num_bts_warning += 1
        if "- Queue is full -" in line:
            num_bts_dropped += 1
        if "Packet is ECN Marked 1 -" in line:
            num_ecn += 1
        if "NACK " in line:
            num_nack += 1

        # Size
        result = re.search(r"of size (\d+) from ", line)
        if result:
            size_m = int(result.group(1))
            list_size.append(round(size_m))
            incast_size = int(result.group(1))

    # MIN BW
    max_time = max(list_fct)
    min_bw = (incast_size * 8 + (incast_size*8*0.03)) / (max_time - 8500)

        

if (args.scaling_plot is not None and int(args.scaling_plot) == 1):
    file_name = folder + '/GeneratedReport{}_{}.tmp'.format(name.rstrip(), size_m)
else:
    file_name = folder + '/GeneratedReport{}.tmp'.format(name.rstrip())

with open(file_name, 'w') as f:
    # Name
    f.write('Name: {}\n'.format(name)) 

    # BW
    f.write('Theo BW: {}\n'.format(bw_speed_gbps)) 

    # Size Messages
    b = [0] * (len(list_fct) - len(list_size))
    list_size.extend(b)
    for size_m in list_size:
        f.write('Size: {}\n'.format(size_m)) 

    # FCT
    for fct_ele in list_fct:
        f.write('FCT: {}\n'.format(fct_ele)) 
    
    # BTS, ECN, NACK
    f.write('BTS Warning: {}\n'.format(num_bts_warning)) 
    f.write('BTS Dropped: {}\n'.format(num_bts_dropped)) 
    f.write('ECN: {}\n'.format(num_ecn)) 
    f.write('NACK: {}\n'.format(num_nack)) 

    # Effective BW
    for idx, item in enumerate(list_fct):
        if (list_fct[idx] == 0):
            eff_bw = 0
        else:
            eff_bw = (list_size[idx] * 8 / list_fct[idx])
        f.write('Effective BW: {}\n'.format(eff_bw)) 

    # Incast Size, Scalign Plot
    f.write('Incast Size: {}\n'.format(incast_size)) 

    # Min BW
    f.write('Min BW: {}\n'.format(min_bw))     

    # Max FCT
    f.write('Max FCT: {}\n'.format(max(list_fct)))  

    
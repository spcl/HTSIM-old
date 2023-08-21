import re
import numpy as np
import pandas as pd
import seaborn as sns
from pathlib import Path
import matplotlib.pyplot as plt
import argparse


list_names = []
df = pd.DataFrame()
i = 0

bw_ndp = {}
bw_bts = {}
bw_trimming = {}
size_incast = 0

parser = argparse.ArgumentParser()
parser.add_argument('--input_file', dest='input_file', type=str, help='File to parse.')
parser.add_argument('--folder', dest='folder', type=str, help='Folder to parse and save')
parser.add_argument('--bdp', dest='bdp', type=int, help='BDP Value', default=0)
parser.add_argument('--incast_degree', dest='incast_degree', type=int, help='BDP Value', default=1)
parser.add_argument('--link_speed', dest='link_speed', type=int, help='In Gbps', default=1)
parser.add_argument('--latency', dest='latency', type=int, help='Latency in NS', default=1)
parser.add_argument('--name', dest='name', type=int, help='Name', default=1)




args = parser.parse_args()
folder = args.folder
file_name = args.input_file
my_bw = 0

def average(lst):
    return sum(lst) / len(lst)

pathlist = Path(folder).glob('**/*.tmp')
for files in sorted(pathlist):
    if ("GeneratedReport" not in str(files)):
        continue
    with open(files) as file:
        for line in file:
            # Theo BW
            result = re.search(r"Theo BW: (\d+)", line)
            if result:
                my_bw = int(result.group(1))
            # Size
            result = re.search(r"Incast Size: (\d+)", line)
            if result:
                size_incast = int(result.group(1))
                # Theoretical BW
                latency = args.latency
                #size_incast = round(size_incast / 2048) * 2048
                #theoretical_bw = ((((size_incast * 0.03125) + size_incast ) * 8) * args.incast_degree / args.link_speed) + (latency * 12) + (64*8*12/args.link_speed) + (2048*8*(args.incast_degree-1)/args.link_speed)
                theoretical_bw = ((((size_incast * 0.03125) + size_incast ) * 8) * args.incast_degree / args.link_speed) + (latency * 6) + (2112*8*6/args.link_speed) + (latency * 6) +  (64*8*6/args.link_speed)
                #theoretical_bw = ((((size_incast * 0.031) + size_incast ) * 8) * args.incast_degree / 100) + (latency * 12) + (64*8*12/100)
            # BW
            result = re.search(r"Max FCT: (\d+.\d+)", line)
            if result:
                bw_1 = float(result.group(1))
                print(size_incast * 1000)
                print(bw_1)
                print(theoretical_bw)
                print()
                bw_1 = 1 /(bw_1 / theoretical_bw)
                if (bw_1 > 1):
                    bw_1 = 1
                if ("NDP" in str(files)):
                    bw_ndp[size_incast / 1000] = bw_1
                elif ("BTS" in str(files)):
                    bw_bts[size_incast / 1000] = bw_1
                elif ("UEC" in str(files)):
                    bw_trimming[size_incast / 1000] = bw_1
        

bw_bts = dict(sorted(bw_bts.items()))
bw_trimming = dict(sorted(bw_trimming.items()))
bw_ndp = dict(sorted(bw_ndp.items()))
print(bw_ndp)
print(bw_trimming)

# Calculate percentage difference between Line 2 and Line 1
y1 = list(bw_bts.values())
y2 = list(bw_trimming.values())
percentage_diff = [((y2_val - y1_val) / y1_val) * 100 for y1_val, y2_val in zip(y2, y1)]

sns.set(style="darkgrid")

# Increase the size of the plot
plt.figure(figsize=(10, 6))
# Plot Line 1
x1 = list(bw_bts.keys())
y1 = list(bw_bts.values())

# Plot Line 2
x2 = list(bw_trimming.keys())
y2 = list(bw_trimming.values())
plt.plot(x2, y2, marker='s', label='UEC Trimming')

# Plot Line 3
x3 = list(bw_ndp.keys())
y3 = list(bw_ndp.values())
plt.plot(x3, y3, marker='x', label='NDP')

x_vertical_line = args.bdp / 1000
plt.axvline(x_vertical_line, color='gray', linestyle='dashed', alpha=0.3)

# Add a horizontal dashed line at y=7 with low opacity
#y_horizontal_line = (((size_incast * 0.03) + size_incast) * 8) / ()
#print(my_bw)
#plt.axhline(y_horizontal_line, color='gray', linestyle='dashed', alpha=0.3)

# Add text annotation "Max BW" next to the horizontal line
#plt.text(max(x2) - 0.1, y_horizontal_line, "Max Theoretical BW", color='gray', fontsize=10,
#         ha='right', va='bottom')

# Set x-axis to log2 scale
plt.xscale('log', base=2)

# Set plot labels and title
plt.xlabel('Message Size (KiB)')
plt.ylabel('Normalized Performance to Theoretical Best')
plt.title('Scaling During {}:1 Incast - Link Speed {}Gbps - 2048B MTU'.format(args.incast_degree, args.link_speed))
plt.legend()

 
# Make boxplot for one group only
plt.tight_layout()
plt.savefig(folder + "/scaling.png", bbox_inches='tight')


plt.show()
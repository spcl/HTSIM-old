import re
import numpy as np
import pandas as pd
import seaborn as sns
from pathlib import Path
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser()
parser.add_argument('--input_file', dest='input_file', type=str, help='File to parse.')
parser.add_argument('--folder', dest='folder', type=str, help='Folder to parse and save')
parser.add_argument('--name', dest='name', type=str, help='name to save', default=None)
parser.add_argument('--best_time', dest='best_time', type=float, help='Best Possible Time', default=None)


args = parser.parse_args()
folder = args.folder
file_name = args.input_file

list_fct = []
list_names = []
df = pd.DataFrame()
i = 0

pathlist = Path(folder).glob('**/*.tmp')
print(folder)
for files in sorted(pathlist):
    if ("GeneratedReport" not in str(files)):
        continue
    with open(files) as file:
        
        list_fct.append([])
        for line in file:
            # Name 
            if "Name: " in line:
                name = line.split(': ')[1]
                list_names.append(name)
            # FCT
            result = re.search(r"FCT: (\d+)", line)
            if result:
                fct = int(result.group(1))
                list_fct[i].append(fct / 1000)
        i += 1

# set a grey background (use sns.set_theme() if seaborn version 0.11.0 or above) 
sns.set(style="darkgrid")

print(list_names)

combined_data = []
hue_list = []
for idx, names in enumerate(list_names):
    combined_data += list_fct[idx]
    hue_list += [list_names[idx]] * len(list_fct[idx])

# Combine the data and create a 'hue' column to differentiate between the two groups
# combined_data = list_fct[0] + list_fct[1]
#hue_list = ['Group 1'] * len(list_fct[0]) + ['Group 2'] * len(list_fct[1])

# Create the violin plot
my = sns.violinplot(x=hue_list, y=combined_data, cut=0)

yticks, ylabels = plt.yticks()
xticks, xlabels = plt.xticks()

# set the x-axis ticklabel size
my.set_xticklabels(xlabels, size=9)

#my.axhline(args.best_time, ls='--', color='black', linewidth=1.5, alpha=.6)


my.set_title('Individual Flow Completion Time')
my.set_ylabel('FCT (us)')
my.set_xlabel('Sender Based Trimming UEC')
 
# Make boxplot for one group only
plt.tight_layout()
if (args.name is not None):
    plt.savefig(folder + "/{}.png".format(args.name), bbox_inches='tight')
else:
    plt.savefig(folder + "/fct.png", bbox_inches='tight')
#plt.show()



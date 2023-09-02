import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
import re
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
import argparse


def getBestTheoretical(name, size):
    if ("4_1" in name):
        return (size+size*0.03125)*8*4/args.link_speed/1000+(args.latency/1000*12)
    elif ("8_1" in name):
        return (size+size*0.03125)*8*8/args.link_speed/1000+(args.latency/1000*12)
    elif ("16_1" in name):
        return (size+size*0.03125)*8*16/args.link_speed/1000+(args.latency/1000*12)
    elif ("32_1" in name):
        return (size+size*0.03125)*8*32/args.link_speed/1000+(args.latency/1000*12)
    elif ("100_1" in name):
        return (size+size*0.03125)*8*100/args.link_speed/1000+(args.latency/1000*12)
    elif ("200_1" in name):
        return (size+size*0.03125)*8*200/args.link_speed/1000+(args.latency/1000*12)
    elif ("Perm_Small_No_OS" == name):
        return (size+size*0.03125)*8*1/args.link_speed/1000+(args.latency/1000*12)
    elif ("Perm_Small" == name):
        return (size+size*0.03125)*8*1/(args.link_speed/4)/1000+(args.latency/1000*12)
    elif ("Perm_Large" == name):
        return (size+size*0.03125)*8*1/(args.link_speed/4)/1000+(args.latency/1000*12)



parser = argparse.ArgumentParser()
parser.add_argument('--input_file', dest='input_file', type=str, help='File to parse.')
parser.add_argument('--folder', dest='folder', type=str, help='Folder to parse and save')
parser.add_argument('--name', dest='name', type=str, help='name to save', default=None)
parser.add_argument('--link_speed', dest='link_speed', type=int, help='LinkSpeed', default=None)
parser.add_argument('--latency', dest='latency', type=int, help='Best Possible Time', default=None)
parser.add_argument('--size', dest='size', type=int, help='Best Possible Time', default=None)


args = parser.parse_args()

# Define the path to the main folder
main_folder_path = Path(args.folder)
list_fct = []
list_category = []
list_group = []
size = 0
# Iterate through the subfolders inside the main folder
for subfolder in main_folder_path.iterdir():
    if subfolder.is_dir():
        print(f"Subfolder: {subfolder}")
        
        # Iterate through the contents of each subfolder
        for item in subfolder.iterdir():
            if item.is_file() and "Generated" in str(item):
                print(f"  File: {item}")
                # Open and read the file line by line
                with open(item, 'r') as file:
                    for line in file:
                        result = re.search(r"Size: (\d+)", line)
                        if result:
                            size = int(result.group(1))
                        result = re.search(r"Max FCT: (\d+)", line)
                        if result:
                            fct = int(result.group(1))
                            str_subfolder = str(subfolder)
                            str_subfolder = str_subfolder.split('/', 1)[-1]
                            result = getBestTheoretical(str_subfolder,size) / (fct / 1000)  * 100
                            list_fct.append(result)
                            if ("Perm" not in str_subfolder):
                                str_subfolder = "Incast_" + str_subfolder
                            list_category.append(str_subfolder)
                        if "Name: " in line:
                            line = line.split('Name: ', 1)[-1]
                            list_group.append(str(line.rstrip()))

print(list_fct)
print(list_group)
print(list_category)
data = {
    'Category': list_category,
    'Value': list_fct,
    'Group': list_group
}

# Create a DataFrame
df = pd.DataFrame(data)

# Create a bar chart with multiple bars grouped by 'Group' within each 'Category'
sns.set(style="darkgrid")

plt.figure(figsize=(18, 12))  # Adjust the figure size (optional)

# Extract the numeric part of the 'Category' column and convert it to integers
def extract_numeric(category):
    match = re.search(r'\d+', category)
    if match:
        return int(match.group())
    return 0  # Return 0 if no numeric part is found

df['Category_Order'] = df['Category'].apply(extract_numeric)

# Sort the DataFrame by 'Category_Order' for proper ordering
df = df.sort_values(by='Category_Order')
sns.barplot(data=df, x='Category', y='Value', hue='Group')
plt.ylim(70, plt.ylim()[1])

plt.title('Performance Relative to Best Theoretical One (Higher is Better)')
plt.xlabel('Experiment')
plt.ylabel('% of Best Theoretical Runtime')
plt.legend()  # Add a legend for the groups

plt.tight_layout()

plt.savefig(args.folder + "/summary.png", bbox_inches='tight')
#plt.show()



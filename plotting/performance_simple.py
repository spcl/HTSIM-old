import plotly.express as px
import pandas as pd
from pathlib import Path
import plotly.graph_objs as go
import plotly
from plotly.subplots import make_subplots
from datetime import datetime
import os
import re
import natsort 
from argparse import ArgumentParser


# Parameters
skip_small_value = True
ECN = True

def main(args):
    # Clean Data and Copy Data
    os.system("rm -r /home/tommaso/csg-htsim/plotting/queue_size_normalized/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/rtt/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/cwd/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/ecn/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/sent/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/nack/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/acked/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/fasti/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/fastd/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/mediumi/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/trimmed_rtt/")
    os.system("rm -r /home/tommaso/csg-htsim/plotting/ecn_rtt/")

    os.system("cp -a /home/tommaso/csg-htsim/sim/output/cwd/. /home/tommaso/csg-htsim/plotting/cwd/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/rtt/. /home/tommaso/csg-htsim/plotting/rtt/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/queue/. /home/tommaso/csg-htsim/plotting/queue_size_normalized/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/sent/. /home/tommaso/csg-htsim/plotting/sent/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/ecn/. /home/tommaso/csg-htsim/plotting/ecn/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/nack/. /home/tommaso/csg-htsim/plotting/nack/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/fasti/. /home/tommaso/csg-htsim/plotting/fasti/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/fastd/. /home/tommaso/csg-htsim/plotting/fastd/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/mediumi/. /home/tommaso/csg-htsim/plotting/mediumi/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/acked/. /home/tommaso/csg-htsim/plotting/acked/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/trimmed_rtt/. /home/tommaso/csg-htsim/plotting/trimmed_rtt/")
    os.system("cp -a /home/tommaso/csg-htsim/sim/output/ecn_rtt/. /home/tommaso/csg-htsim/plotting/ecn_rtt/")

    # RTT Data
    colnames=['Time', 'RTT', 'seqno', 'ackno']
    df = pd.DataFrame(columns =['Time','RTT', 'seqno', 'ackno'])
    name = ['0'] * df.shape[0]
    df = df.assign(Node=name)

    pathlist = Path('rtt').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df.shape[0]
        temp_df = temp_df.assign(Node=name)
        df = pd.concat([df, temp_df])

    # Cwd Data
    colnames=['Time', 'Congestion Window'] 
    df2 = pd.DataFrame(columns =colnames)
    name = ['0'] * df2.shape[0]
    df2 = df2.assign(Node=name)
    df2.drop_duplicates('Time', inplace = True)

    pathlist = Path('cwd').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df2 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        if (temp_df2.shape[0] < 20):
            continue
        name = [str(path_in_str)] * temp_df2.shape[0]
        temp_df2 = temp_df2.assign(Node=name)
        temp_df2.drop_duplicates('Time', inplace = True)
        df2 = pd.concat([df2, temp_df2])

    # Queue Data
    colnames=['Time', 'Queue'] 
    df3= pd.DataFrame(columns =colnames)
    name = ['0'] * df3.shape[0]
    df3 = df3.assign(Node=name)
    df3.drop_duplicates('Time', inplace = True)

    pathlist = Path('queue_size_normalized').glob('**/*.txt')
    for files in natsort.natsorted(pathlist,reverse=False):
        '''pattern = r'^queue_size_normalized/queueUS_\d+-CS_\d+\.txt$'
        if not re.match(pattern, str(files)):
            continue'''

        path_in_str = str(files)
        temp_df3 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df3.shape[0]
        temp_df3 = temp_df3.assign(Node=name)
        temp_df3.drop_duplicates('Time', inplace = True)
        df3 = pd.concat([df3, temp_df3])

    # ECN Data
    colnames=['Time', 'ECN'] 
    df4 = pd.DataFrame(columns =colnames)
    name = ['0'] * df4.shape[0]
    df4 = df4.assign(Node=name)

    pathlist = Path('ecn').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df4 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df4.shape[0]
        temp_df4 = temp_df4.assign(Node=name)
        df4 = pd.concat([df4, temp_df4])

    # Sent data
    colnames=['Time', 'Sent'] 
    df5 = pd.DataFrame(columns =colnames)
    name = ['0'] * df5.shape[0]
    df5 = df5.assign(Node=name)

    pathlist = Path('sent').glob('*')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df5 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df5.shape[0]
        temp_df5 = temp_df5.assign(Node=name)
        df5 = pd.concat([df5, temp_df5])

    # Nack data
    colnames=['Time', 'Nack'] 
    df6 = pd.DataFrame(columns =colnames)
    name = ['0'] * df6.shape[0]
    df6 = df6.assign(Node=name)
    

    pathlist = Path('nack').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df6 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df6.shape[0]
        temp_df6 = temp_df6.assign(Node=name)
        df6 = pd.concat([df6, temp_df6])


    # Acked Bytes Data
    colnames=['Time', 'AckedBytes'] 
    df8 = pd.DataFrame(columns =colnames)
    name = ['0'] * df8.shape[0]
    df8 = df8.assign(Node=name)
    df8.drop_duplicates('Time', inplace = True)

    pathlist = Path('acked').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df8 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df8.shape[0]
        temp_df8 = temp_df8.assign(Node=name)
        temp_df8.drop_duplicates('Time', inplace = True)
        df8 = pd.concat([df8, temp_df8])

    # ECN in RTT Data
    colnames=['Time', 'ECNRTT'] 
    df13 = pd.DataFrame(columns =colnames)
    name = ['0'] * df13.shape[0]
    df13 = df13.assign(Node=name)
    df13.drop_duplicates('Time', inplace = True)

    pathlist = Path('ecn_rtt').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df13 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df13.shape[0]
        temp_df13 = temp_df13.assign(Node=name)
        temp_df13.drop_duplicates('Time', inplace = True)
        df13 = pd.concat([df13, temp_df13])

    # Trimming in RTT Data
    colnames=['Time', 'TrimmedRTT'] 
    df14 = pd.DataFrame(columns =colnames)
    name = ['0'] * df14.shape[0]
    df14 = df14.assign(Node=name)
    df14.drop_duplicates('Time', inplace = True)

    pathlist = Path('trimmed_rtt').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df14 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df14.shape[0]
        temp_df14 = temp_df14.assign(Node=name)
        temp_df14.drop_duplicates('Time', inplace = True)
        df14 = pd.concat([df14, temp_df14])


    # FastI data
    colnames=['Time', 'FastI'] 
    df9 = pd.DataFrame(columns =colnames)
    name = ['0'] * df9.shape[0]
    df9 = df9.assign(Node=name)
    

    pathlist = Path('fasti').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df9 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df9.shape[0]
        temp_df9 = temp_df9.assign(Node=name)
        df9 = pd.concat([df9, temp_df9])

    # FastD data
    colnames=['Time', 'FastD'] 
    df10 = pd.DataFrame(columns =colnames)
    name = ['0'] * df10.shape[0]
    df10 = df10.assign(Node=name)
    

    pathlist = Path('fastd').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df10 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df10.shape[0]
        temp_df10 = temp_df10.assign(Node=name)
        df10 = pd.concat([df10, temp_df10])

    # MediumI data
    colnames=['Time', 'MediumI'] 
    df11 = pd.DataFrame(columns =colnames)
    name = ['0'] * df11.shape[0]
    df11 = df11.assign(Node=name)
    

    pathlist = Path('mediumi').glob('**/*.txt')
    for files in sorted(pathlist):
        path_in_str = str(files)
        temp_df11 = pd.read_csv(path_in_str, names=colnames, header=None, index_col=False, sep=',')
        name = [str(path_in_str)] * temp_df11.shape[0]
        temp_df11 = temp_df11.assign(Node=name)
        df11 = pd.concat([df11, temp_df11])

    print("Finished Parsing")
    # Create figure with secondary y-axis
    fig = make_subplots(specs=[[{"secondary_y": True}]])
    color = ['#636EFA', '#0511a9', '#EF553B', '#00CC96', '#AB63FA', '#FFA15A', '#19D3F3', '#FF6692', '#B6E880', '#FF97FF', '#FECB52']
    # Add traces
    mean_rtt = df["RTT"].mean()
    max_rtt = df["RTT"].max()
    y_sent = max_rtt * 0.9
    y_ecn = max_rtt * 0.85
    y_nack =max_rtt * 0.80
    y_fasti =max_rtt * 0.75
    y_fastd =max_rtt * 0.70
    y_mediumi =max_rtt * 0.65
    mean_rtt = 10000
    count = 0
    for i in df['Node'].unique():
        sub_df = df.loc[df['Node'] == str(i)]
        fig.add_trace(
            go.Scatter(x=sub_df["Time"], y=sub_df['RTT'], mode='markers', marker=dict(size=2), name=str(i), line=dict(color=color[0]), opacity=0.9, showlegend=True),
            secondary_y=False,
        )
        '''
        fig.add_trace(
            go.Scatter(x=sub_df["Time"], y=sub_df['RTT'], mode='markers', marker=dict(size=10), name=str(i), line=dict(color=color[0]), opacity=0.9, showlegend=True, marker_symbol="triangle-up"),
            secondary_y=False,
        )
        '''
        if (args.show_triangles is not None):
            fig.add_trace(
                go.Scatter(x=sub_df["Time"], y=sub_df['RTT'], mode="markers", marker_symbol="triangle-up", name="Mark Packet", marker=dict(size=6, color=color[1]), showlegend=True),
                secondary_y=False
            )
        if (args.num_to_show == 1):
            break

    # Queue
    count = 0
    df3['Queue'] = pd.to_numeric(df3['Queue'])
    max_ele = df3[['Queue']].idxmax(1)
    for i in df3['Node'].unique():
        sub_df = df3.loc[df3['Node'] == str(i)]
        if (skip_small_value is True and sub_df['Queue'].max() < 500):
            count += 1
            continue

        fig.add_trace(
            go.Scatter(x=sub_df["Time"], y=sub_df['Queue'], name="Queue " + str(i),   mode="markers",  marker=dict(size=1.4), line=dict(dash='dash', color="black", width=3),  showlegend=True),
            secondary_y=False,
        )
        count += 1


    # NACK
    mean_sent = df6["Time"].mean()
    df6['Nack'] = df6['Nack'].multiply(y_nack)
    for i in df6['Node'].unique():
        sub_df6 = df6.loc[df6['Node'] == str(i)]
        fig.add_trace(
            go.Scatter(x=sub_df6["Time"], y=sub_df6["Nack"], mode="markers", marker_symbol="triangle-up", name="NACK Packet", marker=dict(size=5, color="grey"), showlegend=True),
            secondary_y=False
        )


    if args.name is not None:
        my_title=args.name
    else:
        my_title="<b>Permutation Across - 4:1 FT - 400Gbps - 2 MiB - UEC</b>"

    # Add figure title
    fig.update_layout(title_text=my_title)


    if (args.x_limit is not None):
        fig.update_layout(xaxis_range=[0,args.x_limit])

    if (args.annotations is not None):
        fig.add_annotation(
        xref="x domain",
        yref="y domain",
        # The arrow head will be 25% along the x axis, starting from the left
        x=0.04,
        # The arrow head will be 40% along the y axis, starting from the bottom
        y=0.99,
        showarrow=False,
        text="Congestion Window",
        font=dict(color=color[2], size=13),
        )

        fig.add_annotation(
        xref="x domain",
        yref="y domain",
        # The arrow head will be 25% along the x axis, starting from the left
        x=0.12,
        # The arrow head will be 40% along the y axis, starting from the bottom
        y=0.8,
        showarrow=False,
        text="RTT",
        font=dict(color=color[1], size=13),
        )

        fig.add_annotation(
        xref="x domain",
        yref="y domain",
        # The arrow head will be 25% along the x axis, starting from the left
        x=0.03,
        # The arrow head will be 40% along the y axis, starting from the bottom
        y=0.47,
        showarrow=False,
        text="Queuing<br>Latency",
        font=dict(color=color[3], size=13),
        )


    config = {
    'toImageButtonOptions': {
        'format': 'png', # one of png, svg, jpeg, webp
        'filename': 'custom_image',
        'height': 550,
        'width': 1000,
        'scale':4 # Multiply title/legend/axis/canvas sizes by this factor
    }
    }

    # Set x-axis title
    fig.update_xaxes(title_text="Time (ns)")
    # Set y-axes titles
    fig.update_yaxes(title_text="RTT || Queuing Latency (ns)", secondary_y=False)
    fig.update_yaxes(title_text="Congestion Window (B)", secondary_y=True)

    now = datetime.now() # current date and time
    date_time = now.strftime("%m:%d:%Y_%H:%M:%S")
    fig.write_image("out/fid_simple_{}.png".format(date_time),  width=2560, height=1440)
    plotly.offline.plot(fig, filename='out/fid_simple_{}.html'.format(date_time))
    if (args.output_folder is not None):
        plotly.offline.plot(fig, filename=args.output_folder + "/{}.html".format(args.name))
    if (args.no_show is None):
        fig.show()

if __name__ == "__main__":
    parser = ArgumentParser()
    parser.add_argument("--x_limit", type=int, help="Max X Value showed", default=None)
    parser.add_argument("--y_limit", type=int, help="Max Y value showed", default=None)
    parser.add_argument("--show_ecn", type=str, help="Show ECN points", default=None)
    parser.add_argument("--show_sent", type=str, help="Show Sent Points", default=None) 
    parser.add_argument("--show_triangles", type=str, help="Show RTT triangles", default=None) 
    parser.add_argument("--num_to_show", type=int, help="Number of lines to show", default=None) 
    parser.add_argument("--annotations", type=str, help="Number of lines to show", default=None) 
    parser.add_argument("--output_folder", type=str, help="OutFold", default=None) 
    parser.add_argument("--input_file", type=str, help="InFold", default=None) 
    parser.add_argument("--name", type=str, help="Name Algo", default=None) 
    parser.add_argument("--no_show", type=int, help="Don't show plot, just save", default=None) 
    args = parser.parse_args()
    main(args)

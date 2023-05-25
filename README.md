# htsim Network Simulator

htsim is a high performance discrete event simulator, inspired by ns2, but much faster, primarily intended to examine congestion control algorithm behaviour.  It was originally written by [Mark Handley](http://www0.cs.ucl.ac.uk/staff/M.Handley/) to allow [Damon Wishik](https://www.cl.cam.ac.uk/~djw1005/) to examine TCP stability issues when large numbers of flows are multiplexed.  It was extended by [Costin Raiciu](http://nets.cs.pub.ro/~costin/) to examine [Multipath TCP performance](http://nets.cs.pub.ro/~costin/files/mptcp-nsdi.pdf) during the MPTCP standardization process, and models of datacentre networks were added to [examine multipath transport](http://nets.cs.pub.ro/~costin/files/mptcp_dc_sigcomm.pdf) in a variety of datacentre topologies.  [NDP](http://nets.cs.pub.ro/~costin/files/ndp.pdf) was developed using htsim, and simple models of DCTCP, DCQCN were added for comparison.  Later htsim was adopted by Correct Networks (now part of Broadcom) to develop [EQDS](http://nets.cs.pub.ro/~costin/files/eqds.pdf), and switch models were improved to allow a variety of forwarding methods.  Support for a simple RoCE model, PFC, Swift and HPCC were added.

## Getting Started

There are some limited instructions in the [wiki](https://github.com/Broadcom/csg-htsim/wiki).  

htsim is written in C++, and has no dependencies.  It should compile and run with g++ or clang on MacOS or Linux.  To compile htsim, cd into the sim directory and run make.

To get started with running experiments, take a look in the experiments directory where there are some examples.  These examples generally require bash, python3 and gnuplot.

## Basic Instructions for htsim

Compile with the following instruction. Note that currently there are some issues with the make file (being fixed ASAP) so it is necessary to clean and recompile everything each time to avoid issues. To do so, we recommend running the following command line from the ```/sim``` directory.

```
make clean && cd datacenter/ && make clean && cd .. && make -j 8 && cd datacenter/ && make -j 8 && cd ..
```

Once that is done, we currently provide two entry points to run the simulator with the support of LogGOPSim: htsim_uec_entry_modern (and also the oversubscribed version) and htsim_ndp_entry_modern. The idea here is that these entry points will execute the GOAL input file (in binary format) that is given as part of the ```-goal``` command. The input file should be inside ```sim/lgs/input/```.
We also include a list of pre-compiled binary input GOAL files to use for certain benchmarks (incast, permutation...).

To actually run the application a typical command line would look like this (for the UEC protocol):
```
./htsim_uec_entry_modern_os -o uec_entry -nodes 64 -q 255500 -strat perm -kmin 20 -kmax 80 -linkspeed 400000 -mtu 2048  -hop_latency 400 -switch_latency 0 -k 4 -reuse_entropy 1 -goal permutation_across_64_large.bin > outUEC.tmp
```

And like this for NDP:
```
./htsim_ndp_entry_modern_os -o uec_entry -nodes 512 -cwnd 255500 -q 255500 -strat perm -linkspeed 400000 -mtu 2048  -hop_latency 400 -switch_latency 0 -k 4 -goal mp.bin > outUEC.tmp
```

Other protocols are currently being added. It is possible to add one by editing the logsim-interface files.


## Generating GOAL input files
We will now explain how to generate a goal inpute file and then how to convert it into a binary format for it to be used as input.

### Manually
It is possible to write a goal input file manually. For the specific of it, please check the original [GOAL](https://htor.inf.ethz.ch/publications/img/hoefler-goal.pdf) paper and [LGS](https://htor.inf.ethz.ch/publications/img/hoefler-loggopsim.pdf) paper.

### Schedgen
Schedgen offers a quick system to generate traces based on a specific traffic format. We currently support a numbers of MPI collectives and also general traffic patters such as incast, permutation, tornado... Check the ```schedgen/schedgen.cpp``` file for more infromation.
As as example, to generate a permutation with multiple send/recv per node, we would use (after running ```make```):

```
./schedgen -p multiple_permutation -s 64 -o mp.goal -d 1000000
```
This will generate a ```.goal``` input file for 64 nodes and a message of 1MiB.

### Schedgen ML Traces
Coming soon, it is implemented but missing some features. 

### Convert GOAL raw files to binary
Finally, to convert the GOAL input files to binary we need to use the built in tool. The usage is very simple and follows the following command line from ```goal_converter/``` (after running ```make``` from the same folder):
```
./txt2bin -i ./mp.goal -o ./mp.bin -p
```
This will generate a binary file that we can then use as input file for htsim when specifying the ```-goal``` parameter.

## Plotting

We currently provide two plotting files:

- ```performance.py``` will generate a plot with some overall results including RTT, CWND, NACK, ECN, Queue sizes over time.

- ```load_balance_anlysis.py``` will generate a plot with link utilization after having fixed a specific switch to analyze. 

Please check the files for more command line options that can be used with both. It is also possible that some Python libraries will need to be installed

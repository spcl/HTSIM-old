// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
#include "config.h"
#include "network.h"
#include "randomqueue.h"
#include <iostream>
#include <math.h>
#include <sstream>
#include <string.h>
#include <strstream>
//#include "subflow_control.h"
#include "clock.h"
#include "compositequeue.h"
#include "connection_matrix.h"
#include "eventlist.h"
#include "firstfit.h"
#include "logfile.h"
#include "loggers.h"
#include "logsim-interface.h"
#include "pipe.h"
#include "shortflows.h"
#include "topology.h"
#include "uec.h"
//#include "vl2_topology.h"

// Fat Tree topology was modified to work with this script, others won't work
// correctly
#include "fat_tree_topology.h"
//#include "oversubscribed_fat_tree_topology.h"
//#include "multihomed_fat_tree_topology.h"
//#include "star_topology.h"
//#include "bcube_topology.h"
#include <list>

// Simulation params

#define PRINT_PATHS 0

#define PERIODIC 0
#include "main.h"

// int RTT = 10; // this is per link delay; identical RTT microseconds = 0.02 ms
uint32_t RTT = 400; // this is per link delay in ns; identical RTT microseconds
                    // = 0.02 ms
int DEFAULT_NODES = 128;
#define DEFAULT_QUEUE_SIZE                                                     \
    100000000 // ~100MB, just a large value so we can ignore queues
// int N=128;

FirstFit *ff = NULL;
unsigned int subflow_count = 1;

string ntoa(double n);
string itoa(uint64_t n);

//#define SWITCH_BUFFER (SERVICE * RTT / 1000)
#define USE_FIRST_FIT 0
#define FIRST_FIT_INTERVAL 100

EventList eventlist;

Logfile *lg;

void exit_error(char *progr) {
    cout << "Usage " << progr
         << " [UNCOUPLED(DEFAULT)|COUPLED_INC|FULLY_COUPLED|COUPLED_EPSILON] "
            "[epsilon][COUPLED_SCALABLE_TCP"
         << endl;
    exit(1);
}

void print_path(std::ofstream &paths, const Route *rt) {
    for (unsigned int i = 1; i < rt->size() - 1; i += 2) {
        RandomQueue *q = (RandomQueue *)rt->at(i);
        if (q != NULL)
            paths << q->str() << " ";
        else
            paths << "NULL ";
    }

    paths << endl;
}

int main(int argc, char **argv) {
    Packet::set_packet_size(PKT_SIZE_MODERN);
    eventlist.setEndtime(timeFromSec(1));
    Clock c(timeFromSec(5 / 100.), eventlist);
    mem_b queuesize = INFINITE_BUFFER_SIZE;
    int no_of_conns = 0, cwnd = MAX_CWD_MODERN_UEC, no_of_nodes = DEFAULT_NODES;
    stringstream filename(ios_base::out);
    RouteStrategy route_strategy = NOT_SET;
    std::string goal_filename;

    int i = 1;
    filename << "logout.dat";

    while (i < argc) {
        if (!strcmp(argv[i], "-o")) {
            filename.str(std::string());
            filename << argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-sub")) {
            subflow_count = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-conns")) {
            no_of_conns = atoi(argv[i + 1]);
            cout << "no_of_conns " << no_of_conns << endl;
            cout << "!!currently hardcoded to 8, value will be ignored!!"
                 << endl;
            i++;
        } else if (!strcmp(argv[i], "-nodes")) {
            no_of_nodes = atoi(argv[i + 1]);
            cout << "no_of_nodes " << no_of_nodes << endl;
            i++;
        } else if (!strcmp(argv[i], "-cwnd")) {
            cwnd = atoi(argv[i + 1]);
            cout << "cwnd " << cwnd << endl;
            i++;
        } else if (!strcmp(argv[i], "-q")) {
            queuesize = atoi(argv[i + 1]);
            i++;
        } else if (!strcmp(argv[i], "-goal")) {
            goal_filename = argv[i + 1];
            i++;
        } else if (!strcmp(argv[i], "-strat")) {
            if (!strcmp(argv[i + 1], "perm")) {
                route_strategy = SCATTER_PERMUTE;
            } else if (!strcmp(argv[i + 1], "rand")) {
                route_strategy = SCATTER_RANDOM;
            } else if (!strcmp(argv[i + 1], "pull")) {
                route_strategy = PULL_BASED;
            } else if (!strcmp(argv[i + 1], "single")) {
                route_strategy = SINGLE_PATH;
            }
            i++;
        } else
            exit_error(argv[0]);

        i++;
    }
    srand(time(NULL));
    system("mkdir -p ../output && mkdir -p ../output/rtt && mkdir -p "
           "../output/cwd && mkdir -p ../output/queue && mkdir -p "
           "../output/ecn && mkdir -p ../output/sent && mkdir -p "
           "../output/acked  && mkdir -p "
           "../output/nack");
    system("rm -r ../output/rtt/*");
    system("rm -r ../output/cwd/*");
    system("rm -r ../output/queue/*");
    system("rm -r ../output/ecn/*");
    system("rm -r ../output/sent/*");
    system("rm -r ../output/acked/*");
    system("rm -r ../output/nack/*");

    if (route_strategy == NOT_SET) {
        fprintf(stderr, "Route Strategy not set.  Use the -strat param.  "
                        "\nValid values are perm, rand, pull, rg and single\n");
        exit(1);
    }

    cout << "Using subflow count " << subflow_count << endl;

    // prepare the loggers

    cout << "Logging to " << filename.str() << endl;
    // Logfile
    Logfile logfile(filename.str(), eventlist);

#if PRINT_PATHS
    filename << ".paths";
    cout << "Logging path choices to " << filename.str() << endl;
    std::ofstream paths(filename.str().c_str());
    if (!paths) {
        cout << "Can't open for writing paths file!" << endl;
        exit(1);
    }
#endif

    int tot_subs = 0;
    int cnt_con = 0;

    lg = &logfile;

    logfile.setStartTime(timeFromSec(0));

    // UecLoggerSimple uecLogger;
    // logfile.addLogger(uecLogger);
    TrafficLoggerSimple traffic_logger = TrafficLoggerSimple();
    logfile.addLogger(traffic_logger);

    // UecSrc *uecSrc;
    // UecSink *uecSink;

    // Route *routeout, *routein;
    // double extrastarttime;

    int dest;

#if USE_FIRST_FIT
    if (subflow_count == 1) {
        ff = new FirstFit(timeFromMs(FIRST_FIT_INTERVAL), eventlist);
    }
#endif

#ifdef FAT_TREE
    FatTreeTopology::set_tiers(3);
    FatTreeTopology *top = new FatTreeTopology(
            no_of_nodes, speedFromMbps(static_cast<double>(HOST_NIC)),
            queuesize, NULL, &eventlist, ff, COMPOSITE, timeFromNs(RTT),
            timeFromUs(0.0)); // TODO(tommaso): check parameters -- see
                              // main_ndp.cpp for how you can turn them into
                              // runtime options instead of compiled parameters
#endif

#ifdef OV_FAT_TREE
    OversubscribedFatTreeTopology *top =
            new OversubscribedFatTreeTopology(&logfile, &eventlist, ff);
#endif

#ifdef MH_FAT_TREE
    MultihomedFatTreeTopology *top =
            new MultihomedFatTreeTopology(&logfile, &eventlist, ff);
#endif

#ifdef STAR
    StarTopology *top = new StarTopology(&logfile, &eventlist, ff);
#endif

#ifdef BCUBE
    BCubeTopology *top = new BCubeTopology(&logfile, &eventlist, ff);
    cout << "BCUBE " << K << endl;
#endif

#ifdef VL2
    VL2Topology *top = new VL2Topology(&logfile, &eventlist, ff);
#endif

    vector<const Route *> ***net_paths;
    net_paths = new vector<const Route *> **[no_of_nodes];

    int *is_dest = new int[no_of_nodes];

    for (int i = 0; i < no_of_nodes; i++) {
        is_dest[i] = 0;
        net_paths[i] = new vector<const Route *> *[no_of_nodes];
        for (int j = 0; j < no_of_nodes; j++)
            net_paths[i][j] = NULL;
    }

#if USE_FIRST_FIT
    if (ff)
        ff->net_paths = net_paths;
#endif

    // vector<int> *destinations;

    // Permutation connections
    // ConnectionMatrix *conns = new ConnectionMatrix(no_of_conns);
    // conns->setLocalTraffic(top);

    // cout << "Running perm with " << no_of_conns << " connections" << endl;
    // conns->setPermutation(no_of_conns);
    cout << "Running incastt with " << no_of_conns << " connections" << endl;
    // conns->setIncast(no_of_conns, no_of_nodes - no_of_conns);
    //  conns->setStride(no_of_conns);
    //  conns->setStaggeredPermutation(top,(double)no_of_conns/100.0);
    //  conns->setStaggeredRandom(top,512,1);
    //  conns->setHotspot(no_of_conns,512/no_of_conns);
    //  conns->setManytoMany(128);

    // conns->setVL2();

    // conns->setRandom(no_of_conns);

    map<int, vector<int> *>::iterator it;

    // used just to print out stats data at the end
    list<const Route *> routes;

    int connID = 0;
    dest = 1;
    // int receiving_node = 127;
    vector<int> subflows_chosen;

    vector<UecSrc *> uecSrcVector;
    printf("Starting LGS Interface");
    LogSimInterface *lgs = new LogSimInterface(NULL, &traffic_logger, eventlist,
                                               top, net_paths);
    lgs->set_protocol(UEC_PROTOCOL);
    lgs->set_cwd(cwnd);
    lgs->set_queue_size(queuesize);
    start_lgs(goal_filename, *lgs);

    for (int src = 0; src < dest; ++src) {
        connID++;
        if (!net_paths[src][dest]) {
            vector<const Route *> *paths = top->get_paths(src, dest);
            net_paths[src][dest] = paths;
            for (unsigned int i = 0; i < paths->size(); i++) {
                routes.push_back((*paths)[i]);
            }
        }
        if (!net_paths[dest][src]) {
            vector<const Route *> *paths = top->get_paths(dest, src);
            net_paths[dest][src] = paths;
        }
    }

    cout << "Mean number of subflows " << ntoa((double)tot_subs / cnt_con)
         << endl;

    // Record the setup
    int pktsize = Packet::data_packet_size();
    logfile.write("# pktsize=" + ntoa(pktsize) + " bytes");
    logfile.write("# subflows=" + ntoa(subflow_count));
    logfile.write("# hostnicrate = " + ntoa(HOST_NIC) + " pkt/sec");
    logfile.write("# corelinkrate = " + ntoa(HOST_NIC * CORE_TO_HOST) +
                  " pkt/sec");
    // logfile.write("# buffer = " + ntoa((double)
    // (queues_na_ni[0][1]->_maxsize) / ((double) pktsize)) + " pkt");
    double rtt = timeAsSec(timeFromUs(RTT));
    logfile.write("# rtt =" + ntoa(rtt));

    cout << "Done" << endl;
    list<const Route *>::iterator rt_i;
    int counts[10];
    int hop;
    for (int i = 0; i < 10; i++)
        counts[i] = 0;
    for (rt_i = routes.begin(); rt_i != routes.end(); rt_i++) {
        const Route *r = (*rt_i);
        // print_route(*r);
        cout << "Path:" << endl;
        hop = 0;
        for (std::size_t i = 0; i < r->size(); i++) {
            PacketSink *ps = r->at(i);
            CompositeQueue *q = dynamic_cast<CompositeQueue *>(ps);
            if (q == 0) {
                cout << ps->nodename() << endl;
            } else {
                cout << q->nodename() << " id=" << 0 /*q->id*/ << " "
                     << q->num_packets() << "pkts " << q->num_headers()
                     << "hdrs " << q->num_acks() << "acks " << q->num_nacks()
                     << "nacks " << q->num_stripped() << "stripped"
                     << endl; // TODO(tommaso): compositequeues don't have id.
                              // Need to add that or find an alternative way.
                              // Verify also that compositequeue is the right
                              // queue to use here.
                counts[hop] += q->num_stripped();
                hop++;
            }
        }
        cout << endl;
    }
    for (int i = 0; i < 10; i++)
        cout << "Hop " << i << " Count " << counts[i] << endl;
}

string ntoa(double n) {
    stringstream s;
    s << n;
    return s.str();
}

string itoa(uint64_t n) {
    stringstream s;
    s << n;
    return s.str();
}

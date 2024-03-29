// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
#ifndef shortflows
#define shortflows

#include "connection_matrix.h"
#include "eventlist.h"
#include "logfile.h"
#include "main.h"
#include "randomqueue.h"
#include "tcp.h"
#include "tcp_transfer.h"
#include <list>
#include <map>
extern int N;

struct ShortFlow {
    TcpSrcTransfer *src;
    TcpSinkTransfer *snk;
};

class ShortFlows : public EventSource {
  public:
    ShortFlows(double l, EventList &eventlist, vector<const Route *> ***np, ConnectionMatrix *c, Logfile *logfile,
               TcpRtxTimerScanner *r);
    void doNextEvent();

    void run();

    ShortFlow *createConnection(int src, int dst, simtime_picosec starttime);
    vector<const Route *> ***net_paths;

  private:
    vector<ShortFlow *> connections[1024][1024];
    vector<connection *> *_traffic_matrix;
    Logfile *logfile;

    // create connections periodically
    // change path
    // start
    // log finish time when it's done

    double _lambda;
    TcpRtxTimerScanner *tcpRtxScanner;
};

#endif

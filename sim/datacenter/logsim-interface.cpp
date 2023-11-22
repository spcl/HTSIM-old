/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "logsim-interface.h"
#include "lgs/LogGOPSim.hpp"
#include "lgs/Network.hpp"
#include "lgs/Noise.hpp"
#include "lgs/Parser.hpp"
#include "lgs/TimelineVisualization.hpp"
#include "lgs/cmdline.h"
#include "main.h"
#include "ndp.h"
#include "swifttrimming.h"
#include "topology.h"
#include "uec.h"
#include "uec_drop.h"
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <queue>
#include <regex>
#include <string>
#include <utility>
/*#define BOOST_NO_CXX11_SCOPED_ENUMS
#include <boost/filesystem.hpp>
#undef BOOST_NO_CXX11_SCOPED_ENUMS*/

#define DEBUG_PRINT 0

static bool print = false;

LogSimInterface::LogSimInterface() {}

LogSimInterface::LogSimInterface(UecLogger *logger, TrafficLogger *pktLogger,
                                 EventList &eventList, FatTreeTopology *topo,
                                 std::vector<const Route *> ***routes) {
    _logger = logger;
    _flow = pktLogger;
    _eventlist = &eventList;
    _topo = topo;
    _netPaths = routes;
    _latest_recv = new graph_node_properties();
}

void LogSimInterface::set_cwd(int cwd) { _cwd = cwd; }

void LogSimInterface::htsim_schedule(u_int32_t host, int to, int size, int tag,
                                     u_int64_t start_time_event,
                                     int my_offset) {
    // Send event to htsim for actual send
    send_event(host, to, size, tag, start_time_event);

    // Save Event for internal tracking
    std::string to_hash = std::to_string(host) + "@" + std::to_string(to) +
                          "@" + std::to_string(tag);
    printf("Scheduling Event (%s) of size %d from %d to %d tag %d start_tiem "
           "%lu - Time is %lu\n ",
           to_hash.c_str(), size, host, to, tag, start_time_event, GLOBAL_TIME);
    MsgInfo entry;
    entry.start_time = start_time_event;
    entry.total_bytes_msg = size;
    entry.offset = my_offset;
    entry.bytes_left_to_recv = size;
    entry.to_parse = 42;
    active_sends[to_hash] = entry;
}

void LogSimInterface::send_event(int from, int to, int size, int tag,
                                 u_int64_t start_time_event) {

    // Create UEC Src and Dest
    if (_protocolName == UEC_PROTOCOL) {
        if (_uecRtxScanner == NULL) {
            _uecRtxScanner =
                    new UecRtxTimerScanner(BASE_RTT_MODERN * 1000, *_eventlist);
        }

        printf("Starting1 UEC LGS Setup\n");
        fflush(stdout);
        // This is updated inside UEC if it doesn't fit the default values
        uint64_t rtt = BASE_RTT_MODERN * 1000;
        uint64_t bdp = BDP_MODERN_UEC;
        uint64_t queueDrainTime =
                _queuesize * 8 * 1000 /
                (LINK_SPEED_MODERN); // convert queuesize to bits, then divide
                                     // by link speed converted from Mbps to
                                     // bpps

        // Create Routes from/to Src and Dest
        vector<const Route *> *paths = _topo->get_paths(from, to);
        _netPaths[from][to] = paths;
        vector<const Route *> *paths2 = _topo->get_paths(to, from);
        _netPaths[to][from] = paths2;

        // Choose Path from possible routes
        int choice = rand() % _netPaths[from][to]->size();

        Route *routein = new Route(*_topo->get_paths(to, from)->at(choice));
        UecSrc *uecSrc = new UecSrc(_logger, _flow, *_eventlist, rtt, bdp,
                                    queueDrainTime, routein->hop_count());

        uecSrc->setFlowSize(size);
        uecSrc->setReuse(_use_good_entropies);
        uecSrc->setIgnoreEcnAck(_ignore_ecn_ack);
        uecSrc->setIgnoreEcnData(_ignore_ecn_data);
        uecSrc->setNumberEntropies(_num_entropies);
        printf("CWD Start %d - From %d To %d\n", uecSrc->_cwnd, from, to);
        uecSrc->setName("uec_" + std::to_string(from) + "_" +
                        std::to_string(to));
        uecSrc->set_flow_over_hook(std::bind(&LogSimInterface::flow_over, this,
                                             std::placeholders::_1));
        uecSrc->from = from;
        uecSrc->to = to;
        uecSrc->tag = tag;
        printf("Setting %d %d %d\n", from, to, tag);
        std::string to_hash = std::to_string(from) + "@" + std::to_string(to) +
                              "@" + std::to_string(tag);
        connection_log[to_hash] = uecSrc;
        _uecSrcVector.push_back(uecSrc);
        UecSink *uecSink = new UecSink();
        uecSink->setName("uec_sink_Rand");
        uecSink->from = from;
        uecSink->to = to;
        uecSink->tag = tag;

        uecSrc->set_dst(to);
        uecSink->set_src(from);

        Route *srctotor = new Route();
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->pipes_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]
                        ->getRemoteEndpoint());

        Route *dsttotor = new Route();
        dsttotor->push_back(
                _topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(
                _topo->pipes_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(_topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]
                                    ->getRemoteEndpoint());

        if (tag == 99) {
            uecSrc->connect(srctotor, dsttotor, *uecSink, GLOBAL_TIME + 100000);
        } else {
            uecSrc->connect(srctotor, dsttotor, *uecSink, GLOBAL_TIME);
        }

        uecSrc->set_paths(path_entropy_size);
        uecSink->set_paths(path_entropy_size);

        // register src and snk to receive packets from their respective TORs.
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(from)]);
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(to)]);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(from)]->addHostPort(
                from, uecSrc->flow_id(), uecSrc);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(to)]->addHostPort(
                to, uecSrc->flow_id(), uecSink);

        // Actually connect src and dest
        _uecRtxScanner->registerUec(*uecSrc);
        printf("Finished UEC LGS Setup\n");
        fflush(stdout);
    } else if (_protocolName == UEC_DROP_PROTOCOL) {
        if (_uecDropRtxScanner == NULL) {
            _uecDropRtxScanner = new UecDropRtxTimerScanner(50000, *_eventlist);
        }

        printf("Starting1 UEC DROP LGS Setup\n");
        fflush(stdout);
        // This is updated inside UEC if it doesn't fit the default values
        uint64_t rtt = BASE_RTT_MODERN * 1000;
        uint64_t bdp = BDP_MODERN_UEC;
        uint64_t queueDrainTime =
                _queuesize * 8 * 1000 /
                (LINK_SPEED_MODERN); // convert queuesize to bits, then divide
                                     // by link speed converted from Mbps to
                                     // bpps

        // Create Routes from/to Src and Dest
        vector<const Route *> *paths = _topo->get_paths(from, to);
        _netPaths[from][to] = paths;
        vector<const Route *> *paths2 = _topo->get_paths(to, from);
        _netPaths[to][from] = paths2;

        // Choose Path from possible routes
        int choice = rand() % _netPaths[from][to]->size();

        Route *routein = new Route(*_topo->get_paths(to, from)->at(choice));
        UecDropSrc *uecSrc =
                new UecDropSrc(NULL, _flow, *_eventlist, rtt, bdp,
                               queueDrainTime, routein->hop_count());

        uecSrc->setFlowSize(size);
        uecSrc->setReuse(_use_good_entropies);
        uecSrc->setIgnoreEcnAck(_ignore_ecn_ack);
        uecSrc->setIgnoreEcnData(_ignore_ecn_data);
        uecSrc->setNumberEntropies(_num_entropies);
        printf("CWD Start %d - From %d To %d\n", uecSrc->_cwnd, from, to);
        uecSrc->setName("uec_" + std::to_string(from) + "_" +
                        std::to_string(to));
        uecSrc->set_flow_over_hook(std::bind(&LogSimInterface::flow_over, this,
                                             std::placeholders::_1));
        uecSrc->from = from;
        uecSrc->to = to;
        uecSrc->tag = tag;
        printf("Setting %d %d %d\n", from, to, tag);
        std::string to_hash = std::to_string(from) + "@" + std::to_string(to) +
                              "@" + std::to_string(tag);
        _uecDropSrcVector.push_back(uecSrc);
        UecDropSink *uecSink = new UecDropSink();
        uecSink->setName("uec_sink_Rand");
        uecSink->from = from;
        uecSink->to = to;
        uecSink->tag = tag;

        uecSrc->set_dst(to);
        uecSink->set_src(from);

        Route *srctotor = new Route();
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->pipes_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]
                        ->getRemoteEndpoint());

        Route *dsttotor = new Route();
        dsttotor->push_back(
                _topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(
                _topo->pipes_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(_topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]
                                    ->getRemoteEndpoint());

        if (tag == 989) {
            uecSrc->connect(srctotor, dsttotor, *uecSink, GLOBAL_TIME + 100000);
        } else {
            uecSrc->connect(srctotor, dsttotor, *uecSink, GLOBAL_TIME);
        }

        uecSrc->set_paths(path_entropy_size);
        uecSink->set_paths(path_entropy_size);

        // register src and snk to receive packets from their respective TORs.
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(from)]);
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(to)]);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(from)]->addHostPort(
                from, uecSrc->flow_id(), uecSrc);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(to)]->addHostPort(
                to, uecSrc->flow_id(), uecSink);

        // Actually connect src and dest
        _uecDropRtxScanner->registerUecDrop(*uecSrc);
        printf("Finished UEC LGS Setup\n");
        fflush(stdout);
    } else if (_protocolName == SWIFT_PROTOCOL) {
        if (_swiftTrimmingRtxScanner == NULL) {
            _swiftTrimmingRtxScanner = new SwiftTrimmingRtxTimerScanner(
                    BASE_RTT_MODERN * 1000, *_eventlist);
        }

        printf("Starting1 Swift LGS Setup\n");
        fflush(stdout);
        // This is updated inside UEC if it doesn't fit the default values
        uint64_t rtt = BASE_RTT_MODERN * 1000;
        uint64_t bdp = BDP_MODERN_UEC;
        uint64_t queueDrainTime =
                _queuesize * 8 * 1000 /
                (LINK_SPEED_MODERN); // convert queuesize to bits, then divide
                                     // by link speed converted from Mbps to
                                     // bpps

        // Create Routes from/to Src and Dest
        vector<const Route *> *paths = _topo->get_paths(from, to);
        _netPaths[from][to] = paths;
        vector<const Route *> *paths2 = _topo->get_paths(to, from);
        _netPaths[to][from] = paths2;

        // Choose Path from possible routes
        int choice = rand() % _netPaths[from][to]->size();

        Route *routein = new Route(*_topo->get_paths(to, from)->at(choice));
        SwiftTrimmingSrc *swiftTrimmingSrc =
                new SwiftTrimmingSrc(NULL, _flow, *_eventlist, rtt, bdp,
                                     queueDrainTime, routein->hop_count());

        swiftTrimmingSrc->setFlowSize(size);
        swiftTrimmingSrc->setReuse(_use_good_entropies);
        swiftTrimmingSrc->setIgnoreEcnAck(_ignore_ecn_ack);
        swiftTrimmingSrc->setIgnoreEcnData(_ignore_ecn_data);
        swiftTrimmingSrc->setNumberEntropies(_num_entropies);
        printf("CWD Start %d - From %d To %d\n", swiftTrimmingSrc->_cwnd, from,
               to);
        swiftTrimmingSrc->setName("swiftTrimming_" + std::to_string(from) +
                                  "_" + std::to_string(to));
        swiftTrimmingSrc->set_flow_over_hook(std::bind(
                &LogSimInterface::flow_over, this, std::placeholders::_1));
        swiftTrimmingSrc->from = from;
        swiftTrimmingSrc->to = to;
        swiftTrimmingSrc->tag = tag;
        printf("Setting %d %d %d\n", from, to, tag);
        std::string to_hash = std::to_string(from) + "@" + std::to_string(to) +
                              "@" + std::to_string(tag);
        _swiftTrimmingSrcVector.push_back(swiftTrimmingSrc);
        SwiftTrimmingSink *swiftTrimmingSink = new SwiftTrimmingSink();
        swiftTrimmingSink->setName("swiftTrimming_sink_Rand");
        swiftTrimmingSink->from = from;
        swiftTrimmingSink->to = to;
        swiftTrimmingSink->tag = tag;

        swiftTrimmingSrc->set_dst(to);
        swiftTrimmingSink->set_src(from);

        Route *srctotor = new Route();
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->pipes_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]
                        ->getRemoteEndpoint());

        Route *dsttotor = new Route();
        dsttotor->push_back(
                _topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(
                _topo->pipes_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(_topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]
                                    ->getRemoteEndpoint());

        swiftTrimmingSrc->connect(srctotor, dsttotor, *swiftTrimmingSink,
                                  GLOBAL_TIME);
        swiftTrimmingSrc->set_paths(path_entropy_size);
        swiftTrimmingSink->set_paths(path_entropy_size);

        // register src and snk to receive packets from their respective TORs.
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(from)]);
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(to)]);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(from)]->addHostPort(
                from, swiftTrimmingSrc->flow_id(), swiftTrimmingSrc);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(to)]->addHostPort(
                to, swiftTrimmingSrc->flow_id(), swiftTrimmingSink);

        // Actually connect src and dest
        connection_log_swift[to_hash] = swiftTrimmingSrc;
        _swiftTrimmingRtxScanner->registerSwiftTrimming(*swiftTrimmingSrc);
        printf("Finished UEC LGS Setup\n");
        fflush(stdout);
    } else if (_protocolName == NDP_PROTOCOL) {
        if (_ndpRtxScanner == NULL) {
            _ndpRtxScanner = new NdpRtxTimerScanner(timeFromUs((uint32_t)1000),
                                                    *_eventlist);
        }

        // NdpSrc::setRouteStrategy(SCATTER_RANDOM);
        // NdpSink::setRouteStrategy(SCATTER_RANDOM);

        NdpSrc *ndpSrc = new NdpSrc(NULL, NULL, *_eventlist);
        _ndpSrcVector.push_back(ndpSrc);
        ndpSrc->setCwnd(_cwd);
        ndpSrc->set_dst(to);
        ndpSrc->set_flowsize(size);
        ndpSrc->set_flow_over_hook(std::bind(&LogSimInterface::flow_over, this,
                                             std::placeholders::_1));
        if (_puller_map.count(to) == 0) {
            _puller_map[to] = new NdpPullPacer(
                    *_eventlist, speedFromMbps(LINK_SPEED_MODERN * 1000), 0.99);
        }
        NdpSink *ndpSnk = new NdpSink(_puller_map[to]);
        ndpSrc->from = from;
        ndpSrc->to = to;
        ndpSrc->tag = tag;
        printf("sink tag is %d\n", ndpSnk->tag);
        ndpSnk->set_src(from);

        ndpSrc->setName("NDP_" + std::to_string(from) + "_" +
                        std::to_string(to));
        ndpSnk->setName("NDP_Sink");
        _ndpRtxScanner->registerNdp(*ndpSrc);

        Route *srctotor = new Route();
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->pipes_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]);
        srctotor->push_back(
                _topo->queues_ns_nlp[from][_topo->HOST_POD_SWITCH(from)]
                        ->getRemoteEndpoint());

        Route *dsttotor = new Route();
        dsttotor->push_back(
                _topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(
                _topo->pipes_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]);
        dsttotor->push_back(_topo->queues_ns_nlp[to][_topo->HOST_POD_SWITCH(to)]
                                    ->getRemoteEndpoint());

        ndpSrc->connect(srctotor, dsttotor, *ndpSnk, GLOBAL_TIME);
        ndpSrc->set_paths(path_entropy_size);
        ndpSnk->set_paths(path_entropy_size);

        // register src and snk to receive packets from their respective TORs.
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(from)]);
        assert(_topo->switches_lp[_topo->HOST_POD_SWITCH(to)]);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(from)]->addHostPort(
                from, ndpSrc->flow_id(), ndpSrc);
        _topo->switches_lp[_topo->HOST_POD_SWITCH(to)]->addHostPort(
                to, ndpSrc->flow_id(), ndpSnk);
    }
}

void LogSimInterface::update_active_map(std::string to_hash, int size) {

    // Check that the flow actually exists
    active_sends[to_hash].bytes_left_to_recv =
            active_sends[to_hash].bytes_left_to_recv -
            Packet::data_packet_size();
    printf("Updated is %d\n", active_sends[to_hash].bytes_left_to_recv);
    if (active_sends[to_hash].bytes_left_to_recv <= 0) {

        // active_sends.erase(to_hash);
    }
}

bool LogSimInterface::all_sends_delivered() { return active_sends.size() == 0; }

void LogSimInterface::flow_over(const Packet &p) {

    // Get Unique Hash
    std::string to_hash = std::to_string(p.from) + "@" + std::to_string(p.to) +
                          "@" + std::to_string(p.tag);
    // active_sends[to_hash].bytes_left_to_recv = 0;
    printf("Flow Finished %d@%d@%d at %lu\n", p.from, p.to, p.tag, GLOBAL_TIME);
    fflush(stdout);

    // Here we have received a message fully, we need to give control back to
    // LGS
    _latest_recv = new graph_node_properties();
    _latest_recv->updated = true;
    _latest_recv->tag = p.tag;
    _latest_recv->type = OP_MSG;
    _latest_recv->target = p.from;
    _latest_recv->host = p.to;
    _latest_recv->starttime = GLOBAL_TIME * 1;
    _latest_recv->time = GLOBAL_TIME * 1;
    _latest_recv->size = active_sends[to_hash].total_bytes_msg;
    _latest_recv->offset = active_sends[to_hash].offset;
    _latest_recv->proc = 0;
    _latest_recv->nic = 0;

    active_sends.erase(to_hash);
    if (_protocolName == SWIFT_PROTOCOL) {
        connection_log_swift.erase(to_hash);
    } else if (_protocolName == NDP_PROTOCOL) {
        // connection_log.erase(to_hash);
    } else if (_protocolName == UEC_PROTOCOL) {
        connection_log.erase(to_hash);
    }
    return;
}

void LogSimInterface::reset_latest_receive() { _latest_recv->updated = false; }

void LogSimInterface::terminate_sim() {
    if (_protocolName == SWIFT_PROTOCOL) {
        for (std::size_t i = 0; i < _swiftTrimmingSrcVector.size(); ++i) {
            delete _swiftTrimmingSrcVector[i];
        }
    } else if (_protocolName == NDP_PROTOCOL) {
        for (std::size_t i = 0; i < _ndpSrcVector.size(); ++i) {
            delete _ndpSrcVector[i];
        }
    } else if (_protocolName == UEC_PROTOCOL) {
        for (std::size_t i = 0; i < _uecSrcVector.size(); ++i) {
            delete _uecSrcVector[i];
        }
    } else if (_protocolName == UEC_DROP_PROTOCOL) {
        for (std::size_t i = 0; i < _uecDropSrcVector.size(); ++i) {
            delete _uecDropSrcVector[i];
        }
    }
}

graph_node_properties LogSimInterface::htsim_simulate_until(u_int64_t until) {

    printf("Simulating Until Called\n");
    fflush(stdout);
    // GO!
    while (_eventlist->doNextEvent()) {
        if (_latest_recv->updated) {
            printf("Running - %d - %lu\n", _latest_recv->updated, GLOBAL_TIME);
            break;
        }
    }

    if (_latest_recv->updated) {
        printf("Updated - Time %lu\n", _latest_recv->time);
    } else
        printf("Nothing\n");
    return *_latest_recv;
}

int start_lgs(std::string filename_goal, LogSimInterface &lgs) {
    LogSimInterface *lgs_interface = &lgs;

    filename_goal = PROJECT_ROOT_PATH / ("sim/lgs/input/" + filename_goal);

    // Time Inside LGS
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;
    // auto start = high_resolution_clock::now();
    std::chrono::milliseconds global_time = std::chrono::milliseconds::zero();

    //  End Temp Only

#ifdef STRICT_ORDER
    btime_t aqtime = 0;
#endif

#ifndef LIST_MATCH
#endif

    // read input parameters
    // For now we use hardcoded constants
    const int o = 1500;
    const int O = 0;
    const int g = 1000;
    const int L = 2500;
    const int G = 6;
    print = false;
    const uint32_t S = 65535;
    std::uint64_t htsim_time = 0;

    Parser parser(filename_goal, 0);
    Network net;

    const uint p = parser.schedules.size();
    const int ncpus = parser.GetNumCPU();
    const int nnics = parser.GetNumNIC();

    printf("size: %i (%i CPUs, %i NICs); L=%i, o=%i g=%i, G=%i, O=%i, P=%i, "
           "S=%u\n",
           p, ncpus, nnics, L, o, g, G, O, p, S);

    Noise osnoise(p);

    // DATA structures for storing MPI matching statistics
    std::vector<int> rq_max(0);
    std::vector<int> uq_max(0);

    std::vector<std::vector<std::pair<int, btime_t>>> rq_matches(0);
    std::vector<std::vector<std::pair<int, btime_t>>> uq_matches(0);

    std::vector<std::vector<std::pair<int, btime_t>>> rq_misses(0);
    std::vector<std::vector<std::pair<int, btime_t>>> uq_misses(0);

    std::vector<std::vector<btime_t>> rq_times(0);
    std::vector<std::vector<btime_t>> uq_times(0);

    if (0) {
        // Initialize MPI matching data structures
        rq_max.resize(p);
        uq_max.resize(p);

        for (int i : rq_max) {
            rq_max[i] = 0;
            uq_max[i] = 0;
        }

        rq_matches.resize(p);
        uq_matches.resize(p);

        rq_misses.resize(p);
        uq_misses.resize(p);

        rq_times.resize(p);
        uq_times.resize(p);
    }

    // the active queue
    std::priority_queue<graph_node_properties,
                        std::vector<graph_node_properties>, aqcompare_func>
            aq;
    // the queues for each host
    std::vector<ruq_t> rq(p), uq(p); // receive queue, unexpected queue
    // next available time for o, g(receive) and g(send)
    std::vector<std::vector<btime_t>> nexto(p), nextgr(p), nextgs(p);
#ifdef HOSTSYNC
    std::vector<btime_t> hostsync(p);
#endif

    // initialize o and g for all PEs and hosts
    for (uint i = 0; i < p; ++i) {
        nexto[i].resize(ncpus);
        std::fill(nexto[i].begin(), nexto[i].end(), 0);
        nextgr[i].resize(nnics);
        std::fill(nextgr[i].begin(), nextgr[i].end(), 0);
        nextgs[i].resize(nnics);
        std::fill(nextgs[i].begin(), nextgs[i].end(), 0);
    }

    struct timeval tstart, tend;
    gettimeofday(&tstart, NULL);

    int host = 0;
    uint64_t num_events = 0;

    printf("Starting %lu\n", parser.schedules.size());

    for (Parser::schedules_t::iterator sched = parser.schedules.begin();
         sched != parser.schedules.end(); ++sched, ++host) {
        // initialize free operations (only once per run!)
        // sched->init_free_operations();

        // retrieve all free operations
        // typedef std::vector<std::string> freeops_t;
        // freeops_t free_ops = sched->get_free_operations();
        SerializedGraph::nodelist_t free_ops;
        sched->GetExecutableNodes(&free_ops);
        // ensure that the free ops are ordered by type
        std::sort(free_ops.begin(), free_ops.end(), gnp_op_comp_func());

        num_events += sched->GetNumNodes();

        // walk all new free operations and throw them in the queue
        for (SerializedGraph::nodelist_t::iterator freeop = free_ops.begin();
             freeop != free_ops.end(); ++freeop) {
            // if(print) std::cout << *freeop << " " ;

            freeop->host = host;
            freeop->time = 0;
#ifdef STRICT_ORDER
            freeop->ts = aqtime++;
#endif

            switch (freeop->type) {
            case OP_LOCOP:
                if (0)
                    printf("init %i (%i,%i) loclop: %lu\n", host, freeop->proc,
                           freeop->nic, (long unsigned int)freeop->size);
                break;
            case OP_SEND:
                if (1)
                    printf("init %i (%i,%i) send to: %i, tag: %i, size: %lu\n",
                           host, freeop->proc, freeop->nic, freeop->target,
                           freeop->tag, (long unsigned int)freeop->size);
                break;
            case OP_RECV:
                if (1)
                    printf("init %i (%i,%i) recvs from: %i, tag: %i, size: "
                           "%lu\n",
                           host, freeop->proc, freeop->nic, freeop->target,
                           freeop->tag, (long unsigned int)freeop->size);
                break;
            default:
                printf("not implemented!\n");
            }
            freeop->time = htsim_time;
            aq.push(*freeop);
            // std::cout << "AQ size after push: " << aq.size() << "\n";
        }
    }

    // printf("Initial AQ Size is %d\n\n", aq.size());

    // bool new_events = true;
    bool first_cycle = true;
    // uint lastperc = 0;
    int cycles = 0;

    while (!aq.empty() || (size_queue(rq, p) > 0) || (size_queue(uq, p) > 0)/* ||
           !lgs_interface->all_sends_delivered()*/) {
        if (cycles > 100000) {
            printf("\nERROR: We are in some sort of loop in the main WHILE. "
                   "Breaking after 100k cycles\n\n");
            break;
        }
        printf("----------------------------    ENTERING WHILE        // "
               "---------------------------- | %d - %ld %ld -  %d %d - %d %d "
               "Size AQ %lu - "
               "%d %d %d\n",
               aq.empty(), aq.top().time, htsim_time, 1, first_cycle,
               size_queue(rq, p), size_queue(uq, p), aq.size(), aq.top().host,
               aq.top().target, aq.top().tag);

        graph_node_properties temp_elem = aq.top();
        while (!aq.empty() && aq.top().time <= htsim_time) {
            if (cycles > 100000) {
                printf("\nERROR: We are in some sort of loop in the main "
                       "WHILE. Breaking after 100k cycles\n\n");
                exit(0);
            }
            cycles++;

            graph_node_properties elem = aq.top();
            aq.pop();

            // the lists of hosts that actually finished someting -- a host is
            // added whenever an irequires or requires is satisfied
            std::vector<int> check_hosts;

            // the BIG switch on element type that we just found
            // printf("Elem Type is %d\n", elem.type);
            switch (elem.type) {
            case OP_LOCOP: {
                if (print)
                    printf("[%i] found loclop of length %lu - t: %lu (CPU: "
                           "%i)\n",
                           elem.host, (ulint)elem.size, (ulint)elem.time,
                           elem.proc);
                if (nexto[elem.host][elem.proc] <= elem.time) {
                    // check if OS Noise occurred
                    osnoise.get_noise(elem.host, elem.time,
                                      elem.time + elem.size);
                    nexto[elem.host][elem.proc] = elem.size + htsim_time;
                    // printf("====================== Updated time is %ld
                    // ===============================1\n",
                    // nexto[elem.host][elem.proc]);
                    //  satisfy irequires
                    // parser.schedules[elem.host].issue_node(elem.node);
                    //  satisfy requires+irequires
                    // parser.schedules[elem.host].remove_node(elem.node);
                    parser.schedules[elem.host].MarkNodeAsStarted(elem.offset);
                    // parser.schedules[elem.host].MarkNodeAsDone(elem.offset);
                    elem.type = OP_LOCOP_IN_PROGRESS;
                    // check_hosts.push_back(elem.host);
                    // add to timeline
                    // tlviz.add_loclop(elem.host, elem.time,
                    // elem.time+elem.size, elem.proc);
                } else {
                    if (print)
                        printf("-- locop local o not available -- "
                               "reinserting\n");
                    elem.time = nexto[elem.host][elem.proc];
                }
                aq.push(elem);
            } break;

            case OP_LOCOP_IN_PROGRESS: {
                parser.schedules[elem.host].MarkNodeAsDone(elem.offset);
            } break;

            case OP_SEND: { // a send op

                // htsim_time = GLOBAL_TIME;
                if (elem.target == 999999999 || elem.target == 9999999) {
                    std::cout << "Time at Step " << elem.host << ": "
                              << htsim_time << " - Tag " << elem.tag << "\n";
                    parser.schedules[elem.host].MarkNodeAsStarted(elem.offset);
                    parser.schedules[elem.host].MarkNodeAsDone(elem.offset);
                    break;
                }

                if (0)
                    printf("[%i] found send to %i - t: %lu (CPU: %i)\n",
                           elem.host, elem.target, (ulint)elem.time, elem.proc);
                if (std::max(nexto[elem.host][elem.proc],
                             nextgs[elem.host][elem.nic]) <=
                    elem.time) { // local o,g available!
                    if (print)
                        printf("-- satisfy local irequires\n");
                    parser.schedules[elem.host].MarkNodeAsStarted(elem.offset);

                    lgs_interface->htsim_schedule(elem.host, elem.target,
                                                  elem.size, elem.tag,
                                                  elem.starttime, elem.offset);
                    printf("Send host %d - offset %d\n", elem.host,
                           elem.offset);
                    // parser.schedules[elem.host].MarkNodeAsDone(elem.offset);
                }
            } break;
            case OP_RECV: {
                printf("OP_RECV\n");
                if (1)
                    printf("[%i] found recv from %i - t: %lu (CPU: %i)\n",
                           elem.host, elem.target, (ulint)elem.time, elem.proc);

                parser.schedules[elem.host].MarkNodeAsStarted(elem.offset);
                check_hosts.push_back(elem.host);
                if (print)
                    printf("-- satisfy local irequires\n");

                ruqelem_t matched_elem;
                // NUMBER of elements that were searched during message matching
                int32_t match_attempts;

                match_attempts = match(elem, &uq[elem.host], &matched_elem);
                if (match_attempts >= 0) { // found it in local UQ
                    if (1)
                        printf("-- found in local UQ\n");
                    // satisfy local requires
                    parser.schedules[elem.host].MarkNodeAsDone(elem.offset);

                    printf("Recv host %d - offset %d\n", matched_elem.src,
                           matched_elem.offset);
                    // parser.schedules[matched_elem.src].MarkNodeAsDone(
                    //         matched_elem.offset);
                    // parser.schedules[elem.host].MarkNodeAsDone(elem.offset);

                    if (print)
                        printf("-- satisfy local requires\n");
                } else { // not found in local UQ - add to RQ

                    if (1)
                        printf("-- not found in local UQ -- add to RQ\n");
                    ruqelem_t nelem;
                    nelem.size = elem.size;
                    nelem.src = elem.target;
                    nelem.tag = elem.tag;
                    nelem.offset = elem.offset;
#ifdef LIST_MATCH
                    // printf("############ Host added is %d\n\n", elem.host);
                    rq[elem.host].push_back(nelem);
#else
                    rq[elem.host][std::make_pair(nelem.tag, nelem.src)].push(
                            nelem);
#endif
                }
            } break;

            case OP_MSG: {
                printf("OP MSG");
                if (print)
                    printf("[%i] found msg from %i, t: %lu (CPU: %i) - %d %d "
                           "%d\n",
                           elem.host, elem.target, (ulint)elem.time, elem.proc,
                           elem.nic, elem.proc, elem.offset);
                uint64_t earliestfinish = 0;
                // NUMBER of elements that were searched during message matching
                int32_t match_attempts;
                printf("nexto[elem.host][elem.proc] %ld (%d %d) - "
                       "nextgr[elem.host][elem.nic] %ld(% d % d) - time % ld\n",
                       nexto[elem.host][elem.proc], elem.host, elem.proc,
                       nextgr[elem.host][elem.nic], elem.host, elem.nic,
                       elem.time);
                if (std::max(nexto[elem.host][elem.proc],
                             nextgr[elem.host][elem.nic]) <=
                    elem.time /* local o,g available! */) {
                    // if (print) {
                    // printf("Reaching here %d %d - %d %d\n", nexto.size(),
                    // nextgr.size(), nexto[0].size(), nextgr[0].size());
                    // fflush(stdout); if(0) printf("-- msg o,g available
                    // (nexto: %lu, nextgr: %lu)\n", (long unsigned int)
                    // nexto[elem.host][elem.proc], (long unsigned int)
                    // nextgr[elem.host][elem.nic]);
                    //  check if OS Noise occurred
                    // btime_t noise = osnoise.get_noise(elem.host, elem.time,
                    // elem.time+o);
                    nexto[elem.host][elem.proc] =
                            elem.time +
                            0; /* message is only received after G is charged !!
                                  TODO: consuming o seems a bit odd in the LogGP
                                  model but well in practice */
                    ;
                    nextgr[elem.host][elem.nic] = elem.time + 0;
                    // printf("====================== Updated time is %ld
                    // ===============================2\n",
                    // nexto[elem.host][elem.proc]);

                    // nexto[elem.host][elem.proc] =
                    // elem.time+o+0+std::max((elem.size-1)*O,(elem.size-1)*G)
                    // /* message is only received after G is charged !! TODO:
                    // consuming o seems a bit odd in the LogGP model but well
                    // in practice */; nextgr[elem.host][elem.nic] =
                    // elem.time+g+(elem.size-1)*G; printf("Reaching here\n");
                    // fflush(stdout);
                    ruqelem_t matched_elem;
                    match_attempts = match(elem, &rq[elem.host], &matched_elem);
                    printf("Searching for MATCH");
                    if (match_attempts >= 0) { // found it in RQ
                        if (0) {
                            // RECORD match queue statistics
                            std::pair<int, btime_t> match =
                                    std::make_pair(match_attempts, elem.time);
                            rq_matches[elem.host].push_back(match);
                            /* Amount of time spent in queue */
                            rq_times[elem.host].push_back(
                                    elem.time - matched_elem.starttime);
                        }

                        if (1)
                            printf("-- Found in RQ\n");
                        parser.schedules[elem.host].MarkNodeAsDone(
                                matched_elem.offset);
                        parser.schedules[elem.target].MarkNodeAsDone(
                                elem.offset);
                        printf("Unlocking also Elem.Target %d and Elem.offset "
                               "%d\n",
                               elem.target, elem.offset);
                        // check_hosts.push_back(elem.host);
                        // printf("Reached after DONE \n"); fflush(stdout);

                    } else { // not in RQ

                        if (1)
                            printf("-- not found in RQ - add to UQ\n");
                        ruqelem_t nelem;
                        nelem.size = elem.size;
                        nelem.src = elem.target;
                        nelem.tag = elem.tag;
                        nelem.offset = elem.offset;
                        nelem.starttime = elem.time; // when it was started
#ifdef LIST_MATCH
                        uq[elem.host].push_back(nelem);
#else
                        uq[elem.host][std::make_pair(nelem.tag, nelem.src)]
                                .push(nelem);
#endif
                    }
                } else {
                    elem.time = std::max(std::max(nexto[elem.host][elem.proc],
                                                  nextgr[elem.host][elem.nic]),
                                         earliestfinish);
                    if (0)
                        printf("-- msg o,g not available -- reinserting\n");
                    aq.push(elem);
                }

            } break;

            default:
                printf("not supported\n");
                break;
            }

            // do only ask hosts that actually completed something in this
            // round!
            //        new_events=false;
            //        std::sort(check_hosts.begin(), check_hosts.end());
            //        check_hosts.erase(unique(check_hosts.begin(),
            //        check_hosts.end()), check_hosts.end());
        }

        // How much ime inside NS3
        auto start_ns3 = high_resolution_clock::now();

        // Need support for RECV
        graph_node_properties recev_msg;
        recev_msg.updated = false;
        // We Run NS-3 if we have a compute message or if we have still some
        // data in the network
        printf("Current elem time is %ld while NS3 %ld\n", temp_elem.time,
               htsim_time);
        fflush(stdout);
        if (!lgs_interface
                     ->all_sends_delivered() /*&& temp_elem.type != OP_MSG*/) {
            if (temp_elem.time > htsim_time) {
                // printf("Running until\n");
                //  htsim_time = htsim_simulate_until(temp_elem.time,
                //  &recev_msg);
            } else {
                printf("Running infinite - AQ Size %d - Temp Time %lu vs Htsim "
                       "%lu - Global %lu\n",
                       aq.size(), temp_elem.time, htsim_time, GLOBAL_TIME);
                recev_msg = lgs_interface->htsim_simulate_until(100);
                htsim_time = GLOBAL_TIME;
            }
        }
        // How much time inside NS3
        auto t2 = high_resolution_clock::now();
        /* Getting number of milliseconds as an integer. */
        auto ms_int = duration_cast<milliseconds>(t2 - start_ns3);
        global_time += ms_int;
        // std::cout << "\n\nRunTime Run LGS -> " << ms_int.count() << "ms\n";
        // std::cout << "\n\nRunTime Total Partial LGS -> " <<
        // global_time.count() << "ms\n";

        // If the OP is NULL then we just continue
        if (recev_msg.updated) {
            printf("..... Received a MSG -- host %d to target %d, proc %d - "
                   "Type %d\n",
                   recev_msg.host, recev_msg.target, recev_msg.proc,
                   temp_elem.type);
            aq.push(recev_msg);
            lgs_interface->reset_latest_receive();
        } else {
            // If not NULL, we add it to the AQ
            printf("..... NOT Received a MSG\n");
        }

        host = 0;
        for (Parser::schedules_t::iterator sched = parser.schedules.begin();
             sched != parser.schedules.end(); ++sched, ++host) {
            // printf("Starting to parse new ops %d\n", 1);
            fflush(stdout);
            // host = *iter;
            // for(host = 0; host < p; host++)
            // SerializedGraph *sched=&parser.schedules[host];

            // retrieve all free operations
            SerializedGraph::nodelist_t free_ops;
            sched->GetExecutableNodes(&free_ops);
            // ensure that the free ops are ordered by type
            std::sort(free_ops.begin(), free_ops.end(), gnp_op_comp_func());

            // printf("Free Op Size %d\n", free_ops.size());

            // walk all new free operations and throw them in the queue
            for (SerializedGraph::nodelist_t::iterator freeop =
                         free_ops.begin();
                 freeop != free_ops.end(); ++freeop) {
                // if(print) std::cout << *freeop << " " ;
                // new_events = true;

                // assign host that it starts on
                freeop->host = host;

#ifdef STRICT_ORDER
                freeop->ts = aqtime++;
#endif
                printf("We arrive here %d\n", freeop->type);
                fflush(stdout);
                switch (freeop->type) {
                case OP_LOCOP:
                    freeop->time = nexto[host][freeop->proc];
                    if (print)
                        printf("%i (%i,%i) loclop: %lu, time: %lu, offset: "
                               "%i\n",
                               host, freeop->proc, freeop->nic,
                               (long unsigned int)freeop->size,
                               (long unsigned int)freeop->time, freeop->offset);
                    break;
                case OP_SEND:
                    freeop->time = htsim_time;
                    // freeop->time = std::max(nexto[host][freeop->proc],
                    // nextgs[host][freeop->nic]);
                    if (0)
                        printf("%i (%i,%i) send to: %i, tag: %i, size: %lu, "
                               "time: %lu, offset: %i\n",
                               host, freeop->proc, freeop->nic, freeop->target,
                               freeop->tag, (long unsigned int)freeop->size,
                               (long unsigned int)freeop->time, freeop->offset);
                    break;
                case OP_RECV:
                    freeop->time = nexto[host][freeop->proc];
                    if (print)
                        printf("%i (%i,%i) recvs from: %i, tag: %i, size: %lu, "
                               "time: %lu, offset: %i\n",
                               host, freeop->proc, freeop->nic, freeop->target,
                               freeop->tag, (long unsigned int)freeop->size,
                               (long unsigned int)freeop->time, freeop->offset);
                    break;
                default:
                    printf("not implemented!\n");
                }
                freeop->time = GLOBAL_TIME;
                printf("Unlocked operation host %d target %d at time %lu\n",
                       freeop->host, freeop->target, freeop->time);
                aq.push(*freeop);
            }
        }

        first_cycle = false;
        cycles++;
    }
    std::cout << "\n\nRunTime Total LGS -> " << global_time.count() << "ms\n";
    // ns3_terminate(htsim_time);
    lgs_interface->terminate_sim();
    printf("\n\nhtsim Terminates at %ld\n\n\n", htsim_time);
    printf("Remaining %lu\n", parser.schedules.size());

    gettimeofday(&tend, NULL);
    unsigned long int diff = tend.tv_sec - tstart.tv_sec;

#ifndef STRICT_ORDER
    ulint aqtime = 0;
#endif
    printf("PERFORMANCE: Processes: %i \t Events: %lu \t Time: %lu s \t Speed: "
           "%.2f ev/s\n",
           p, (long unsigned int)aqtime, (long unsigned int)diff,
           (float)aqtime / (float)diff);
    printf("AQ is %lu\n", aq.size());
    // check if all queues are empty!!
    bool ok = true;
    for (uint i = 0; i < p; ++i) {

#ifdef LIST_MATCH
        if (!uq[i].empty()) {
            printf("unexpected queue on host %i contains %lu elements!\n", i,
                   (ulint)uq[i].size());
            for (ruq_t::iterator iter = uq[i].begin(); iter != uq[i].end();
                 ++iter) {
                printf(" src: %i, tag: %i\n", iter->src, iter->tag);
            }
            ok = false;
        }
        if (!rq[i].empty()) {
            printf("receive queue on host %i contains %lu elements!\n", i,
                   (ulint)rq[i].size());
            for (ruq_t::iterator iter = rq[i].begin(); iter != rq[i].end();
                 ++iter) {
                printf(" src: %i, tag: %i\n", iter->src, iter->tag);
            }
            ok = false;
        }
#endif
    }

    if (ok) {
        if (p <= 10000 && !0) { // print all hosts
            printf("Times: \n");
            host = 0;
            for (uint i = 0; i < p; ++i) {
                btime_t maxo =
                        *(std::max_element(nexto[i].begin(), nexto[i].end()));
                // btime_t maxgr=*(std::max_element(nextgr[i].begin(),
                // nextgr[i].end())); btime_t
                // maxgs=*(std::max_element(nextgs[i].begin(),
                // nextgs[i].end())); std::cout << "Host " << i <<": "<<
                // std::max(std::max(maxgr,maxgs),maxo) << "\n";
                std::cout << "Host " << i << ": " << maxo << "\n";
            }

            long long unsigned int max = 0;
            int host = 0;
            for (uint i = 0; i < p; ++i) { // find maximum end time
                btime_t maxo =
                        *(std::max_element(nexto[i].begin(), nexto[i].end()));
                // btime_t maxgr=*(std::max_element(nextgr[i].begin(),
                // nextgr[i].end())); btime_t
                // maxgs=*(std::max_element(nextgs[i].begin(),
                // nextgs[i].end())); btime_t cur =
                // std::max(std::max(maxgr,maxgs),maxo);
                btime_t cur = maxo;
                if (cur > max) {
                    host = i;
                    max = cur;
                }
            }
            std::cout << "Maximum finishing time at host " << host << ": "
                      << max << " (" << (double)max / 1e9 << " s)\n";
        }

        // WRITE match queue statistics
        if (0) {
            char filename[1024];

            // Maximum RQ depth
            snprintf(filename, sizeof filename, "%d-%s", 0, "rq-max.data");
            std::ofstream rq_max_file(filename);

            if (!rq_max_file.is_open()) {
                std::cerr << "Can't open rq-max data file" << std::endl;
            } else {
                // WRITE one line per rank
                for (int n : rq_max) {
                    rq_max_file << n << std::endl;
                }
            }
            rq_max_file.close();

            // Maximum UQ depth
            snprintf(filename, sizeof filename, "%d-%s", 0, "uq-max.data");
            std::ofstream uq_max_file(filename);

            if (!uq_max_file.is_open()) {
                std::cerr << "Can't open uq-max data file" << std::endl;
            } else {
                // WRITE one line per rank
                for (int n : uq_max) {
                    uq_max_file << n << std::endl;
                }
            }
            uq_max_file.close();

            // RQ hit depth (number of elements searched for each successful
            // search)
            snprintf(filename, sizeof filename, "%d-%s", 0, "rq-hit.data");
            std::ofstream rq_hit_file(filename);

            if (!rq_hit_file.is_open()) {
                std::cerr << "Can't open rq-hit data file (" << filename << ")"
                          << std::endl;
            } else {
                // WRITE one line per rank
                for (auto per_rank_matches = rq_matches.begin();
                     per_rank_matches != rq_matches.end(); per_rank_matches++) {
                    for (auto match_pair = (*per_rank_matches).begin();
                         match_pair != (*per_rank_matches).end();
                         match_pair++) {
                        rq_hit_file << (*match_pair).first << ","
                                    << (*match_pair).second << " ";
                    }
                    rq_hit_file << std::endl;
                }
            }
            rq_hit_file.close();

            // UQ hit depth (number of elements searched for each successful
            // search)
            snprintf(filename, sizeof filename, "%d-%s", 0, "uq-hit.data");
            std::ofstream uq_hit_file(filename);

            if (!uq_hit_file.is_open()) {
                std::cerr << "Can't open uq-hit data file (" << filename << ")"
                          << std::endl;
            } else {
                // WRITE one line per rank
                for (auto per_rank_matches = uq_matches.begin();
                     per_rank_matches != uq_matches.end(); per_rank_matches++) {
                    for (auto match_pair = (*per_rank_matches).begin();
                         match_pair != (*per_rank_matches).end();
                         match_pair++) {
                        uq_hit_file << (*match_pair).first << ","
                                    << (*match_pair).second << " ";
                    }
                    uq_hit_file << std::endl;
                }
            }
            uq_hit_file.close();

            // RQ miss depth (number of elements searched for each unsuccessful
            // search)
            snprintf(filename, sizeof filename, "%d-%s", 0, "rq-miss.data");
            std::ofstream rq_miss_file(filename);

            if (!rq_miss_file.is_open()) {
                std::cerr << "Can't open rq-miss data file (" << filename << ")"
                          << std::endl;
            } else {
                // WRITE one line per rank
                for (auto per_rank_misses = rq_misses.begin();
                     per_rank_misses != rq_misses.end(); per_rank_misses++) {
                    for (auto miss_pair = (*per_rank_misses).begin();
                         miss_pair != (*per_rank_misses).end(); miss_pair++) {
                        rq_miss_file << (*miss_pair).first << ","
                                     << (*miss_pair).second << " ";
                    }
                    rq_miss_file << std::endl;
                }
            }
            rq_miss_file.close();

            // UQ miss depth (number of elements searched for each unsuccessful
            // search)
            snprintf(filename, sizeof filename, "%d-%s", 0, "uq-miss.data");
            std::ofstream uq_miss_file(filename);

            if (!uq_miss_file.is_open()) {
                std::cerr << "Can't open uq-miss data file (" << filename << ")"
                          << std::endl;
            } else {
                // WRITE one line per rank
                for (auto per_rank_misses = uq_misses.begin();
                     per_rank_misses != uq_misses.end(); per_rank_misses++) {
                    for (auto miss_pair = (*per_rank_misses).begin();
                         miss_pair != (*per_rank_misses).end(); miss_pair++) {
                        uq_miss_file << (*miss_pair).first << ","
                                     << (*miss_pair).second << " ";
                    }
                    uq_miss_file << std::endl;
                }
            }
            uq_miss_file.close();
        }
    }
    return 0;
}

int size_queue(std::vector<ruq_t> my_queue, int num_proce) {
    std::size_t max = 0;
    for (int i = 0; i < num_proce; i++) {
        if (my_queue[i].size() > max) {
            max = my_queue[i].size();
        }
    }
    return max;
}

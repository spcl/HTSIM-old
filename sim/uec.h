// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

#ifndef UEC_H
#define UEC_H

/*
 * A UEC source and sink
 */
#include "config.h"
#include "eventlist.h"
#include "fairpullqueue.h"
//#include "datacenter/logsim-interface.h"
#include "network.h"
#include "uecpacket.h"
#include <functional>
#include <list>
#include <map>

class UecSink;
// class LogSimInterface;

class SentPacket {
  public:
    SentPacket(simtime_picosec t, uint64_t s, bool a, bool n, bool to)
            : timer{t}, seqno{s}, acked{a}, nacked{n}, timedOut{to} {}
    SentPacket(const SentPacket &sp)
            : timer{sp.timer}, seqno{sp.seqno}, acked{sp.acked}, nacked{sp.nacked}, timedOut{sp.timedOut} {}
    simtime_picosec timer;
    uint64_t seqno;
    bool acked;
    bool nacked;
    bool timedOut;
};

class UecSrc : public PacketSink, public EventSource {
    friend class UecSink;

  public:
    UecSrc(UecLogger *logger, TrafficLogger *pktLogger, EventList &eventList, uint64_t rtt, uint64_t bdp,
           uint64_t queueDrainTime);
    // UecSrc(UecLogger *logger, TrafficLogger* pktLogger, EventList& eventList,
    // uint64_t rtt=timeFromUs(5.25), uint64_t bdp=63000);
    ~UecSrc();

    virtual void doNextEvent() override;

    void receivePacket(Packet &pkt) override;
    const string &nodename() override;

    virtual void connect(const Route &routeout, const Route &routeback, UecSink &sink, simtime_picosec startTime);
    void startflow();
    void set_paths(vector<const Route *> *rt);

    void setCwnd(uint64_t cwnd) { _cwnd = cwnd; };
    void setFlowSize(uint64_t flow_size) { _flow_size = flow_size; }
    void setKeepAcksInTargetRtt(bool keep) { _target_based_received = keep; }
    // void setLgs(LogSimInterface *lgs) { _lgs = lgs; };
    // void setUsingLgs(bool val) { _using_lgs = val; };
    void enableTrimming(bool enable) { _trimming_enabled = enable; };

    uint64_t getCwnd() { return _cwnd; };
    uint64_t getHighestSent() { return _highest_sent; }
    uint32_t getUnacked() { return _unacked; }
    uint32_t getConsecutiveLowRtt() { return _consecutive_low_rtt; }
    uint64_t getLastAcked() { return _last_acked; }
    uint32_t getReceivedSize() { return _received_ecn.size(); }
    uint32_t getRto() { return _rto; }
    bool supportsTrimming() { return _trimming_enabled; }
    std::size_t getEcnInTargetRtt();

    void set_traffic_logger(TrafficLogger *pktlogger);
    void set_flow_over_hook(std::function<void(const Packet &)> hook) { f_flow_over_hook = hook; }

    virtual void rtx_timer_hook(simtime_picosec now, simtime_picosec period);

    // should really be private, but loggers want to see:
    uint64_t _highest_sent; // seqno is in bytes
    uint64_t _packets_sent;
    uint64_t _new_packets_sent;
    uint64_t _rtx_packets_sent;
    uint64_t _acks_received;
    uint64_t _nacks_received;
    uint64_t _pulls_received;
    uint64_t _implicit_pulls;
    uint64_t _bounces_received;
    uint32_t _cwnd;
    uint64_t _last_acked;
    uint32_t _flight_size;
    uint32_t _acked_packets;
    uint64_t _flow_start_time;
    uint64_t _next_check_window;
    bool _start_timer_window = true;

  private:
    uint32_t _unacked;
    uint32_t _effcwnd;
    uint16_t _mss;
    uint64_t _flow_size;
    uint64_t _rtt;
    uint64_t _rto;
    uint64_t _rto_margin;
    uint64_t _rtx_timeout;
    uint64_t _maxcwnd;
    uint16_t _crt_path;
    // LogSimInterface *_lgs;
    bool _flow_finished = false;

    bool _rtx_timeout_pending;
    bool _rtx_pending;

    // new CC variables
    uint64_t _target_rtt;
    uint64_t _bdp;
    uint32_t _consecutive_low_rtt;
    uint64_t _ignore_ecn_until = 0;
    bool _target_based_received;
    bool _using_lgs = false;

    // SentPackets _sent_packets;
    uint64_t _highest_data_seq;

    UecLogger *_logger;
    UecSink *_sink;

    const Route *_route;
    vector<const Route *> _paths;
    PacketFlow _flow;

    string _nodename;
    std::function<void(const Packet &p)> f_flow_over_hook;

    list<std::tuple<simtime_picosec, bool, uint64_t>> _received_ecn; // list of packets received
    vector<SentPacket> _sent_packets;
    unsigned _nack_rtx_pending;
    vector<tuple<simtime_picosec, uint64_t, uint64_t, uint64_t>> _list_rtt;
    vector<pair<simtime_picosec, uint64_t>> _list_cwd;
    vector<pair<simtime_picosec, uint64_t>> _list_unacked;

    vector<const Route *> _good_entropies;
    bool _use_good_entropies;
    std::size_t _max_good_entropies;
    std::size_t _next_good_entropy;
    bool _enableDistanceBasedRtx;
    bool _trimming_enabled;

    void send_packets();
    uint64_t get_unacked();

    void adjust_window(simtime_picosec ts, bool ecn);
    bool no_ecn_last_target_rtt();
    bool ecn_congestion();
    void drop_old_received();
    const Route *get_path();
    void mark_received(UecAck &pkt);
    void add_ack_path(const Route *rt);
    bool resend_packet(std::size_t i);
    void retransmit_packet();
    void processAck(UecAck &pkt);
    std::size_t get_sent_packet_idx(uint32_t pkt_seqno);

    void update_rtx_time();
    void reduce_cwnd(uint64_t amount);
    void processNack(UecNack &nack);
    void reduce_unacked(uint64_t amount);
    void apply_timeout_penalty();
};

class UecSink : public PacketSink, public DataReceiver {
    friend class UecSrc;

  public:
    UecSink();

    void receivePacket(Packet &pkt) override;
    const string &nodename() override;

    uint64_t cumulative_ack() override;
    uint32_t drops() override;
    void connect(UecSrc &src, const Route &route);
    void set_paths(vector<const Route *> *rt);
    uint32_t from, to, tag;

  private:
    UecAck::seq_t _cumulative_ack;
    uint64_t _packets;
    uint32_t _drops;
    string _nodename;
    list<UecAck::seq_t> _received; // list of packets received OOO
    uint16_t _crt_path;
    const Route *_route;
    vector<const Route *> _paths;
    UecSrc *_src;

    void send_ack(simtime_picosec ts, bool marked, UecAck::seq_t seqno, UecAck::seq_t ackno, const Route *rt,
                  const Route *inRoute);
    void send_nack(simtime_picosec ts, bool marked, UecAck::seq_t seqno, UecAck::seq_t ackno, const Route *rt);
    bool already_received(UecPacket &pkt);
};

class UecRtxTimerScanner : public EventSource {
  public:
    UecRtxTimerScanner(simtime_picosec scanPeriod, EventList &eventlist);
    void doNextEvent();
    void registerUec(UecSrc &uecsrc);

  private:
    simtime_picosec _scanPeriod;
    simtime_picosec _lastScan;
    typedef list<UecSrc *> uecs_t;
    uecs_t _uecs;
};

#endif

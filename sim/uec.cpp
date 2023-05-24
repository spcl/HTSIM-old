// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-
#include "uec.h"
#include "ecn.h"
#include "queue.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <regex>
#include <stdio.h>
#include <utility>

#define timeInf 0

UecSrc::UecSrc(UecLogger *logger, TrafficLogger *pktLogger,
               EventList &eventList, uint64_t rtt, uint64_t bdp,
               uint64_t queueDrainTime, int hops)
        : EventSource(eventList, "uec"), _logger(logger), _flow(pktLogger) {
    _mss = Packet::data_packet_size();
    _unacked = 0;
    _nodename = "uecsrc";

    _last_acked = 0;
    _highest_sent = 0;
    _use_good_entropies = false;
    _next_good_entropy = 0;

    _nack_rtx_pending = 0;

    // new CC variables
    _hop_count = hops;
    _base_rtt = ((_hop_count * LINK_DELAY_MODERN) +
                 (PKT_SIZE_MODERN * 8 / LINK_SPEED_MODERN * _hop_count) +
                 (_hop_count * LINK_DELAY_MODERN) +
                 (64 * 8 / LINK_SPEED_MODERN * _hop_count)) *
                1000;
    _target_rtt = _base_rtt * 1.15;

    /*printf("Link Delay %lu - Link Speed %lu - Pkt Size %d - Base RTT %lu - "
           "Target RTT is %lu\n",
           LINK_DELAY_MODERN, LINK_SPEED_MODERN, PKT_SIZE_MODERN, _base_rtt,
           _target_rtt);*/

    _rtt = _base_rtt;
    _rto = rtt + _hop_count * queueDrainTime + (rtt * 90000);
    _rto_margin = _rtt / 2;
    _rtx_timeout = timeInf;
    _rtx_timeout_pending = false;
    _rtx_pending = false;
    _crt_path = 0;
    _flow_size = _mss * 934;
    _trimming_enabled = true;

    _bdp = (_base_rtt * LINK_SPEED_MODERN / 8) / 1000;
    _maxcwnd = _bdp;
    _cwnd = _bdp;
    _consecutive_low_rtt = 0;
    //_consecutive_no_ecn = 0;

    _target_based_received = true;

    _max_good_entropies = 8; // TODO: experimental value
    _enableDistanceBasedRtx = false;
    f_flow_over_hook = nullptr;
}

// Add deconstructor and save data once we are done.
UecSrc::~UecSrc() {
    // If we are collecting specific logs
    if (COLLECT_DATA) {
        // RTT
        std::string file_name = "../output/rtt/rtt" + _name + ".txt";
        std::ofstream MyFile(file_name, std::ios_base::app);

        for (const auto &p : _list_rtt) {
            MyFile << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ","
                   << get<3>(p) << std::endl;
        }

        MyFile.close();

        // CWD
        file_name = "../output/cwd/cwd" + _name + ".txt";
        std::ofstream MyFileCWD(file_name, std::ios_base::app);

        for (const auto &p : _list_cwd) {
            MyFileCWD << p.first << "," << p.second << std::endl;
        }

        MyFileCWD.close();

        // Unacked
        file_name = "../output/unacked/unacked" + _name + ".txt";
        std::ofstream MyFileUnack(file_name, std::ios_base::app);

        for (const auto &p : _list_unacked) {
            MyFileUnack << p.first << "," << p.second << std::endl;
        }

        MyFileUnack.close();

        // NACK
        file_name = "../output/nack/nack" + _name + ".txt";
        std::ofstream MyFileNack(file_name, std::ios_base::app);

        for (const auto &p : _list_nack) {
            MyFileNack << p.first << "," << p.second << std::endl;
        }

        MyFileNack.close();

        // US TO CS
        /*if (us_to_cs.size() > 0) {
            file_name = "../output/us_to_cs/us_to_cs" + _name + ".txt";
            std::ofstream MyFileUsToCs(file_name, std::ios_base::app);

            for (const auto &p : us_to_cs) {
                MyFileUsToCs << p.first << "," << p.second << std::endl;
            }

            MyFileUsToCs.close();
        }

        // LS TO US
        if (ls_to_us.size() > 0) {
            file_name = "../output/ls_to_us/ls_to_us" + _name + ".txt";
            std::ofstream MyFileLsToUs(file_name, std::ios_base::app);

            for (const auto &p : ls_to_us) {
                MyFileLsToUs << p.first << "," << p.second << std::endl;
            }

            MyFileLsToUs.close();
        }*/
    }
}

void UecSrc::doNextEvent() { startflow(); }

std::size_t UecSrc::get_sent_packet_idx(uint32_t pkt_seqno) {
    for (std::size_t i = 0; i < _sent_packets.size(); ++i) {
        if (pkt_seqno == _sent_packets[i].seqno) {
            return i;
        }
    }
    return _sent_packets.size();
}

void UecSrc::update_rtx_time() {
    _rtx_timeout = timeInf;
    for (const auto &sp : _sent_packets) {
        auto timeout = sp.timer;
        if (!sp.acked && !sp.nacked && !sp.timedOut &&
            (timeout < _rtx_timeout || _rtx_timeout == timeInf)) {
            _rtx_timeout = timeout;
        }
    }
}

void UecSrc::mark_received(UecAck &pkt) {
    // cummulative ack
    if (pkt.seqno() == 1) {
        while (!_sent_packets.empty() &&
               (_sent_packets[0].seqno <= pkt.ackno() ||
                _sent_packets[0].acked)) {
            _sent_packets.erase(_sent_packets.begin());
        }
        update_rtx_time();
        return;
    }
    if (_sent_packets.empty() || _sent_packets[0].seqno > pkt.ackno()) {
        // duplicate ACK -- since we support OOO, this must be caused by
        // duplicate retransmission
        return;
    }
    auto i = get_sent_packet_idx(pkt.seqno());
    if (i == 0) {
        // this should not happen because of cummulative acks, but
        // shouldn't cause harm either
        do {
            _sent_packets.erase(_sent_packets.begin());
        } while (!_sent_packets.empty() && _sent_packets[0].acked);
    } else {
        assert(i < _sent_packets.size());
        auto timer = _sent_packets[i].timer;
        auto seqno = _sent_packets[i].seqno;
        auto nacked = _sent_packets[i].nacked;
        _sent_packets[i] = SentPacket(timer, seqno, true, false, false);
        if (nacked) {
            --_nack_rtx_pending;
        }
        _last_acked = seqno + _mss - 1;
        if (_enableDistanceBasedRtx) {
            bool trigger = true;
            // TODO: this could be optimized with counters or bitsets,
            // but I'm doing this the simple way to avoid bugs while
            // we don't need the optimizations
            for (std::size_t k = 1; k < _sent_packets.size() / 2; ++k) {
                if (!_sent_packets[k].acked) {
                    trigger = false;
                    break;
                }
            }
            if (trigger) {
                // TODO: what's the proper way to act if this packet was
                // NACK'ed? Not super relevant right now as we are not enabling
                // this feature anyway
                _sent_packets[0].timer = eventlist().now();
                _rtx_timeout_pending = true;
            }
        }
    }
    update_rtx_time();
}

void UecSrc::add_ack_path(const Route *rt) {
    for (auto &r : _good_entropies) {
        if (r == rt) {
            return;
        }
    }
    if (_good_entropies.size() < _max_good_entropies) {
        _good_entropies.push_back(rt);
    } else {
        // TODO: this could cause some weird corner cases that would
        // preserve old entropies, but it probably won't be an issue
        // for simulations.
        // Example corner case: if a path is used, then the other
        // paths are replaced up to the point that _next_good_entropy
        // comes back to that path, it could be used again before any
        // of the newer paths.
        _good_entropies[_next_good_entropy] = rt;
        ++_next_good_entropy;
        _next_good_entropy %= _max_good_entropies;
    }
}

void UecSrc::set_traffic_logger(TrafficLogger *pktlogger) {
    _flow.set_logger(pktlogger);
}

void UecSrc::reduce_cwnd(uint64_t amount) {
    // printf("Reducing by %lu\n", amount);
    if (_cwnd >= amount + _mss) {
        _cwnd -= amount * 1;
    } else {
        _cwnd = _mss;
    }
}

void UecSrc::reduce_unacked(uint64_t amount) {
    if (_unacked >= amount) {
        _unacked -= amount;
    } else {
        // printf("Else2 - Unacked %d - Amount %lu\n", _unacked, amount);
        _unacked = 0;
    }
}

void UecSrc::processNack(UecNack &pkt) {
    if (GLOBAL_TIME > _ignore_ecn_until) {
        _list_cwd.push_back(std::make_pair(eventlist().now() / 1000, _cwnd));
        reduce_cwnd(_mss);
    }

    _list_nack.push_back(std::make_pair(eventlist().now() / 1000, 1));
    // mark corresponding packet for retransmission
    auto i = get_sent_packet_idx(pkt.seqno());
    assert(i < _sent_packets.size());

    assert(!_sent_packets[i].acked); // TODO: would it be possible for a packet
                                     // to receive a nack after being acked?
    if (!_sent_packets[i].nacked) {
        // ignore duplicate nacks for the same packet
        _sent_packets[i].nacked = true;
        ++_nack_rtx_pending;
    }

    /*
    // update ECN
    simtime_picosec ts = pkt.ts();
    bool marked = pkt.flags() &
                  ECN_ECHO; // ECN was marked on data packet and echoed on NACK
    _received_ecn.push_back(
            std::make_tuple(eventlist().now(), marked,
                            0)); // TODO: assuming same size for all packets
    if (marked) {
        adjust_window(ts, marked);
        }*/
    // retransmit only the NACK'ed packet
    bool success = resend_packet(i);
    if (!_rtx_pending && !success) {
        _rtx_pending = true;
    }
}

void UecSrc::processAck(UecAck &pkt) {
    UecAck::seq_t seqno = pkt.ackno();
    simtime_picosec ts = pkt.ts();
    if (seqno < _last_acked) {
        // return; // TODO: not for now
    }
    bool marked = pkt.flags() &
                  ECN_ECHO; // ECN was marked on data packet and echoed on ACK

    // printf("packet is ECN Marked %d - Time %lu\n", marked, GLOBAL_TIME /
    // 1000);

    if (_start_timer_window) {
        _start_timer_window = false;
        _next_check_window = GLOBAL_TIME + TARGET_RTT_MODERN * 1000;
    }
    uint64_t newRtt = eventlist().now() - ts;
    _received_ecn.push_back(std::make_tuple(
            eventlist().now(), marked, _mss,
            newRtt)); // TODO: assuming same size for all packets
    mark_received(pkt);

    add_ack_path(pkt.inRoute);

    if (!marked) {
        ++_consecutive_no_ecn;
    } else {
        _consecutive_no_ecn = 0;
    }
    if (newRtt < _target_rtt) {
        ++_consecutive_low_rtt;
    } else {
        _consecutive_low_rtt = 0;
    }

    // printf("Current Time %lu - New RTT is %lu - Sent Time %lu - FlowName
    // %s - ECN %d\n",(long long)eventlist().now(), (long long)newRtt, (long
    // long) ts, _name.c_str(), marked);
    _list_rtt.push_back(std::make_tuple(eventlist().now() / 1000, newRtt / 1000,
                                        pkt.seqno(), pkt.ackno()));

    // printf("Received Good Ack %lu vs %lu || %lu\n", seqno, _flow_size,
    //        _last_acked);
    if (seqno >= _flow_size && _sent_packets.empty() && !_flow_finished) {
        _flow_finished = true;
        if (f_flow_over_hook) {
            f_flow_over_hook(pkt);
        }

        cout << "Flow " << nodename() << " finished at "
             << timeAsMs(eventlist().now()) << endl;
        cout << "Flow " << nodename() << " completion time is "
             << timeAsMs(eventlist().now() - _flow_start_time) << endl;
    }

    if (seqno > _last_acked || true) { // TODO: new ack, we don't care about
                                       // ordering for now. Check later though
        if (seqno >= _highest_sent) {
            _highest_sent = seqno;
        }

        _last_acked = seqno;

        _list_cwd.push_back(std::make_pair(eventlist().now() / 1000, _cwnd));
        // printf("Window Is %d - From %d To %d\n", _cwnd, from, to);
        adjust_window(ts, marked);

        _effcwnd = _cwnd;
        send_packets();
        return; // TODO: if no further code, this can be removed
    }
}

uint64_t UecSrc::get_unacked() {
    // return _unacked;
    uint64_t missing = 0;
    for (const auto &sp : _sent_packets) {
        if (!sp.acked && !sp.nacked && !sp.timedOut) {
            missing += _mss;
        }
    }
    return missing;
}

void UecSrc::receivePacket(Packet &pkt) {
    // every packet received represents one less packet in flight

    reduce_unacked(_mss);

    // TODO: receive window?
    pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_RCVDESTROY);

    if (_logger) {
        _logger->logUec(*this, UecLogger::UEC_RCV);
    }
    switch (pkt.type()) {
    case UEC:
        // RTS
        break;
    case UECACK:
        // printf("NORMALACK");
        processAck(dynamic_cast<UecAck &>(pkt));
        break;
    case UECNACK:
        // printf("NACK %d\n", from);
        if (_trimming_enabled) {
            processNack(dynamic_cast<UecNack &>(pkt));
        }
        break;
    default:
        std::cout << "unknown packet receive with type code: " << pkt.type()
                  << "\n";
        return;
    }
    if (get_unacked() < _cwnd && _rtx_timeout_pending) {
        eventlist().sourceIsPendingRel(*this, 0);
    }
    pkt.free();
}

void UecSrc::adjust_window(simtime_picosec ts, bool ecn) {
    /*printf("From %d - Time %lu - Ecn %d - Consecutive Low %d - BDP %lu -
       CWD "
           "%d - Sizr "
           "%d - No ECN %d - MSS is %d\n",
           from, _eventlist.now() / 1000, ecn, _consecutive_no_ecn,
           (long long)_bdp, _cwnd, _received_ecn.size(),
           no_ecn_last_target_rtt(), _mss);*/
    if (ecn) {
        uint64_t total = 0;
        for (auto [ts, ecn, size, rtt] : _received_ecn) {
            total += size;
        }
        if (ecn_congestion() && GLOBAL_TIME >= _next_check_window &&
            _cwnd > 2 * total && ENABLE_FAST_DROP == true) {
            if (GLOBAL_TIME >= _next_check_window && _cwnd > 2 * total) {
                //_start_timer_window = true;
                _ignore_ecn_until = GLOBAL_TIME + (1 * BASE_RTT_MODERN * 1000 *
                                                   ((_cwnd / total) - 1));
                _start_timer_window = true;
                drop_old_received();
                _cwnd = total;
                reduce_cwnd(0); // fix cwnd if it goes below minimum
            }
        } else if (GLOBAL_TIME > _ignore_ecn_until) {
            reduce_cwnd(static_cast<double>(_cwnd) / _bdp * _mss);
        }
        // max(1.0, floor((double)_cwnd / _mss) * ((double)_cwnd / _bdp))) {
    } else if (true && no_ecn_last_target_rtt() &&
               no_rtt_over_target_last_target_rtt()) {
        _cwnd += _mss * ((double)_cwnd / _bdp);
        _consecutive_low_rtt = 0;
        _consecutive_no_ecn = 0;
        // printf("%d Else1 %lu\n", from, GLOBAL_TIME / 1000);
    } else if (false && _cwnd > ceil(sqrt(_bdp * _mss)) &&
               _consecutive_no_ecn >= ceil(_bdp / _cwnd)) {
        _cwnd += _mss * _mss * (_bdp / _cwnd) / _cwnd;
        _consecutive_no_ecn = 0;
        // printf("%d Else2 %lu\n", from, GLOBAL_TIME / 1000);
    } else if (!ecn) {
        // _consecutive_no_ecn >= floor((double)_cwnd / _mss) *
        // ((double)_cwnd / _bdp)
        //_cwnd += ((double)_mss / _cwnd) * _mss * ((double)_cwnd / _bdp);
        _cwnd += ((double)_mss / _cwnd) * 1 * _mss;
        //_cwnd += ((double)_mss / _cwnd) * _mss * ((double)_cwnd / _bdp);
        // printf("%d Else3 %lu\n", from, GLOBAL_TIME / 1000);
        _consecutive_no_ecn = 0;
    }

    if (_cwnd > _maxcwnd || false) {
        _cwnd = _maxcwnd;
    }
}

void UecSrc::drop_old_received() {
    if (true) {
        if (eventlist().now() > _target_rtt) {
            uint64_t lower_thresh = eventlist().now() - (_target_rtt * 1);
            while (!_received_ecn.empty() &&
                   std::get<0>(_received_ecn.front()) < lower_thresh) {
                _received_ecn.pop_front();
            }
        }
    } else {
        while (_received_ecn.size() > 10) {
            _received_ecn.pop_front();
        }
    }
}

bool UecSrc::no_ecn_last_target_rtt() {
    drop_old_received();
    for (const auto &[ts, ecn, size, rtt] : _received_ecn) {
        if (ecn) {
            return false;
        }
    }
    return true;
}

bool UecSrc::no_rtt_over_target_last_target_rtt() {
    drop_old_received();
    for (const auto &[ts, ecn, size, rtt] : _received_ecn) {
        if (rtt > _target_rtt) {
            return false;
        }
    }
    return true;
}

std::size_t UecSrc::getEcnInTargetRtt() {
    drop_old_received();
    std::size_t ecn_count = 0;
    for (const auto &[ts, ecn, size, rtt] : _received_ecn) {
        if (ecn) {
            ++ecn_count;
        }
    }
    return ecn_count;
}

bool UecSrc::ecn_congestion() {
    if (getEcnInTargetRtt() >= _received_ecn.size() / 2) {
        return true;
    }
    return false;
}

const string &UecSrc::nodename() { return _nodename; }

void UecSrc::connect(const Route &routeout, const Route &routeback,
                     UecSink &sink, simtime_picosec startTime) {
    _route = &routeout;

    assert(_route);
    _sink = &sink;
    _sink->connect(*this, routeback);

    eventlist().sourceIsPending(*this, startTime);
}

void UecSrc::startflow() {
    _flow_start_time = eventlist().now();
    send_packets();
}

const Route *UecSrc::get_path() {
    // TODO: add other ways to select paths
    if (_use_good_entropies && !_good_entropies.empty()) {
        auto rt = _good_entropies[_next_good_entropy];
        ++_next_good_entropy;
        _next_good_entropy %= _good_entropies.size();
        return rt;
    }
    _crt_path = random() % _paths.size();
    return _paths.at(_crt_path);
}

void UecSrc::send_packets() {
    if (_rtx_pending) {
        retransmit_packet();
    }
    _list_unacked.push_back(std::make_pair(eventlist().now() / 1000, _unacked));
    unsigned c = _cwnd;

    while ( //_last_acked + c >= _highest_sent + _mss &&
            get_unacked() + _mss <= c && _highest_sent <= _flow_size + 1) {
        uint64_t data_seq = 0;

        // choose path
        const Route *rt = get_path();

        // create packet
        UecPacket *p = UecPacket::newpkt(_flow, *rt, _highest_sent + 1,
                                         data_seq, _mss);
        p->from = this->from;
        p->to = this->to;
        p->tag = this->tag;
        // printf("Sending packet %d %d %d - Name %s - Time %ld\n",
        // this->from, this->to, this->tag, _name.c_str(), (long
        // long)GLOBAL_TIME//);

        p->flow().logTraffic(*p, *this, TrafficLogger::PKT_CREATESEND);
        p->set_ts(eventlist().now());

        // send packet
        _highest_sent += _mss; // XX beware wrapping
        _packets_sent += _mss;
        _unacked += _mss;

        // Getting time until packet is really sent
        PacketSink *sink = p->sendOn();
        HostQueue *q = dynamic_cast<HostQueue *>(sink);
        assert(q);
        uint32_t service_time = q->serviceTime(*p);
        _sent_packets.push_back(
                SentPacket(eventlist().now() + service_time + _rto, p->seqno(),
                           false, false, false));
        if (COLLECT_DATA) {
            for (size_t i = 0; i < rt->size(); i++) {
                if (i == 4) { // Intercept US TO CS
                    PacketSink *sink = rt->at(i);
                    if (sink->nodename().find("US_0") != std::string::npos) {
                        std::regex pattern("CS_(\\d+)");
                        std::smatch matches;
                        if (std::regex_search(sink->nodename(), matches,
                                              pattern)) {
                            std::string numberStr = matches[1].str();
                            int number = std::stoi(numberStr);
                            us_to_cs.push_back(std::make_pair(
                                    eventlist().now() / 1000 +
                                            (service_time / 1000),
                                    number));
                        }
                    }
                }
                if (i == 2) { // Intercept US TO CS
                    PacketSink *sink = rt->at(i);
                    if (sink->nodename().find("LS_0") != std::string::npos) {
                        std::regex pattern("US_(\\d+)");
                        std::smatch matches;
                        if (std::regex_search(sink->nodename(), matches,
                                              pattern)) {
                            std::string numberStr = matches[1].str();
                            int number = std::stoi(numberStr);
                            ls_to_us.push_back(std::make_pair(
                                    eventlist().now() / 1000 +
                                            (service_time / 1000),
                                    number));
                        }
                    }
                }
            }
        }

        if (_rtx_timeout == timeInf) {
            update_rtx_time();
        }
    }
}

void UecSrc::set_paths(vector<const Route *> *rt) {
    _paths.clear();

    for (const Route *route : *rt) {
        Route *t = new Route(*route, *_sink);
        t->add_endpoints(this, _sink);
        _paths.push_back(t);
    }
}

void UecSrc::apply_timeout_penalty() {
    if (_trimming_enabled) {
        reduce_cwnd(_mss);
    } else {
        _cwnd = _mss;
    }
}

void UecSrc::rtx_timer_hook(simtime_picosec now, simtime_picosec period) {
    // #ifndef RESEND_ON_TIMEOUT
    //     return; // TODO: according to ndp.cpp, rtx is not necessary with
    //     RTS. Check
    //             // if this applies to us
    // #endif

    if (_highest_sent == 0)
        return;
    if (_rtx_timeout == timeInf || now + period < _rtx_timeout)
        return;

    // here we can run into phase effects because the timer is checked
    // only periodically for ALL flows but if we keep the difference
    // between scanning time and real timeout time when restarting the
    // flows we should minimize them !
    if (!_rtx_timeout_pending) {
        _rtx_timeout_pending = true;
        apply_timeout_penalty();

        cout << "At " << timeAsUs(now) << "us RTO " << timeAsUs(_rto)
             << "us RTT " << timeAsUs(_rtt) << "us SEQ " << _last_acked / _mss
             << " CWND " << _cwnd / _mss << " Flow ID " << str() << endl;

        _cwnd = _mss;

        // check the timer difference between the event and the real value
        simtime_picosec too_early = _rtx_timeout - now;
        if (now > _rtx_timeout) {
            // This might happen because we hold on retransmitting if we
            // have enough packets in flight cout << "late_rtx_timeout: " <<
            // _rtx_timeout << " now: " << now
            //     << " now+rto: " << now + _rto << " rto: " << _rto <<
            //     endl;
            too_early = 0;
        }
        eventlist().sourceIsPendingRel(*this, too_early);
    }
}

bool UecSrc::resend_packet(std::size_t idx) {

    if (get_unacked() >= _cwnd) {
        return false;
    }
    assert(!_sent_packets[idx].acked);

    // this will cause retransmission not only of the offending
    // packet, but others close to timeout
    _rto_margin = _rtt / 2;

    const Route *rt;
    // if (_use_good_entropies && !_good_entropies.empty()) {
    //     rt = _good_entropies[_next_good_entropy];
    //     ++_next_good_entropy;
    //     _next_good_entropy %= _good_entropies.size();
    // } else {
    rt = get_path();
    // }
    // Getting time until packet is really sent
    _unacked += _mss;
    UecPacket *p = UecPacket::newpkt(_flow, *rt, _sent_packets[idx].seqno, 0,
                                     _mss, true);
    p->flow().logTraffic(*p, *this, TrafficLogger::PKT_CREATE);
    PacketSink *sink = p->sendOn();
    HostQueue *q = dynamic_cast<HostQueue *>(sink);
    assert(q);
    uint32_t service_time = q->serviceTime(*p);
    if (_sent_packets[idx].nacked) {
        --_nack_rtx_pending;
        _sent_packets[idx].nacked = false;
    }
    _sent_packets[idx].timer = eventlist().now() + service_time + _rto;
    _sent_packets[idx].timedOut = false;
    update_rtx_time();
    return true;
}

// retransmission for timeout
void UecSrc::retransmit_packet() {
    _rtx_pending = false;
    for (std::size_t i = 0; i < _sent_packets.size(); ++i) {
        auto &sp = _sent_packets[i];
        if (_rtx_timeout_pending && !sp.acked && !sp.nacked &&
            sp.timer <= eventlist().now() + _rto_margin) {
            _cwnd = _mss;
            sp.timedOut = true;
            reduce_unacked(_mss);
        }
        if (!sp.acked && (sp.timedOut || sp.nacked)) {
            if (!resend_packet(i)) {
                _rtx_pending = true;
            }
        }
    }
    _rtx_timeout_pending = false;
}

/**********
 * UecSink *
 **********/

UecSink::UecSink() : DataReceiver("sink"), _cumulative_ack{0}, _drops{0} {
    _nodename = "uecsink";
}

void UecSink::send_nack(simtime_picosec ts, bool marked, UecAck::seq_t seqno,
                        UecAck::seq_t ackno, const Route *rt) {

    UecNack *nack = UecNack::newpkt(_src->_flow, *rt, seqno, ackno, 0);
    nack->is_ack = false;
    nack->flow().logTraffic(*nack, *this, TrafficLogger::PKT_CREATESEND);
    nack->set_ts(ts);
    if (marked) {
        nack->set_flags(ECN_ECHO);
    } else {
        nack->set_flags(0);
    }

    nack->sendOn();
}

bool UecSink::already_received(UecPacket &pkt) {
    UecPacket::seq_t seqno = pkt.seqno();

    if (seqno <= _cumulative_ack) { // TODO: this assumes that all data
                                    // packets have the same size
        return true;
    }
    for (auto it = _received.begin(); it != _received.end(); ++it) {
        if (seqno == *it) {
            return true; // packet received OOO
        }
    }
    return false;
}

void UecSink::receivePacket(Packet &pkt) {
    switch (pkt.type()) {
    case UECACK:
    case UECNACK:
        // bounced, ignore
        pkt.free();
        return;
    case UEC:
        // do what comes after the switch
        break;
    default:
        std::cout << "unknown packet receive with type code: " << pkt.type()
                  << "\n";
        pkt.free();
        return;
    }
    UecPacket *p = dynamic_cast<UecPacket *>(&pkt);
    UecPacket::seq_t seqno = p->seqno();
    UecPacket::seq_t ackno = p->seqno() + p->data_packet_size() - 1;
    simtime_picosec ts = p->ts();

    bool marked = p->flags() & ECN_CE;

    // TODO: consider different ways to select paths
    auto crt_path = random() % _paths.size();
    if (already_received(*p)) {
        // duplicate retransmit
        if (_src->supportsTrimming()) { // we can assume that they have been
                                        // configured similarly, or
                                        // exchanged information about
                                        // options somehow
            send_ack(ts, marked, 1, _cumulative_ack, _paths.at(crt_path),
                     pkt.get_route());
        }
        return;
    }

    // packet was trimmed
    if (pkt.header_only()) {
        send_nack(ts, marked, seqno, ackno, _paths.at(crt_path));
        pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_RCVDESTROY);
        p->free();
        // cout << "trimmed packet";
        return;
    }

    int size = p->data_packet_size();
    // pkt._flow().logTraffic(pkt, *this, TrafficLogger::PKT_RCVDESTROY);
    p->free();

    _packets += size;

    if (seqno == _cumulative_ack + 1) { // next expected seq no
        _cumulative_ack = seqno + size - 1;
        seqno = 1;

        // handling packets received OOO
        while (!_received.empty() && _received.front() == _cumulative_ack + 1) {
            _received.pop_front();
            _cumulative_ack += size; // this assumes that all packets have
                                     // the same size
        }
        ackno = _cumulative_ack;
    } else if (seqno < _cumulative_ack + 1) { // already ack'ed
        // this space intentionally left empty
        seqno = 1;
        ackno = _cumulative_ack;
    } else { // not the next expected sequence number
        // TODO: what to do when a future packet is received?
        if (_received.empty()) {
            _received.push_front(seqno);
            _drops += (1000 + seqno - _cumulative_ack - 1) /
                      1000; // TODO: figure out what is this calculating
                            // exactly
        } else if (seqno > _received.back()) {
            _received.push_back(seqno);
        } else {
            for (auto it = _received.begin(); it != _received.end(); ++it) {
                if (seqno == *it)
                    break; // bad retransmit
                if (seqno < (*it)) {
                    _received.insert(it, seqno);
                    break;
                }
            }
        }
    }
    // TODO: reverse_route is likely sending the packet through the
    // same exact links, which is not correct in Packet Spray, but
    // there doesn't seem to be a good, quick way of doing that in
    // htsim
    send_ack(ts, marked, seqno, ackno, _paths.at(crt_path), pkt.get_route());
}

void UecSink::send_ack(simtime_picosec ts, bool marked, UecAck::seq_t seqno,
                       UecAck::seq_t ackno, const Route *rt,
                       const Route *inRoute) {

    UecAck *ack = UecAck::newpkt(_src->_flow, *rt, seqno, ackno, 0);
    ack->inRoute = inRoute;
    ack->is_ack = true;
    ack->flow().logTraffic(*ack, *this, TrafficLogger::PKT_CREATESEND);
    ack->set_ts(ts);
    if (marked) {
        ack->set_flags(ECN_ECHO);
    } else {
        ack->set_flags(0);
    }

    ack->from = this->from;
    ack->to = this->to;
    ack->tag = this->tag;
    ack->sendOn();
}

const string &UecSink::nodename() { return _nodename; }

uint64_t UecSink::cumulative_ack() { return _cumulative_ack; }

uint32_t UecSink::drops() { return _drops; }

void UecSink::connect(UecSrc &src, const Route &route) {
    _src = &src;
    _route = &route;
    _cumulative_ack = 0;
    _drops = 0;
}

void UecSink::set_paths(vector<const Route *> *rt) {
    _paths.clear();

    for (const Route *route : *rt) {
        Route *t = new Route(*route, *_src);
        t->add_endpoints(this, _src);
        _paths.push_back(t);
    }
}

/**********************
 * UecRtxTimerScanner *
 **********************/

UecRtxTimerScanner::UecRtxTimerScanner(simtime_picosec scanPeriod,
                                       EventList &eventlist)
        : EventSource(eventlist, "RtxScanner"), _scanPeriod{scanPeriod} {
    eventlist.sourceIsPendingRel(*this, 0);
}

void UecRtxTimerScanner::registerUec(UecSrc &uecsrc) {
    _uecs.push_back(&uecsrc);
}

void UecRtxTimerScanner::doNextEvent() {
    simtime_picosec now = eventlist().now();
    uecs_t::iterator i;
    for (i = _uecs.begin(); i != _uecs.end(); i++) {
        (*i)->rtx_timer_hook(now, _scanPeriod);
    }
    eventlist().sourceIsPendingRel(*this, _scanPeriod);
}

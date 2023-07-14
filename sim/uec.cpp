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
    _target_rtt = _base_rtt * 1.21;

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

    _target_based_received = true;

    _max_good_entropies = 10; // TODO: experimental value
    _enableDistanceBasedRtx = false;
    f_flow_over_hook = nullptr;
}

// Add deconstructor and save data once we are done.
UecSrc::~UecSrc() {
    // If we are collecting specific logs
    if (COLLECT_DATA) {
        // RTT
        std::string file_name = "../output/rtt/rtt" + _name + "_" +
                                std::to_string(tag) + ".txt";
        std::ofstream MyFile(file_name, std::ios_base::app);

        for (const auto &p : _list_rtt) {
            MyFile << get<0>(p) << "," << get<1>(p) << "," << get<2>(p) << ","
                   << get<3>(p) << std::endl;
        }

        MyFile.close();

        // CWD
        file_name = "../output/cwd/cwd" + _name + "_" + std::to_string(tag) +
                    ".txt";
        std::ofstream MyFileCWD(file_name, std::ios_base::app);

        for (const auto &p : _list_cwd) {
            MyFileCWD << p.first << "," << p.second << std::endl;
        }

        MyFileCWD.close();

        // Unacked
        file_name = "../output/unacked/unacked" + _name + "_" +
                    std::to_string(tag) + ".txt";
        std::ofstream MyFileUnack(file_name, std::ios_base::app);

        for (const auto &p : _list_unacked) {
            MyFileUnack << p.first << "," << p.second << std::endl;
        }

        MyFileUnack.close();

        // NACK
        file_name = "../output/nack/nack" + _name + "_" + std::to_string(tag) +
                    ".txt";
        std::ofstream MyFileNack(file_name, std::ios_base::app);

        for (const auto &p : _list_nack) {
            MyFileNack << p.first << "," << p.second << std::endl;
        }

        MyFileNack.close();

        // BTS
        if (_list_bts.size() > 0) {
            file_name = "../output/bts/bts" + _name + "_" +
                        std::to_string(tag) + ".txt";
            std::ofstream MyFileBTS(file_name, std::ios_base::app);

            for (const auto &p : _list_bts) {
                MyFileBTS << p.first << "," << p.second << std::endl;
            }

            MyFileBTS.close();
        }
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

// If we are here, it means we are not ECN Marked
void UecSrc::add_ack_path(const Route *rt) {
    // This might get tuned later to better match the pseudocode but it already
    // matches it logically. If we are not full, we just push to array.
    // Otherwise we overwrite the oldest element.
    if (_good_entropies.size() < _max_good_entropies) {
        _good_entropies.push_back(rt);
    } else {
        _good_entropies[0] = rt;
    }
}

void UecSrc::set_traffic_logger(TrafficLogger *pktlogger) {
    _flow.set_logger(pktlogger);
}

void UecSrc::reduce_cwnd(uint64_t amount) {
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
        _unacked = 0;
    }
}

void UecSrc::processNack(UecNack &pkt) {
    if (GLOBAL_TIME > _ignore_ecn_until) {
        _list_cwd.push_back(std::make_pair(eventlist().now() / 1000, _cwnd));
        reduce_cwnd(_mss);
    }
    _consecutive_no_ecn = 0;
    _consecutive_low_rtt = 0;
    _received_ecn.push_back(std::make_tuple(eventlist().now(), true, _mss,
                                            _target_rtt + 10000));

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

    // retransmit only the NACK'ed packet
    bool success = resend_packet(i);
    if (!_rtx_pending && !success) {
        _rtx_pending = true;
    }
}

/*void btsReduce() {
    int cwnd = 0;
    float alpha = 0.5;
    float queue_deviation = (delta * observed_ecn) / max_ecn;
    float observed_queue_occupancy = cwnd =
            (delta * observed_ecn) / max_ecn + kmin + bdp;
    cwnd - (cwnd * alpha *
            (queue_deviation / (observed_queue_occupancy) / (cwnd / _mss)))
}*/

void UecSrc::processBts(UecPacket *pkt) {
    _list_cwd.push_back(std::make_pair(eventlist().now() / 1000, _cwnd));

    // Update variables
    _consecutive_no_ecn = 0;
    _consecutive_low_rtt = 0;
    _received_ecn.push_back(std::make_tuple(eventlist().now(), true, _mss,
                                            _target_rtt + 10000));
    _list_nack.push_back(std::make_pair(eventlist().now() / 1000, 1));

    if (!pkt->_queue_full) {
        return;
    }

    // mark corresponding packet for retransmission
    pkt->unbounce(64 + _mss);
    auto i = get_sent_packet_idx(pkt->seqno());
    assert(i < _sent_packets.size());

    assert(!_sent_packets[i].acked); // TODO: would it be possible for a packet
                                     // to receive a nack after being acked?
    if (!_sent_packets[i].nacked) {
        // ignore duplicate nacks for the same packet
        _sent_packets[i].nacked = true;
        ++_nack_rtx_pending;
    }

    // retransmit only the NACK'ed packet
    bool success = resend_packet(i);
    if (!_rtx_pending && !success) {
        _rtx_pending = true;
    }
    pkt->free();
}

void UecSrc::processAck(UecAck &pkt, bool force_marked) {
    UecAck::seq_t seqno = pkt.ackno();
    simtime_picosec ts = pkt.ts();
    if (seqno < _last_acked) {
        // return; // TODO: not for now
    }
    bool marked = pkt.flags() &
                  ECN_ECHO; // ECN was marked on data packet and echoed on ACK

    if (_start_timer_window) {
        _start_timer_window = false;
        _next_check_window = GLOBAL_TIME + TARGET_RTT_MODERN * 1000;
    }
    uint64_t newRtt = eventlist().now() - ts;
    _received_ecn.push_back(std::make_tuple(
            eventlist().now(), marked, _mss,
            newRtt)); // TODO: assuming same size for all packets
    mark_received(pkt);

    if (!marked) {
        add_ack_path(pkt.inRoute);
        ++_consecutive_no_ecn;
    } else {
        _consecutive_no_ecn = 0;
    }
    if (newRtt < _target_rtt) {
        ++_consecutive_low_rtt;
    } else {
        _consecutive_low_rtt = 0;
    }

    _list_rtt.push_back(std::make_tuple(eventlist().now() / 1000, newRtt / 1000,
                                        pkt.seqno(), pkt.ackno()));

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
    // We received an ACK, BTS Queue Full Event or NACK. We can release the
    // packet size.
    if (pkt._queue_full || !pkt.bounced()) {
        reduce_unacked(pkt.acked_size);
    }

    // Logging
    pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_RCVDESTROY);
    if (_logger) {
        _logger->logUec(*this, UecLogger::UEC_RCV);
    }

    // Check what kind of packet we have received
    switch (pkt.type()) {
    case UECACK:
        // Normal ACK, We do some standard processing on it
        processAck(dynamic_cast<UecAck &>(pkt), false);

        // Increase CWND
        if (!RTT_Exceeded_Seen() && !BTS_Seen()) {
            _cwnd += ((double)_cwnd / _bdp) * pkt.acked_size;
        } else {
            _cwnd += ((double)pkt.acked_size / _cwnd) * _mss;
        }

        // Release the packet
        pkt.free();
        break;
    case UEC:
        // BTS Case
        if (_bts_enabled) {
            if (pkt.bounced()) {
                _cwnd -= ((double)(_cwnd) / _bdp) *
                         (pkt.acked_size * (pkt.queue_status / _max_level_bts));
                if (pkt._queue_full) {
                    // We retransmit here
                    processBts((UecPacket *)(&pkt));
                } else {
                    pkt.free();
                }
            }
        }
        break;
    case UECNACK:
        // Trimmed Packet
        if (_trimming_enabled) {
            processNack(dynamic_cast<UecNack &>(pkt));
            pkt.free();
        }
        break;
    default:
        std::cout << "unknown packet receive with type code: " << pkt.type()
                  << "\n";
        return;
    }

    // Check cwnd boundries
    if (_cwnd > _maxcwnd) {
        _cwnd = _maxcwnd;
    }

    // For timeout purposes
    if (get_unacked() < _cwnd && _rtx_timeout_pending) {
        eventlist().sourceIsPendingRel(*this, 0);
    }
}

void UecSrc::adjust_window(simtime_picosec ts, bool ecn) {

    if (_bts_enabled) {
        if (ecn && _ignore_ecn_ack) {
        } else if (!RTT_Exceeded_Seen() && !BTS_Seen()) {
            _cwnd += _mss * ((double)_cwnd / _bdp);
            _consecutive_low_rtt = 0;
            _consecutive_no_ecn = 0;
        } else if (!ecn) {
            _cwnd += ((double)_mss / _cwnd) * 1 * _mss;
            _consecutive_no_ecn = 0;
        }
    } else {
        if (ecn) {
            reduce_cwnd(static_cast<double>(_cwnd) / _bdp * _mss);
        } else if (true && !RTT_Exceeded_Seen() && !BTS_Seen()) {
            _cwnd += _mss * ((double)_cwnd / _bdp);
            _consecutive_low_rtt = 0;
            _consecutive_no_ecn = 0;
        } else if (false && _cwnd > ceil(sqrt(_bdp * _mss)) &&
                   _consecutive_no_ecn >= ceil(_bdp / _cwnd)) {
            _cwnd += _mss * _mss * (_bdp / _cwnd) / _cwnd;
            _consecutive_no_ecn = 0;
        } else if (!ecn) {
            _cwnd += ((double)_mss / _cwnd) * 1 * _mss;
            _consecutive_no_ecn = 0;
        }
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

bool UecSrc::BTS_Seen() {
    drop_old_received();
    for (const auto &[ts, ecn, size, rtt] : _received_ecn) {
        // Here we consider either ECN or BTS
        if (ecn) {
            return true;
        }
    }
    return false;
}

bool UecSrc::RTT_Exceeded_Seen() {
    drop_old_received();
    for (const auto &[ts, ecn, size, rtt] : _received_ecn) {
        if (rtt > _target_rtt) {
            return true;
        }
    }
    return false;
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
    // Use Good Entropies is a flag to enable this logic
    if (_use_good_entropies && !_good_entropies.empty()) {
        auto rt = _good_entropies.front();
        _good_entropies.erase(_good_entropies.begin());
        return rt;
    }

    // Means we want to select a random one out of all paths.
    // This holds true if we don't set a fixed number of entropies
    if (_num_entropies == -1) {
        _crt_path = random() % _paths.size();
    } else {
        // Else we use our entropy array of a certain size and roud robin it
        _crt_path = _entropy_array[current_entropy];
        current_entropy = ++current_entropy % _num_entropies;
    }

    return _paths.at(_crt_path);
}

void UecSrc::map_entropies() {
    for (int i = 0; i < _num_entropies; i++) {
        _entropy_array.push_back(random() % _paths.size());
    }
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
    map_entropies();
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
        if (pkt.bounced()) {
            printf("Bounced at Sink, no sense\n");
        }
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

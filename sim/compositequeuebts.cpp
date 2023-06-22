// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
#include "compositequeuebts.h"
#include "ecn.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <math.h>
#include <regex>
#include <sstream>
#include <stdio.h>
#include <utility>

CompositeQueueBts::CompositeQueueBts(linkspeed_bps bitrate, mem_b maxsize,
                                     EventList &eventlist, QueueLogger *logger)
        : Queue(bitrate, maxsize, eventlist, logger) {
    _ratio_high = 100000;
    _ratio_low = 1;
    _crt = 0;
    _num_headers = 0;
    _num_packets = 0;
    _num_acks = 0;
    _num_nacks = 0;
    _num_pulls = 0;
    _num_drops = 0;
    _num_stripped = 0;
    _num_bounced = 0;
    _ecn_minthresh = maxsize * 2; // don't set ECN by default
    _ecn_maxthresh = maxsize * 2; // don't set ECN by default
    _bts_triggering = maxsize;

    _queuesize_high = _queuesize_low = 0;
    _serv = QUEUE_INVALID;
    stringstream ss;
    ss << "compqueue(" << bitrate / 1000000 << "Mb/s," << maxsize << "bytes)";
    _nodename = ss.str();
}

void CompositeQueueBts::beginService() {
    if (!_enqueued_high.empty() && !_enqueued_low.empty()) {
        _crt++;

        if (_crt >= (_ratio_high + _ratio_low))
            _crt = 0;

        if (_crt < _ratio_high) {
            _serv = QUEUE_HIGH;
            eventlist().sourceIsPendingRel(*this,
                                           drainTime(_enqueued_high.back()));
        } else {
            assert(_crt < _ratio_high + _ratio_low);
            _serv = QUEUE_LOW;
            eventlist().sourceIsPendingRel(*this,
                                           drainTime(_enqueued_low.back()));
        }
        return;
    }

    if (!_enqueued_high.empty()) {
        _serv = QUEUE_HIGH;
        eventlist().sourceIsPendingRel(*this, drainTime(_enqueued_high.back()));
    } else if (!_enqueued_low.empty()) {
        _serv = QUEUE_LOW;
        eventlist().sourceIsPendingRel(*this, drainTime(_enqueued_low.back()));
    } else {
        assert(0);
        _serv = QUEUE_INVALID;
    }
}

bool CompositeQueueBts::decide_ECN() {
    // ECN mark on deque
    printf("Min ECN %d - Max ECN %d\n",
           int(_ecn_minthresh * 8 / (_bitrate / 1e9)),
           int(_ecn_maxthresh * 8 / (_bitrate / 1e9)));
    if (_queuesize_low > _ecn_maxthresh) {
        return true;
    } else if (_queuesize_low > _ecn_minthresh) {
        uint64_t p = (0x7FFFFFFF * (_queuesize_low - _ecn_minthresh)) /
                     (_ecn_maxthresh - _ecn_minthresh);
        if ((uint64_t)random() < p) {
            return true;
        }
    }
    return false;
}

void CompositeQueueBts::completeService() {
    Packet *pkt;
    if (_serv == QUEUE_LOW) {

        assert(!_enqueued_low.empty());
        pkt = _enqueued_low.pop();
        _queuesize_low -= pkt->size();

        // ECN mark on deque
        if (decide_ECN()) {
            // pkt->set_flags(pkt->flags() | ECN_CE);
            if (COLLECT_DATA) {
                /*std::string file_name = "../output/ecn/ecn" +
                                        std::to_string(pkt->from) + "_" +
                                        std::to_string(pkt->to) + ".txt";
                std::ofstream MyFile(file_name, std::ios_base::app);

                MyFile << eventlist().now() / 1000 << "," << 1 << std::endl;

                MyFile.close();*/
            }
        }

        if (COLLECT_DATA && !pkt->header_only()) {
            if (_nodename.find("US_0") != std::string::npos &&
                pkt->type() == UEC) {
                std::regex pattern("-CS_(\\d+)");
                std::smatch matches;
                if (std::regex_search(_nodename, matches, pattern)) {
                    std::string numberStr = matches[1].str();
                    int number = std::stoi(numberStr);
                    std::string file_name =
                            "../output/us_to_cs/us_to_cs" + _name + ".txt";
                    std::ofstream MyFileUsToCs(file_name, std::ios_base::app);

                    MyFileUsToCs << eventlist().now() / 1000 << "," << number
                                 << std::endl;

                    MyFileUsToCs.close();
                }
            }
            // printf("Test1 %s\n", _nodename.c_str());
            if (_nodename.find("LS0") != std::string::npos &&
                pkt->type() == UEC) {
                // printf("Test2 %s\n", _nodename.c_str());
                std::regex pattern(">US(\\d+)");
                std::smatch matches;
                if (std::regex_search(_nodename, matches, pattern)) {
                    std::string numberStr = matches[1].str();
                    int number = std::stoi(numberStr);
                    std::string file_name =
                            "../output/ls_to_us/ls_to_us" + _name + ".txt";
                    std::ofstream MyFileUsToCs(file_name, std::ios_base::app);

                    MyFileUsToCs << eventlist().now() / 1000 << "," << number
                                 << std::endl;

                    MyFileUsToCs.close();
                }
            }
        }

        if (_logger)
            _logger->logQueue(*this, QueueLogger::PKT_SERVICE, *pkt);
        _num_packets++;
    } else if (_serv == QUEUE_HIGH) {
        assert(!_enqueued_high.empty());
        pkt = _enqueued_high.pop();
        _queuesize_high -= pkt->size();
        if (_logger)
            _logger->logQueue(*this, QueueLogger::PKT_SERVICE, *pkt);
        if (pkt->type() == NDPACK)
            _num_acks++;
        else if (pkt->type() == NDPNACK)
            _num_nacks++;
        else if (pkt->type() == NDPPULL)
            _num_pulls++;
        else {
            // cout << "Hdr: type=" << pkt->type() << endl;
            _num_headers++;
            // ECN mark on deque of a header, if low priority queue is still
            // over threshold
            // if (decide_ECN()) {
            //    pkt->set_flags(pkt->flags() | ECN_CE);
            //}
        }
    } else {
        assert(0);
    }

    pkt->flow().logTraffic(*pkt, *this, TrafficLogger::PKT_DEPART);
    pkt->sendOn();

    //_virtual_time += drainTime(pkt);

    _serv = QUEUE_INVALID;

    // cout << "E[ " << _enqueued_low.size() << " " << _enqueued_high.size() <<
    // " ]" << endl;

    if (!_enqueued_high.empty() || !_enqueued_low.empty())
        beginService();
}

void CompositeQueueBts::doNextEvent() { completeService(); }

void CompositeQueueBts::receivePacket(Packet &pkt) {
    pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_ARRIVE);
    if (_logger)
        _logger->logQueue(*this, QueueLogger::PKT_ARRIVE, pkt);
    // is this a Tofino packet from the egress pipeline?
    if (!pkt.header_only()) {
        //  Queue
        if (COLLECT_DATA) {
            if (_queuesize_low != 0) {
                std::string file_name =
                        "../output/queue/queue" +
                        _nodename.substr(_nodename.find(")") + 1) + ".txt";
                std::ofstream MyFile(file_name, std::ios_base::app);

                MyFile << eventlist().now() / 1000 << ","
                       << int(_queuesize_low * 8 / (_bitrate / 1e9))
                       << std::endl;

                MyFile.close();
            }
        }
        if (_queuesize_low + pkt.size() > _bts_triggering ||
            _queuesize_low + pkt.size() > _maxsize || decide_ECN()) {
            // If queue is full, we send it back
            if (_queuesize_low + pkt.size() > _maxsize) {
                pkt._queue_full = true;
            } else {
                pkt._queue_full = false;
            }
            printf("BTS Case\n");
            pkt.queue_status =
                    ((_queuesize_low + pkt.size()) * 64.0) / _maxsize;
            pkt.strip_payload();
            pkt.bounce();
            pkt.reverse_route();
            _num_bounced++;
            pkt.sendOn();
            return;
        } else {
            Packet *pkt_p = &pkt;
            _enqueued_low.push(pkt_p);
            _queuesize_low += pkt.size();
            if (_logger)
                _logger->logQueue(*this, QueueLogger::PKT_ENQUEUE, pkt);

            if (_serv == QUEUE_INVALID) {
                beginService();
            }

            return;
        }

        assert(_queuesize_low + pkt.size() <= _maxsize);
        Packet *pkt_p = &pkt;
        _enqueued_low.push(pkt_p);
        _queuesize_low += pkt.size();
        if (_logger)
            _logger->logQueue(*this, QueueLogger::PKT_ENQUEUE, pkt);

        if (_serv == QUEUE_INVALID) {
            beginService();
        }

        // cout << "BL[ " << _enqueued_low.size() << " " <<
        // _enqueued_high.size() << " ]" << endl;

        return;
    } else {
        assert(pkt.header_only());

        if (_queuesize_high + pkt.size() > 200 * _maxsize) {
            // drop header
            // cout << "drop!\n";
            if (pkt.reverse_route() && pkt.bounced() == false) {
                // return the packet to the sender
                if (_logger)
                    _logger->logQueue(*this, QueueLogger::PKT_BOUNCE, pkt);
                pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_BOUNCE);
                // XXX what to do with it now?
#if 0
            printf("Bounce1 at %s\n", _nodename.c_str());
            printf("Fwd route:\n");
            print_route(*(pkt.route()));
            printf("nexthop: %d\n", pkt.nexthop());
#endif
                pkt.bounce();
#if 0
            printf("\nRev route:\n");
            print_route(*(pkt.reverse_route()));
            printf("nexthop: %d\n", pkt.nexthop());
#endif
                _num_bounced++;
                pkt.sendOn();
                return;
            } else {
                if (_logger)
                    _logger->logQueue(*this, QueueLogger::PKT_DROP, pkt);
                pkt.flow().logTraffic(pkt, *this, TrafficLogger::PKT_DROP);
                cout << "B[ " << _enqueued_low.size() << " "
                     << _enqueued_high.size() << " ] DROP "
                     << pkt.flow().get_id() << endl;
                pkt.free();
                _num_drops++;
                return;
            }
        }

        // if (pkt.type()==NDP)
        //   cout << "H " << pkt.flow().str() << endl;
        Packet *pkt_p = &pkt;
        _enqueued_high.push(pkt_p);
        _queuesize_high += pkt.size();
        if (_logger)
            _logger->logQueue(*this, QueueLogger::PKT_ENQUEUE, pkt);

        // cout << "BH[ " << _enqueued_low.size() << " " <<
        // _enqueued_high.size() << " ]" << endl;

        if (_serv == QUEUE_INVALID) {
            beginService();
        }
    }
}

mem_b CompositeQueueBts::queuesize() const {
    return _queuesize_low + _queuesize_high;
}

// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
#include "queue_lossless_input.h"
#include "ecn.h"
#include "switch.h"
#include <iostream>
#include <math.h>
#include <sstream>

uint64_t LosslessInputQueue::_high_threshold = 0;
uint64_t LosslessInputQueue::_low_threshold = 0;

LosslessInputQueue::LosslessInputQueue(EventList &eventlist)
        : Queue(speedFromGbps(1), Packet::data_packet_size() * 2000, eventlist,
                NULL),
          VirtualQueue(), _state_recv(READY) {
    assert(_high_threshold > 0);
    assert(_high_threshold > _low_threshold);
}

LosslessInputQueue::LosslessInputQueue(EventList &eventlist, BaseQueue *peer)
        : Queue(speedFromGbps(1), Packet::data_packet_size() * 2000, eventlist,
                NULL),
          VirtualQueue(), _state_recv(READY) {
    assert(_high_threshold > 0);
    assert(_high_threshold > _low_threshold);

    stringstream ss;
    ss << "VirtualQueue(" << peer->_name << ")";
    _nodename = ss.str();
    _remoteEndpoint = peer;
    _switch = NULL;

    peer->setRemoteEndpoint(this);
}

LosslessInputQueue::LosslessInputQueue(EventList &eventlist, BaseQueue *peer,
                                       Switch *sw)
        : Queue(speedFromGbps(1), Packet::data_packet_size() * 2000, eventlist,
                NULL),
          VirtualQueue(), _state_recv(READY) {
    assert(_high_threshold > 0);
    assert(_high_threshold > _low_threshold);

    stringstream ss;
    ss << "VirtualQueue(" << peer->_name << ")";
    _nodename = ss.str();
    _remoteEndpoint = peer;
    _switch = sw;

    assert(_switch);

    peer->setRemoteEndpoint(this);
}

void LosslessInputQueue::receivePacket(Packet &pkt) {
    /* normal packet, enqueue it */
    _queuesize += pkt.size();

    printf("LosslessInputQueue received (%s / %d) : %d %d %d - Time %ld\n",
           _nodename.c_str(), pkt.from, _queuesize, _high_threshold,
           _state_recv, eventlist().now() / 1000);
    // send PAUSE notifications if that is the case!
    assert(_queuesize > 0);
    if ((uint64_t)_queuesize > _high_threshold && _state_recv != PAUSED) {
        printf("Sending Pause");
        _state_recv = PAUSED;
        sendPause(1000);
        pkt.pfc_just_happened = true;
    }

    // if (_state_recv==PAUSED)
    // cout << timeAsMs(eventlist().now()) << " queue " << _name << " switch ("
    // << _switch->_name << ") "<< " recv when paused pkt " << pkt.type() << "
    // sz " << _queuesize << endl;

    printf("IN: MaxQueueSize %d - CurrentSize %d - PFC High %d - PFC Low %d\n",
           _maxsize, _queuesize, _high_threshold, _low_threshold);

    if (_queuesize > _maxsize) {
        cout << " Queue " << _name
             << " LOSSLESS not working! I should have dropped this packet"
             << _queuesize / Packet::data_packet_size() << endl;
    }

    // tell the output queue we're here!
    if (pkt.nexthop() < pkt.route()->size()) {
        // this should not work...
        // assert(0);
        pkt.sendOn2(this);
    } else {
        assert(_switch);
        pkt.set_ingress_queue(this);
        _switch->receivePacket(pkt);
    }
}

bool LosslessInputQueue::decide_ECN() {
    // ECN mark on deque
    if (_queuesize > _ecn_maxthresh) {
        return true;
    } else if (_queuesize > _ecn_minthresh) {
        uint64_t p = (0x7FFFFFFF * (_queuesize - _ecn_minthresh)) /
                     (_ecn_maxthresh - _ecn_minthresh);
        if ((uint64_t)random() < p) {
            return true;
        }
    }
    return false;
}

void LosslessInputQueue::completedService(Packet &pkt) {
    _queuesize -= pkt.size();

    if (decide_ECN()) {
        pkt.set_flags(pkt.flags() | ECN_CE);
    }

    // unblock if that is the case
    assert(_queuesize >= 0);
    if ((uint64_t)_queuesize < _low_threshold && _state_recv == PAUSED) {
        _state_recv = READY;
        sendPause(0);
    }
}

void LosslessInputQueue::sendPause(unsigned int wait) {
    // cout << "Ingress link " << getRemoteEndpoint() << " PAUSE " << wait <<
    // endl;
    uint32_t switchID = 0;
    if (_switch) {
        switchID = getSwitch()->getID();
    } else {
        printf("Possible error\n");
    }

    printf("\nI am %s (%d) - Remote EndPoint is %s\n", nodename().c_str(),
           switchID, getRemoteEndpoint()->nodename().c_str());

    pfc_happened = true;
    EthPausePacket *pkt = EthPausePacket::newpkt(wait, switchID);
    getRemoteEndpoint()->receivePacket(*pkt);
};

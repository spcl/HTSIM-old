// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
#ifndef CBR_PACKET
#define CBR_PACKET

#include "network.h"
#include <list>

#define ACKSIZE 64

class CbrPacket : public Packet {
  public:
    static PacketDB<CbrPacket> _packetdb;
    inline static CbrPacket *newpkt(PacketFlow &flow, route_t &route, int id, int size) {
        CbrPacket *p = _packetdb.allocPacket();
        p->set_route(flow, route, size, id);
        return p;
    }

    virtual inline void strip_payload() {
        Packet::strip_payload();
        _size = ACKSIZE;
    };

    void free() { _packetdb.freePacket(this); }
    virtual ~CbrPacket(){};
};
#endif

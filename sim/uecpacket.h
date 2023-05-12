#ifndef UECPACKET_H
#define UECPACKET_H

#include "network.h"
#include <list>

// UecPacket and UecAck are subclasses of Packet.
// They incorporate a packet database, to reuse packet objects that are no
// longer needed. Note: you never construct a new UecPacket or UecAck directly;
// rather you use the static method newpkt() which knows to reuse old packets
// from the database.

class UecPacket : public Packet {
  public:
    typedef uint64_t seq_t;

    UecPacket() : Packet(){};

    inline static UecPacket *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, seq_t dataseqno, int size) {
        UecPacket *p = _packetdb.allocPacket();
        p->set_route(flow, route, size + acksize,
                     seqno + size - 1); // The UEC sequence number is the first byte of the
                                        // packet; I will ID the packet by its last byte.
        p->_type = UEC;
        p->_is_header = false;
        p->_bounced = false;
        p->_seqno = seqno;
        p->_data_seqno = dataseqno;
        p->_syn = false;
        p->retransmitted = false;
        p->_flags = 0;
        return p;
    }

    inline static UecPacket *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, int size) {
        return newpkt(flow, route, seqno, 0, size);
    }

    void free() { _packetdb.freePacket(this); }
    virtual ~UecPacket() {}
    inline seq_t seqno() const { return _seqno; }
    inline seq_t data_seqno() const { return _data_seqno; }
    inline simtime_picosec ts() const { return _ts; }
    inline void set_ts(simtime_picosec ts) { _ts = ts; }
    virtual inline void strip_payload() {
        Packet::strip_payload();
        _size = acksize;
    };

    bool retransmitted;
    // inline simtime_picosec ts() const {return _ts;}
    // inline void set_ts(simtime_picosec ts) {_ts = ts;}
    const static int acksize = 64;

  protected:
    seq_t _seqno, _data_seqno;
    bool _syn;
    simtime_picosec _ts;
    static PacketDB<UecPacket> _packetdb;
};

class UecAck : public Packet {
  public:
    typedef UecPacket::seq_t seq_t;

    UecAck() : Packet(){};

    inline static UecAck *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, seq_t ackno, seq_t dackno) {
        UecAck *p = _packetdb.allocPacket();
        p->set_route(flow, route, acksize, ackno);
        p->_bounced = false;
        p->_type = UECACK;
        p->_seqno = seqno;
        p->_ackno = ackno;
        p->_data_ackno = dackno;
        p->_is_header = true;
        p->_flags = 0;

        return p;
    }

    inline static UecAck *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, seq_t ackno) {
        return newpkt(flow, route, seqno, ackno, 0);
    }

    void free() { _packetdb.freePacket(this); }
    inline seq_t seqno() const { return _seqno; }
    inline seq_t ackno() const { return _ackno; }
    inline seq_t data_ackno() const { return _data_ackno; }
    inline simtime_picosec ts() const { return _ts; }
    inline void set_ts(simtime_picosec ts) { _ts = ts; }
    // inline simtime_picosec ts() const {return _ts;}
    // inline void set_ts(simtime_picosec ts) {_ts = ts;}

    virtual ~UecAck() {}
    const static int acksize = 64;
    const Route *inRoute;

  protected:
    seq_t _seqno;
    seq_t _ackno, _data_ackno;
    simtime_picosec _ts;
    static PacketDB<UecAck> _packetdb;
};

class UecNack : public Packet {
  public:
    typedef UecPacket::seq_t seq_t;

    UecNack() : Packet(){};

    inline static UecNack *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, seq_t ackno, seq_t dackno) {
        UecNack *p = _packetdb.allocPacket();
        p->set_route(flow, route, acksize, ackno);
        p->_bounced = false;
        p->_type = UECNACK;
        p->_seqno = seqno;
        p->_ackno = ackno;
        p->_data_ackno = dackno;
        p->_is_header = true;
        p->_flags = 0;

        return p;
    }

    inline static UecNack *newpkt(PacketFlow &flow, const Route &route, seq_t seqno, seq_t ackno) {
        return newpkt(flow, route, seqno, ackno, 0);
    }

    void free() { _packetdb.freePacket(this); }
    inline seq_t seqno() const { return _seqno; }
    inline seq_t ackno() const { return _ackno; }
    inline seq_t data_ackno() const { return _data_ackno; }
    inline simtime_picosec ts() const { return _ts; }
    inline void set_ts(simtime_picosec ts) { _ts = ts; }
    // inline simtime_picosec ts() const {return _ts;}
    // inline void set_ts(simtime_picosec ts) {_ts = ts;}

    virtual ~UecNack() {}
    const static int acksize = 64;

  protected:
    seq_t _seqno;
    seq_t _ackno, _data_ackno;
    simtime_picosec _ts;
    // simtime_picosec _ts;
    static PacketDB<UecNack> _packetdb;
};

#endif

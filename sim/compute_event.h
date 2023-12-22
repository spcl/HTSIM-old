// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

#ifndef COMPUTEEVENT_H
#define COMPUTEEVENT_H

/*
 * A UEC source and sink
 */
#include "config.h"
#include "eventlist.h"
#include "fairpullqueue.h"
//#include "datacenter/logsim-interface.h"
#include "network.h"
#include "trigger.h"
#include "uecpacket.h"
#include <functional>
#include <list>
#include <map>

class ComputeEvent : public EventSource {

  public:
    ComputeEvent(UecLogger *logger, EventList &eventList);

    virtual void doNextEvent() override;

    void set_compute_over_hook(std::function<void(int)> hook) {
        f_compute_over_hook = hook;
    }

    void setCompute(simtime_picosec computation_time);

    // void receivePacket(Packet &pkt) override;
    // const string &nodename() override;

    // virtual void activate() { startflow(); }
    // void set_end_trigger(Trigger &trigger);

    std::function<void(int)> f_compute_over_hook;

  private:
};

#endif

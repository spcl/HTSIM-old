// -*- c-basic-offset: 4; indent-tabs-mode: nil -*-
#include "config.h"
#include "tcppacket.h"
#include <filesystem>
#include <math.h>

double drand() {
    int r = rand();
    int m = RAND_MAX;
    double d = (double)r / (double)m;
    return d;
}

int pareto(int xm, int mean) {
    double oneoveralpha = ((double)mean - xm) / mean;
    return (int)((double)xm / pow(drand(), oneoveralpha));
}

double exponential(double lambda) { return -log(drand()) / lambda; }

simtime_picosec timeFromSec(double secs) {
    simtime_picosec psecs = (simtime_picosec)(secs * 1000000000000.0);
    return psecs;
}

simtime_picosec timeFromMs(double msecs) {
    simtime_picosec psecs = (simtime_picosec)(msecs * 1000000000);
    return psecs;
}

simtime_picosec timeFromMs(int msecs) {
    simtime_picosec psecs = (simtime_picosec)((uint64_t)msecs * 1000000000);
    return psecs;
}

simtime_picosec timeFromUs(double usecs) {
    simtime_picosec psecs = (simtime_picosec)(usecs * 1000000);
    return psecs;
}

simtime_picosec timeFromUs(uint32_t usecs) {
    simtime_picosec psecs = (simtime_picosec)((uint64_t)usecs * 1000000);
    return psecs;
}

simtime_picosec timeFromNs(double nsecs) {
    simtime_picosec psecs = (simtime_picosec)(nsecs * 1000);
    return psecs;
}

double timeAsMs(simtime_picosec ps) {
    double ms_ = (double)(ps / 1000000000.0);
    return ms_;
}

double timeAsUs(simtime_picosec ps) {
    double us_ = (double)(ps / 1000000.0);
    return us_;
}

double timeAsSec(simtime_picosec ps) {
    double s_ = (double)ps / 1000000000000.0;
    return s_;
}

mem_b memFromPkt(double pkts) {
    mem_b m = (mem_b)(ceil(pkts * Packet::data_packet_size()));
    return m;
}

linkspeed_bps speedFromGbps(double Gbitps) {
    double bps = Gbitps * 1000000000;
    return (linkspeed_bps)bps;
}

linkspeed_bps speedFromMbps(uint64_t Mbitps) {
    uint64_t bps;
    bps = Mbitps * 1000000;
    return bps;
}

linkspeed_bps speedFromMbps(double Mbitps) {
    double bps = Mbitps * 1000000;
    return (linkspeed_bps)bps;
}

linkspeed_bps speedFromKbps(uint64_t Kbitps) {
    uint64_t bps;
    bps = Kbitps * 1000;
    return bps;
}

linkspeed_bps speedFromPktps(double packetsPerSec) {
    double bitpersec = packetsPerSec * 8 * Packet::data_packet_size();
    linkspeed_bps spd = (linkspeed_bps)bitpersec;
    return spd;
}

double speedAsPktps(linkspeed_bps bps) {
    double pktps = ((double)bps) / (8.0 * Packet::data_packet_size());
    return pktps;
}

mem_pkts memFromPkts(double pkts) { return (int)(ceil(pkts)); }

void initializeLoggingFolders() {
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/rtt/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/ecn/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/cwd/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/queue/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/acked/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/sent/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/nack/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/bts/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/ls_to_us/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/us_to_cs/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/fasti/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/fastd/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/mediumi/");
    std::filesystem::remove_all("/home/tommaso/csg-htsim/sim/output/ecn_rtt/");
    std::filesystem::remove_all(
            "/home/tommaso/csg-htsim/sim/output/trimmed_rtt/");

    bool ret_val = std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/rtt");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/ecn");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/cwd");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/queue");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/acked");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/sent");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/nack");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/bts");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/ls_to_us");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/us_to_cs");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/fastd");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/fasti");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/mediumi");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/ecn_rtt");
    ret_val &= std::filesystem::create_directory(
            "/home/tommaso/csg-htsim/sim/output/trimmed_rtt");
}
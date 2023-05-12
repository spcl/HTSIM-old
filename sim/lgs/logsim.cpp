/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "logsim.h"

/*
 * Copyright (c) 2009 The Trustees of Indiana University and Indiana
 *                    University Research and Technology
 *                    Corporation.  All rights reserved.
 *
 * Author(s): Torsten Hoefler <htor@cs.indiana.edu>
 *            Timo Schneider <timoschn@cs.indiana.edu>
 *
 */

#include "cmdline.h"
#include <queue>

#include <sys/time.h>

#define DEBUG_PRINT 1

static bool print = false;

// Returns highest queue size between processes
int size_queue(std::vector<ruq_t> my_queue, int num_proce) {
    int max = 0;
    for (int i = 0; i < num_proce; i++) {
        if (my_queue[i].size() > max) {
            max = my_queue[i].size();
        }
    }
    return max;
}

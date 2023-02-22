#ifndef _runtime_h_
#define _runtime_h_

struct RuntimeData {
    double lastTickTimestamp;
    double litresTotal = 0;
    unsigned long ticksCount = 0;
    double currentFlow;
    bool relayStatus;
    float tickIntervalSec;
};

#endif
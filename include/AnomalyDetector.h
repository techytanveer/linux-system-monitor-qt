#ifndef ANOMALY_DETECTOR_H
#define ANOMALY_DETECTOR_H

#include <QString>
#include <QList>
#include "SystemInfo.h" // ProcessInfo struct

class AnomalyDetector {
public:
    static QString check(double currentCpu, const QList<ProcessInfo>& processes);

private:
    static inline QList<double> cpuHistory = {};
    static const int maxHistory = 100;
};

#endif

#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

#include <QString>
#include <QList>

struct ProcessInfo {
    int pid;
    QString name;
    double cpuUsage;
    double memoryMb;
};

struct SystemData {

    int coreCount;

    double ramTotal; // GB
    double ramUsed;  // GB
    double diskTotal; // GB
    double diskUsed;  // GB
    double cpuTemp;
    double cpuLoad;

    QString cpuModel;
    QString osName;
    QString kernelVersion;
    QString uptime;

    // network mon - Current speeds to display in UI
    double downloadSpeed = 0.0; // in KB/s or MB/s
    double uploadSpeed = 0.0;

    // network mon - Total bytes (to calculate the delta)
    long long totalBytesRecv = 0;
    long long totalBytesSent = 0;

    QList<ProcessInfo> topProcesses; // Processes Information

};

class SystemInfo {
	
public:   // Constructor ---

    static SystemData getSnapshot();

    static void getTopProcesses(SystemData &data);

    static QString checkAnomalies(double currentCpu, const QList<ProcessInfo>& processes);

private:  // This is PERSISTENT MEMORY ---

    static long long lastRecv;
    static long long lastSent;
    static QList<double> cpuHistory;
    static const int maxHistory = 100;

};

#endif

#include "../include/AnomalyDetector.h"

QString AnomalyDetector::check(double currentCpu, const QList<ProcessInfo>& processes) {
	
    cpuHistory.append(currentCpu);

    if (cpuHistory.size() > maxHistory) cpuHistory.removeFirst();

    if (cpuHistory.size() < 10) return "Analyzing baseline...";

    double sum = 0;
    for(double val : cpuHistory) sum += val;
    double mean = sum / cpuHistory.size();

    if (currentCpu > mean + 40.0 && currentCpu > 50.0) {
        return QString("⚠️ HIGH CPU: %1%").arg(currentCpu, 0, 'f', 1);
    }

    for (const auto& proc : processes) {
        if (proc.memoryMb > 1536.0) { 
            return QString("⚠️ HIGH RAM: %1 (%2 MB)").arg(proc.name).arg(proc.memoryMb, 0, 'f', 0);
        }
    }

    return "System Healthy";
}

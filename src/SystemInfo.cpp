#include "SystemInfo.h"
#include <QFile>
#include <QTextStream>
#include <QStorageInfo>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <thread>
#include <QDebug>
#include <QRegularExpression>
#include <fstream>
#include <sstream>
#include <algorithm>

QList<double> SystemInfo::cpuHistory;

long long SystemInfo::lastRecv = 0;
long long SystemInfo::lastSent = 0;

SystemData SystemInfo::getSnapshot() {
    SystemData data;

    // 1. CPU Model & Cores
    
    QFile cpuFile("/proc/cpuinfo");
    if (cpuFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&cpuFile);

        QString line = in.readLine();
	while (in.readLineInto(&line)) { 

	    if (line.startsWith("model name", Qt::CaseInsensitive)) {
        	QStringList parts = line.split(":");
        	if (parts.size() > 1) {
            	data.cpuModel = parts.at(1).trimmed();
            	break; // We found what we need, stop the loop!
        	}
    	    }
	}


        /*
        THIS COMMENT IS FOR BELOW ISSUE:
        On some Linux kernels (especially on Ubuntu 22.04+), 
        /proc/cpuinfo doesn't report a file size. 
        This can sometimes make atEnd() behave unexpectedly 
        if the stream hasn't "primed" its buffer yet.

        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.contains("model name")) {
                data.cpuModel = line.split(":").at(1).trimmed();
                break;
            }
        }
	*/
    }


    data.coreCount = std::thread::hardware_concurrency();

    // 2. RAM Info (Using sysinfo syscall)
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        double unit = 1024.0 * 1024.0 * 1024.0;
        data.ramTotal = (si.totalram * (unsigned long)si.mem_unit) / unit;
        data.ramUsed = ((si.totalram - si.freeram) * (unsigned long)si.mem_unit) / unit;
        
        long seconds = si.uptime;
        data.uptime = QString("%1d %2h %3m")
                      .arg(seconds / 86400)
                      .arg((seconds % 86400) / 3600)
                      .arg((seconds % 3600) / 60);
    }

    // 3. Disk Info (Qt internal library)
    QStorageInfo storage = QStorageInfo::root();
    data.diskTotal = storage.bytesTotal() / (1024.0 * 1024.0 * 1024.0);
    data.diskUsed = (storage.bytesTotal() - storage.bytesAvailable()) / (1024.0 * 1024.0 * 1024.0);

    // 4. OS & Kernel
    struct utsname osInfo;
    uname(&osInfo);
    data.kernelVersion = QString(osInfo.release);

    QFile osFile("/etc/os-release");
    if (osFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&osFile);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (line.startsWith("PRETTY_NAME")) {
		QString rawName = line.split("=").value(1);
		data.osName = rawName.replace("\"", "").trimmed();

                // data.osName = line.split("=").at(1).remove('"');
                break;
            }
        }
    }

    // 5. CPU Temperature
    for (int i = 0; i < 10; ++i) {
        QString basePath = QString("/sys/class/hwmon/hwmon%1/").arg(i);
        QFile nameFile(basePath + "name");

        if (nameFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString sensorName = nameFile.readAll().trimmed();
            nameFile.close();

            // 'coretemp' is Intel, 'k10temp' is AMD
            if (sensorName == "coretemp" || sensorName == "k10temp") {
                // temp1_input is usually the 'Package' or 'Die' temperature
                QFile tempFile(basePath + "temp1_input");
                if (tempFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    QString raw = tempFile.readAll().trimmed();
                    // Linux sensors report in millidegrees (e.g. 50000 = 50°C)
                    data.cpuTemp = raw.toDouble() / 1000.0;
                    tempFile.close();
                    break; // Found it, stop looking
                }
            }
        }
    }

    // 6. NETWORK stats

    long long currentRecv = 0;
    long long currentSent = 0;

    QFile netFile("/proc/net/dev");
    if (netFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&netFile);

        /* REMOVED: preferred to use Read-and-Check loop below
        while (!in.atEnd()) {
	   
        QString line = in.readLine();
	    // Skip headers and the loopback (lo) interface  
            if (line.contains(":") && !line.contains("lo:")) {

            // Extract everything after the interface name (e.g., eth0:)
            if (line.contains(":")) {
                QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
                if (parts.size() >= 10) {
                    // Column 1 is Received Bytes, Column 9 is Transmitted Bytes
                    currentRecv += parts[1].toLongLong();
                    currentSent += parts[9].toLongLong();
                }
            }
	    }
        }
        */

        // Read-and-Check loop: introduced coz of virtual file to addr atEnd()
        while (true) {
            QString line = in.readLine();
            if (line.isNull()) break; // Exit loop if we hit the end of the file

            line = line.trimmed();
        
            // Skip headers and loopback
            if (!line.contains(":") || line.startsWith("lo")) {
                continue;
            }

            // Now we process the actual data lines
            QString dataPart = line.section(':', 1);
            QStringList parts = dataPart.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

            if (parts.size() >= 9) {
                currentRecv += parts[0].toLongLong(); 
                currentSent += parts[8].toLongLong(); 

		// DEBUG start
		/*long long recv = parts[0].toLongLong();
                long long sent = parts[8].toLongLong();

                // DEBUG LINE: Run your app from terminal to see this
                qDebug() << "Interface Found! Recv:" << recv << "Sent:" << sent;

                currentRecv += recv;
                currentSent += sent;*/
		// DEBUG end
		
            }
        } 
        netFile.close();

        // Calculate Speed (Bytes per second)
        // Note: This assumes getSnapshot is called exactly once per second


        //qDebug() << "BEFORE CURRENT: " << currentRecv << " : " << currentSent;
        //qDebug() << "last: " << lastRecv << " : " << lastSent;

        if (lastRecv > 0) {
            data.downloadSpeed = (currentRecv - lastRecv) / 1024.0; // KB/s
            data.uploadSpeed = (currentSent - lastSent) / 1024.0;   // KB/s
        }

        // Update the "Last Total" for the next calculation

        data.totalBytesRecv = currentRecv; // this has become useless 
        data.totalBytesSent = currentSent; // this has become useless
        
        lastRecv = currentRecv; // New variable for presistant
        lastSent = currentSent; // New variable for presistant 

    }

    // 7. PROCESSES stats
    getTopProcesses(data);

    // 8. CPU Load
    
    // Calculate deltas using static members
    static long long prevTotal = 0;
    static long long prevIdle = 0;
    std::ifstream file("/proc/stat");
    if (file.is_open()) {

    std::string line;
    if (std::getline(file, line) && line.compare(0, 3, "cpu") == 0) {

        std::stringstream ss(line);
        std::string cpuLabel;
        long long user, nice, system, idle, iowait, irq, softirq, steal;

        // Skip the "cpu" prefix and read the 8 core metrics
        ss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

        long long currentIdle = idle + iowait;
        long long currentNonIdle = user + nice + system + irq + softirq + steal;
        long long currentTotal = currentIdle + currentNonIdle;

        // Calculate deltas using static members
        // static long long prevTotal = 0;
        // static long long prevIdle = 0;

        long long totalDelta = currentTotal - prevTotal;
        long long idleDelta = currentIdle - prevIdle;

        // Avoid division by zero on the first run
        if (prevTotal == 0 || totalDelta == 0) {
            data.cpuLoad = 0.0;
        } else {
            // Correct formula: (Total - Idle) / Total
            double percentage = (static_cast<double>(totalDelta - idleDelta) / totalDelta) * 100.0;
        
            // Clamp between 0 and 100 to avoid weird peaks
            data.cpuLoad = std::max(0.0, std::min(100.0, percentage));
        }

        // Update static trackers for the next call
        prevTotal = currentTotal;
        prevIdle = currentIdle;

        }
        file.close();
    }
    

    // All System Info fetch ended before this line
    return data;
}

void SystemInfo::getTopProcesses(SystemData &data) {
    data.topProcesses.clear();
    QDir procDir("/proc");
    QStringList entries = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QString &pidStr : entries) {
        bool isNumber;
        pidStr.toInt(&isNumber);
        if (!isNumber) continue;

        QFile localFile(QString("/proc/%1/status").arg(pidStr));
        if (localFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&localFile);
            ProcessInfo proc;
            proc.pid = pidStr.toInt();
            bool hasMem = false;

            // Use the "Network Style" loop: Read until a null line is returned
            while (true) {
                QString line = in.readLine();
                if (line.isNull()) break; // End of virtual file reached

                line = line.trimmed();
                if (line.startsWith("Name:")) {
                    proc.name = line.section(':', 1).simplified();
                } else if (line.startsWith("VmRSS:")) {
                    QString val = line.section(':', 1).simplified().split(" ")[0];
                    proc.memoryMb = val.toDouble() / 1024.0;
                    hasMem = true;
                }
            }
            
            // Only include processes that are actually using memory
            if (hasMem && proc.memoryMb > 0) {
                data.topProcesses.append(proc);
            }
            localFile.close();
        }
    }

    // Sort by memory descending
    std::sort(data.topProcesses.begin(), data.topProcesses.end(), 
              [](const ProcessInfo &a, const ProcessInfo &b) {
        return a.memoryMb > b.memoryMb;
    });

    if (data.topProcesses.size() > 4) data.topProcesses.resize(4); // # of. processes to list
}

/*/////// DELETE


// Calculate deltas using static members
static long long prevTotal = 0;
static long long prevIdle = 0;
std::ifstream file("/proc/stat");
if (file.is_open()) {

    std::string line;
    if (std::getline(file, line) && line.compare(0, 3, "cpu") == 0) {

        std::stringstream ss(line);
        std::string cpuLabel;
        long long user, nice, system, idle, iowait, irq, softirq, steal;

        // Skip the "cpu" prefix and read the 8 core metrics
        ss >> cpuLabel >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;

        long long currentIdle = idle + iowait;
        long long currentNonIdle = user + nice + system + irq + softirq + steal;
        long long currentTotal = currentIdle + currentNonIdle;

        // Calculate deltas using static members
        // static long long prevTotal = 0;
        // static long long prevIdle = 0;

        long long totalDelta = currentTotal - prevTotal;
        long long idleDelta = currentIdle - prevIdle;

        // Avoid division by zero on the first run
        if (prevTotal == 0 || totalDelta == 0) {
            data.cpuLoad = 0.0;
        } else {
            // Correct formula: (Total - Idle) / Total
            double percentage = (static_cast<double>(totalDelta - idleDelta) / totalDelta) * 100.0;
        
            // Clamp between 0 and 100 to avoid weird peaks
            data.cpuLoad = std::max(0.0, std::min(100.0, percentage));
        }

        // Update static trackers for the next call
        prevTotal = currentTotal;
        prevIdle = currentIdle;

    }
    file.close();
} */

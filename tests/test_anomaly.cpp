#include "../include/AnomalyDetector.h"
#include <QCoreApplication>
#include <iostream>
#include <cassert>

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);
    QList<ProcessInfo> emptyList;

    // 1. Establish a low baseline (Average will be around 10%)
    for(int i = 0; i < 15; ++i) {
        AnomalyDetector::check(10.0, emptyList);
    }

    // 2. Trigger a sudden spike (99% is much higher than the 10% average)
    QString result = AnomalyDetector::check(99.0, emptyList);
    
    std::cout << "Result after spike: " << result.toStdString() << std::endl;

    if (result.contains("HIGH CPU")) {
        std::cout << "Test Passed!" << std::endl;
        return 0; 
    } else {
        std::cerr << "Test Failed: Spike not detected. Current result: " 
                  << result.toStdString() << std::endl;
        return 1; 
    }

    // Test Case 3: Memory Anomaly
    ProcessInfo badProc;
    badProc.name = "MemoryEater";
    badProc.memoryMb = 2500.0; // 2.5 GB (Above our 1.5GB threshold)

    QList<ProcessInfo> procList;
    procList.append(badProc);

    QString memResult = AnomalyDetector::check(10.0, procList);

    if (memResult.contains("Memory Leak")) {
      std::cout << "Test 3 Passed: Memory anomaly detected!" << std::endl;
    } else {
      std::cerr << "Test 3 Failed: High RAM process was ignored!" << std::endl;
      return 1;
    }



}

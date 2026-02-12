#include "MainWindow.h"
#include "SystemInfo.h"
#include "AnomalyDetector.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QTimer>
#include <QScreen>
#include <QGuiApplication>
#include <QStyle>
#include <QTableWidget>
#include <QHeaderView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Linux System Monitor");

    this->setStyleSheet(
        "QMainWindow { background-color: #2b2b2b; }"
        "QGroupBox { color: #ffffff; font-weight: bold; border: 1px solid #555555; margin-top: 10px; padding: 10px; }"
        "QLabel { color: #e0e0e0; }"
        "QProgressBar { "
        "   border: 1px solid #444; "
        "   border-radius: 3px; "
        "   background-color: #1e1e1e; "
        "   text-align: center; "
        "   color: white; "
        "}"
    );

    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );

    setMinimumSize(600, 500);

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    QGroupBox *group = new QGroupBox("System Statistics", this);
    QGridLayout *gridLayout = new QGridLayout(group);

    // Create labels
    auto addRow = [&](const QString &title, int row) {
        gridLayout->addWidget(new QLabel("<b>" + title + "</b>"), row, 0);
        QLabel *valLabel = new QLabel("Loading...");
        gridLayout->addWidget(valLabel, row, 1);
        return valLabel;
    };

    lblCPU = addRow("CPU Model:", 0);
    lblCores = addRow("CPU Cores:", 1);

    // RAM Section

    gridLayout->addWidget(new QLabel("<b>RAM Usage:</b>"), 2, 0);
    barRAM = new QProgressBar(this);
    barRAM->setRange(0, 100);
    barRAM->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; }"
                      "QProgressBar::chunk { background-color: #3498db; }");
    gridLayout->addWidget(barRAM, 2, 1);
    lblRAM = new QLabel("Loading...");
    gridLayout->addWidget(lblRAM, 2, 2);

    // Disk Section

    gridLayout->addWidget(new QLabel("<b>Disk Usage:</b>"), 3, 0);
    barDisk = new QProgressBar(this);
    barDisk->setRange(0, 100);
    barDisk->setStyleSheet("QProgressBar { border: 1px solid grey; border-radius: 5px; text-align: center; }"
                       "QProgressBar::chunk { background-color: #2ecc71; }");
    gridLayout->addWidget(barDisk, 3, 1);
    lblDisk = new QLabel("Loading...");
    gridLayout->addWidget(lblDisk, 3, 2);


    lblOS = addRow("OS Name:", 4);
    lblKernel = addRow("Kernel:", 5);
    lblUptime = addRow("System Uptime:", 6);

    // CPU Temp Section

    gridLayout->addWidget(new QLabel("<b>CPU Temp:</b>"), 7, 0);
    lblTemp = new QLabel("Reading...");
    lblTemp->setStyleSheet("font-weight: bold; font-size: 13px;");
    gridLayout->addWidget(lblTemp, 7, 1);

    // NETWORK Section

    gridLayout->addWidget(new QLabel("<b>Download:</b>"), 8, 0);
    lblNetDown = new QLabel("0 KB/s");
    gridLayout->addWidget(lblNetDown, 8, 1);

    gridLayout->addWidget(new QLabel("<b>Upload:</b>"), 9, 0);
    lblNetUp = new QLabel("0 KB/s");
    gridLayout->addWidget(lblNetUp, 9, 1);

    // CPU USAGE SECTION
    
    //statusLabel = new QLabel("Initializing...", this);
    
    gridLayout->addWidget(new QLabel("<b>CPU Load:</b>"), 10, 0);
    //lblCPULoad = new QLabel("HERE");
    //gridLayout->addWidget(lblCPULoad, 10, 1);

    barCPU = new QProgressBar(this);
    barCPU->setRange(0, 100);
    gridLayout->addWidget(barCPU, 10, 1); // Row 10, Column 1

    // CPU USAGE END

    // BELOW PROCESS SECTION WILL BE DISPLAYED AT TOP

    procTable = new QTableWidget(5, 3); // 5 rows, 3 columns

    procTable->setHorizontalHeaderLabels({"PID", "Name", "Memory (MB)"});
    procTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // Allow the 'Name' column (index 1) to take up all extra space
    procTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    procTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    procTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    // This makes the table headers and rows fit snugly
    procTable->verticalHeader()->setVisible(false); // Hide the 1, 2, 3 row numbers
    procTable->setFixedHeight(procTable->horizontalHeader()->height() + (procTable->rowHeight(0) * 4) + 2);

    // Making rows distict colors
    procTable->setAlternatingRowColors(true);
    procTable->setStyleSheet("color: white; alternate-background-color: #2a2a2a; background-color: #1e1e1e;");

    // Making table readonly  
    procTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    procTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // re-sorting table if a process suddenly jumps in mem
    procTable->setSortingEnabled(true);

    // Add it to your layout (assuming a QVBoxLayout holds your grid and now this table)
    mainLayout->addWidget(procTable);

    // PROCESS SECTION END

    mainLayout->addWidget(group);
    setCentralWidget(centralWidget);


    // Refresh every 2 seconds
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateStats);
    timer->start(2000);
    updateStats();
}

void MainWindow::updateStats() {
    SystemData data = SystemInfo::getSnapshot();
    lblCPU->setText(data.cpuModel);
    lblCores->setText(QString::number(data.coreCount));
    
    lblOS->setText(data.osName);
    lblKernel->setText(data.kernelVersion);
    lblUptime->setText(data.uptime);

    // Update RAM Bar

    int ramPercent = (data.ramTotal > 0) ? static_cast<int>((data.ramUsed / data.ramTotal) * 100) : 0;
    barRAM->setValue(ramPercent);
    lblRAM->setText(QString("%1 / %2 GB").arg(data.ramUsed, 0, 'f', 1).arg(data.ramTotal, 0, 'f', 1));

    if (ramPercent > 90) {
        barRAM->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }"); // Red
    } else {
        barRAM->setStyleSheet("QProgressBar::chunk { background-color: #3498db; }"); // Back to Blue
    }

    // Update Disk Bar

    int diskPercent = (data.diskTotal > 0) ? static_cast<int>((data.diskUsed / data.diskTotal) * 100) : 0;
    barDisk->setValue(diskPercent);
    lblDisk->setText(QString("%1 / %2 GB").arg(data.diskUsed, 0, 'f', 1).arg(data.diskTotal, 0, 'f', 1));

    if (diskPercent > 90) {
        barDisk->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }"); // Red
    } else {
        barDisk->setStyleSheet("QProgressBar::chunk { background-color: #2ecc71; }"); // Back to Green
    }

    // Change color to red if usage is high (> 90%)

    if (ramPercent > 90) barRAM->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }");
    if (diskPercent > 90) barDisk->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }");

    // Update CPU Temp

    lblTemp->setText(QString("%1 °C").arg(data.cpuTemp, 0, 'f', 1));

    // Color logic for temperature

    if (data.cpuTemp >= 80.0) {
        lblTemp->setStyleSheet("color: #e74c3c;"); // Critical Red
    } else if (data.cpuTemp >= 65.0) {
        lblTemp->setStyleSheet("color: #f39c12;"); // Warm Orange
    } else {
        lblTemp->setStyleSheet("color: #2ecc71;"); // Cool Green
    }

    // Update NETWORK

    lblNetDown->setText(QString("%1 KB/s").arg(data.downloadSpeed, 0, 'f', 1));
    lblNetUp->setText(QString("%1 KB/s").arg(data.uploadSpeed, 0, 'f', 1));

    // Update Table
    procTable->setRowCount(data.topProcesses.size()); 
    for (int i = 0; i < data.topProcesses.size(); ++i) {
        const auto &proc = data.topProcesses[i];
        
        // PID
        procTable->setItem(i, 0, new QTableWidgetItem(QString::number(proc.pid)));

        // Name
        procTable->setItem(i, 1, new QTableWidgetItem(proc.name));

        // Memory - formatted to 1 decimal place
        procTable->setItem(i, 2, new QTableWidgetItem(QString::number(proc.memoryMb, 'f', 1) + " MB"));

        // Memory Column with Alignment

        QTableWidgetItem *memItem = new QTableWidgetItem(QString::number(proc.memoryMb, 'f', 1) + " MB");
        memItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter); // Right-align the number

	// ANOMALY CHECK: Is this process using more than 1GB?
        if (proc.memoryMb > 1024.0) {
            memItem->setIcon(style()->standardIcon(QStyle::SP_MessageBoxWarning));
            memItem->setBackground(QColor(150, 50, 50)); // Dark red background
        }

        procTable->setItem(i, 2, memItem); // 'i' is valid here!

    }


    // Update CPU usage
    //

    int cpuPercent = (data.cpuLoad);
    barCPU->setValue(cpuPercent); // barCPU Value 
				 
    if (cpuPercent > 50){
        barCPU->setStyleSheet("QProgressBar::chunk { background-color: #e74c3c; }"); // Red
    } else {
        barCPU->setStyleSheet("QProgressBar::chunk { background-color: #3498db; }"); // Back to Blue
    }
				  
/*
    if (lblCPULoad) {
        lblCPULoad->setText(QString("CPU Load: %1%").arg(data.cpuLoad, 0, 'f', 1));
        
        // Bonus: Change text color to orange if load > 70%
        if (data.cpuLoad > 50.0) {
            lblCPULoad->setStyleSheet("color: #e67e22; font-weight: bold;");
        } else {
            lblCPULoad->setStyleSheet("color: white;");
        }
    }*/

}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    // This function refreshes the UI labels with new data
    void updateStats();

private:
    // UI Label pointers to update dynamically
    QLabel *lblCPU;
    QLabel *lblCPULoad;
    QLabel *lblCores;
    QLabel *lblRAM;
    QLabel *lblDisk;
    QLabel *lblOS;
    QLabel *lblKernel;
    QLabel *lblUptime;
    QLabel *lblTemp;
    QLabel *lblNetDown;
    QLabel *lblNetUp;
    QLabel *statusLabel;
    QProgressBar *barRAM;
    QProgressBar *barDisk;
    QProgressBar *barCPU;
    QTableWidget *procTable;
};

#endif // MAINWINDOW_H

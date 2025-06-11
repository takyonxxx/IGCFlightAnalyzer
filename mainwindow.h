#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QProgressBar>
#include <QTableWidget>
#include <QTextBrowser>
#include <QTabWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QTime>
#include <QTextStream>
#include <QFileInfo>
#include <QDateTime>
#include <QRegularExpression>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QPolygon>
#include <QMenu>
#include <QKeySequence>

#include "igcanalyzer.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openIGCFile();
    void analyzeThermals();
    void saveWaypoints();
    void exportReport();
    void showAbout();

    void onAnalysisProgress(int percentage);
    void onAnalysisComplete();
    void onThermalTableSelectionChanged();

private:
    // Core components
    IGCAnalyzer *analyzer;
    QString currentFileName;

    // UI Setup methods
    void setupMenuBar();
    void setupStatusBar();
    void setupUI();
    void applyProfessionalStyle();
    QIcon createParaglidingIcon();

    // Update methods
    void updateThermalTable();
    void updateThermalStats();
    void updateFlightInfo();
    void updateOverview();
    void updateXCAnalysis();
    void updateStatusBar();
    void showThermalDetails(const ThermalPoint &thermal);

    // Report generation
    void generateDetailedReport(const QString &fileName);

    // Utility methods
    QString getDistanceRating(double distance);
    QString getOLCRating(double points);
    QString getFlightCategory(double distance);

    // Menu and toolbar actions
    QAction *saveWaypointsMenuAction;
    QAction *exportReportMenuAction;
    QAction *analyzeThermalsMenuAction;
    QAction *calculateXCMenuAction;

    // Main UI components
    QWidget *centralWidget;
    QSplitter *mainSplitter;

    // Left panel - Controls and info
    QGroupBox *flightInfoGroup;
    QGroupBox *controlGroup;
    QTextBrowser *flightInfoBrowser;

    // Analysis parameters
    QDoubleSpinBox *climbRateSpinBox;
    QDoubleSpinBox *radiusSpinBox;
    QDoubleSpinBox *xcDistanceSpinBox;

    // Action buttons
    QPushButton *openButton;
    QPushButton *analyzeButton;
    QPushButton *saveButton;
    QPushButton *exportButton;
    QProgressBar *progressBar;

    // Right panel - Results tabs
    QTabWidget *tabWidget;

    // Flight Overview tab
    QWidget *overviewTab;
    QTextBrowser *overviewBrowser;

    // Thermal Analysis tab
    QWidget *thermalStatsWidget;
    QLabel *thermalCountLabel;
    QLabel *bestClimbLabel;
    QLabel *totalGainLabel;
    QTableWidget *thermalTable;
    QTextBrowser *thermalDetailsBrowser;

    // XC Analysis tab
    QWidget *xcTab;
    QTextBrowser *xcBrowser;

    // Status bar
    QLabel *flightStatusLabel;
    QLabel *thermalStatusLabel;
};

#endif // MAINWINDOW_H

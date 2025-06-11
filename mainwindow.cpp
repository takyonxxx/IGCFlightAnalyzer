#include "mainwindow.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QAction>
#include <QIcon>
#include <QPixmap>
#include <QPainter>
#include <QFont>
#include <QPolygon>
#include <QFileInfo>
#include <QDateTime>
#include <QRegularExpression>
#include <QKeySequence>
#include <QMenu>
#include <algorithm>

// Professional Paragliding IGC Analyzer - MainWindow Implementation
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), analyzer(new IGCAnalyzer(this)) {
    setWindowIcon(createParaglidingIcon());
    setupMenuBar();
    setupUI();
    setupStatusBar();
    applyProfessionalStyle();

    connect(analyzer, &IGCAnalyzer::analysisProgress, this, &MainWindow::onAnalysisProgress);
    connect(analyzer, &IGCAnalyzer::analysisComplete, this, &MainWindow::onAnalysisComplete);
}

MainWindow::~MainWindow() = default;

QIcon MainWindow::createParaglidingIcon() {
    QPixmap pixmap(32, 32);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw a simple paraglider wing
    painter.setPen(QPen(QColor(0, 100, 200), 2));
    painter.setBrush(QColor(50, 150, 255, 100));

    // Wing shape
    QPolygon wing;
    wing << QPoint(4, 20) << QPoint(8, 12) << QPoint(16, 8)
         << QPoint(24, 12) << QPoint(28, 20) << QPoint(16, 18);
    painter.drawPolygon(wing);

    // Lines
    painter.drawLine(8, 20, 16, 25);
    painter.drawLine(24, 20, 16, 25);

    // Pilot
    painter.setBrush(QColor(100, 100, 100));
    painter.drawEllipse(14, 24, 4, 4);

    return QIcon(pixmap);
}

void MainWindow::setupMenuBar() {
    // File Menu
    QMenu *fileMenu = menuBar()->addMenu("&File");

    QAction *openAction = new QAction("&Open IGC File...", this);
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Open IGC flight file for analysis");
    connect(openAction, &QAction::triggered, this, &MainWindow::openIGCFile);
    fileMenu->addAction(openAction);

    fileMenu->addSeparator();

    QAction *saveWaypointsAction = new QAction("Save &Waypoints...", this);
    saveWaypointsAction->setShortcut(QKeySequence::Save);
    saveWaypointsAction->setStatusTip("Save thermal waypoints to file");
    saveWaypointsAction->setEnabled(false);
    connect(saveWaypointsAction, &QAction::triggered, this, &MainWindow::saveWaypoints);
    fileMenu->addAction(saveWaypointsAction);

    QAction *exportReportAction = new QAction("Export &Report...", this);
    exportReportAction->setStatusTip("Export detailed flight analysis report");
    exportReportAction->setEnabled(false);
    connect(exportReportAction, &QAction::triggered, this, &MainWindow::exportReport);
    fileMenu->addAction(exportReportAction);

    fileMenu->addSeparator();

    QAction *exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    // Analysis Menu
    QMenu *analysisMenu = menuBar()->addMenu("&Analysis");

    QAction *analyzeThermalsAction = new QAction("Analyze &Thermals", this);
    analyzeThermalsAction->setShortcut(Qt::CTRL + Qt::Key_T);
    analyzeThermalsAction->setStatusTip("Analyze flight for thermal activity");
    analyzeThermalsAction->setEnabled(false);
    connect(analyzeThermalsAction, &QAction::triggered, this, &MainWindow::analyzeThermals);
    analysisMenu->addAction(analyzeThermalsAction);

    QAction *calculateXCAction = new QAction("Calculate &XC Distance", this);
    calculateXCAction->setShortcut(Qt::CTRL + Qt::Key_X);
    calculateXCAction->setStatusTip("Calculate cross-country distances and OLC scoring");
    calculateXCAction->setEnabled(false);
    analysisMenu->addAction(calculateXCAction);

    // Store actions for later reference
    saveWaypointsMenuAction = saveWaypointsAction;
    exportReportMenuAction = exportReportAction;
    analyzeThermalsMenuAction = analyzeThermalsAction;
    calculateXCMenuAction = calculateXCAction;

    // Help Menu
    QMenu *helpMenu = menuBar()->addMenu("&Help");

    QAction *aboutAction = new QAction("&About Türkay Biliyor Paragliding Analyzer", this);
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(aboutAction);
}

void MainWindow::setupStatusBar() {
    flightStatusLabel = new QLabel("No flight loaded");
    thermalStatusLabel = new QLabel("");

    statusBar()->addWidget(flightStatusLabel);
    statusBar()->addPermanentWidget(thermalStatusLabel);
}

void MainWindow::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    mainSplitter = new QSplitter(Qt::Horizontal, this);

    // Left panel - Flight Controls & Information
    QWidget *leftPanel = new QWidget();
    leftPanel->setMaximumWidth(350);
    leftPanel->setMinimumWidth(300);

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(10);

    // Flight Information Panel
    flightInfoGroup = new QGroupBox("Flight Information");
    flightInfoGroup->setObjectName("FlightInfoGroup");
    QVBoxLayout *flightInfoLayout = new QVBoxLayout(flightInfoGroup);

    flightInfoBrowser = new QTextBrowser();
    flightInfoBrowser->setMaximumHeight(200);
    flightInfoBrowser->setObjectName("FlightInfoBrowser");
    flightInfoLayout->addWidget(flightInfoBrowser);

    leftLayout->addWidget(flightInfoGroup);

    // Analysis Parameters Panel
    controlGroup = new QGroupBox("Analysis Parameters");
    controlGroup->setObjectName("AnalysisGroup");
    QGridLayout *controlLayout = new QGridLayout(controlGroup);
    controlLayout->setSpacing(8);

    // Thermal Detection Parameters
    QLabel *thermalLabel = new QLabel("THERMAL DETECTION");
    thermalLabel->setObjectName("SectionLabel");
    controlLayout->addWidget(thermalLabel, 0, 0, 1, 2);

    controlLayout->addWidget(new QLabel("Min Climb Rate:"), 1, 0);
    climbRateSpinBox = new QDoubleSpinBox();
    climbRateSpinBox->setRange(0.5, 10.0);
    climbRateSpinBox->setSingleStep(0.1);
    climbRateSpinBox->setValue(2.0);
    climbRateSpinBox->setDecimals(1);
    climbRateSpinBox->setSuffix(" m/s");
    climbRateSpinBox->setObjectName("ParameterSpinBox");
    controlLayout->addWidget(climbRateSpinBox, 1, 1);

    controlLayout->addWidget(new QLabel("Thermal Radius:"), 2, 0);
    radiusSpinBox = new QDoubleSpinBox();
    radiusSpinBox->setRange(50, 1000);
    radiusSpinBox->setSingleStep(50);
    radiusSpinBox->setValue(200);
    radiusSpinBox->setDecimals(0);
    radiusSpinBox->setSuffix(" m");
    radiusSpinBox->setObjectName("ParameterSpinBox");
    controlLayout->addWidget(radiusSpinBox, 2, 1);

    // XC Parameters
    QLabel *xcLabel = new QLabel("XC ANALYSIS");
    xcLabel->setObjectName("SectionLabel");
    controlLayout->addWidget(xcLabel, 3, 0, 1, 2);

    controlLayout->addWidget(new QLabel("Min XC Distance:"), 4, 0);
    xcDistanceSpinBox = new QDoubleSpinBox();
    xcDistanceSpinBox->setRange(5.0, 500.0);
    xcDistanceSpinBox->setSingleStep(5.0);
    xcDistanceSpinBox->setValue(25.0);
    xcDistanceSpinBox->setDecimals(1);
    xcDistanceSpinBox->setSuffix(" km");
    xcDistanceSpinBox->setObjectName("ParameterSpinBox");
    controlLayout->addWidget(xcDistanceSpinBox, 4, 1);

    // Action Buttons
    QWidget *buttonPanel = new QWidget();
    QVBoxLayout *buttonLayout = new QVBoxLayout(buttonPanel);
    buttonLayout->setSpacing(8);

    openButton = new QPushButton("📁 Open IGC File");
    openButton->setObjectName("PrimaryButton");
    openButton->setMinimumHeight(40);
    buttonLayout->addWidget(openButton);

    analyzeButton = new QPushButton("🌡️ Analyze Flight");
    analyzeButton->setObjectName("ActionButton");
    analyzeButton->setMinimumHeight(35);
    analyzeButton->setEnabled(false);
    buttonLayout->addWidget(analyzeButton);

    saveButton = new QPushButton("💾 Save Waypoints");
    saveButton->setObjectName("ActionButton");
    saveButton->setMinimumHeight(35);
    saveButton->setEnabled(false);
    buttonLayout->addWidget(saveButton);

    exportButton = new QPushButton("📄 Export Report");
    exportButton->setObjectName("ActionButton");
    exportButton->setMinimumHeight(35);
    exportButton->setEnabled(false);
    buttonLayout->addWidget(exportButton);

    controlLayout->addWidget(buttonPanel, 5, 0, 1, 2);

    progressBar = new QProgressBar();
    progressBar->setVisible(false);
    progressBar->setObjectName("AnalysisProgress");
    controlLayout->addWidget(progressBar, 6, 0, 1, 2);

    leftLayout->addWidget(controlGroup);
    leftLayout->addStretch();

    // Right panel - Analysis Results
    tabWidget = new QTabWidget();
    tabWidget->setObjectName("ResultsTabs");

    // Flight Overview Tab
    overviewTab = new QWidget();
    QVBoxLayout *overviewLayout = new QVBoxLayout(overviewTab);

    overviewBrowser = new QTextBrowser();
    overviewBrowser->setObjectName("OverviewBrowser");
    overviewLayout->addWidget(overviewBrowser);

    tabWidget->addTab(overviewTab, "📊 Flight Overview");

    // Thermal Analysis Tab
    QWidget *thermalTab = new QWidget();
    QVBoxLayout *thermalLayout = new QVBoxLayout(thermalTab);

    // Thermal statistics
    thermalStatsWidget = new QWidget();
    thermalStatsWidget->setObjectName("ThermalStats");
    QHBoxLayout *statsLayout = new QHBoxLayout(thermalStatsWidget);

    thermalCountLabel = new QLabel("Thermals: 0");
    thermalCountLabel->setObjectName("StatLabel");
    statsLayout->addWidget(thermalCountLabel);

    bestClimbLabel = new QLabel("Best: 0.0 m/s");
    bestClimbLabel->setObjectName("StatLabel");
    statsLayout->addWidget(bestClimbLabel);

    totalGainLabel = new QLabel("Total Gain: 0 m");
    totalGainLabel->setObjectName("StatLabel");
    statsLayout->addWidget(totalGainLabel);

    statsLayout->addStretch();
    thermalLayout->addWidget(thermalStatsWidget);

    thermalTable = new QTableWidget();
    thermalTable->setColumnCount(8);
    QStringList headers = {"Name", "Time", "Duration", "Avg Climb", "Max Climb", "Alt Gain", "Radius", "Quality"};
    thermalTable->setHorizontalHeaderLabels(headers);
    thermalTable->horizontalHeader()->setStretchLastSection(true);
    thermalTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    thermalTable->setAlternatingRowColors(true);
    thermalTable->setObjectName("ThermalTable");
    thermalLayout->addWidget(thermalTable);

    thermalDetailsBrowser = new QTextBrowser();
    thermalDetailsBrowser->setMaximumHeight(150);
    thermalDetailsBrowser->setObjectName("ThermalDetails");
    thermalLayout->addWidget(thermalDetailsBrowser);

    tabWidget->addTab(thermalTab, "🌪️ Thermal Analysis");

    // XC Analysis Tab
    xcTab = new QWidget();
    QVBoxLayout *xcLayout = new QVBoxLayout(xcTab);

    xcBrowser = new QTextBrowser();
    xcBrowser->setObjectName("XCBrowser");
    xcLayout->addWidget(xcBrowser);

    tabWidget->addTab(xcTab, "🏁 XC Performance");

    // Add panels to splitter
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(tabWidget);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);
    mainSplitter->setSizes({300, 700});

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->addWidget(mainSplitter);

    // Connect signals
    connect(openButton, &QPushButton::clicked, this, &MainWindow::openIGCFile);
    connect(analyzeButton, &QPushButton::clicked, this, &MainWindow::analyzeThermals);
    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveWaypoints);
    connect(exportButton, &QPushButton::clicked, this, &MainWindow::exportReport);
    connect(thermalTable, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::onThermalTableSelectionChanged);

    setWindowTitle("Türkay Biliyor Paragliding - IGC Flight Analyzer v1.0");
    resize(1200, 800);
}

void MainWindow::applyProfessionalStyle() {
   setStyleSheet("");
}

void MainWindow::openIGCFile() {
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open IGC Flight File - Paragliding Analyzer",
        QStandardPaths::writableLocation(QStandardPaths::DesktopLocation),
        "IGC Flight Files (*.igc);;All Files (*)"
        );

    if (!fileName.isEmpty()) {
        if (analyzer->loadIGCFile(fileName)) {
            currentFileName = fileName;

            // Update UI state
            analyzeButton->setEnabled(true);
            analyzeThermalsMenuAction->setEnabled(true);
            calculateXCMenuAction->setEnabled(true);

            updateFlightInfo();
            updateOverview();
            updateStatusBar();

            QFileInfo fileInfo(fileName);
            QMessageBox::information(this, "Flight Loaded Successfully",
                                     QString("IGC flight file loaded successfully!\n\n"
                                             "File: %1\n"
                                             "Data Points: %2\n"
                                             "Ready for thermal analysis.")
                                         .arg(fileInfo.fileName())
                                         .arg(analyzer->getFlightData().size()));
        } else {
            QMessageBox::critical(this, "Error Loading Flight",
                                  "Failed to load IGC file!\n\n"
                                  "Please ensure the file is a valid IGC format.");
        }
    }
}

void MainWindow::analyzeThermals() {
    if (analyzer->getFlightData().empty()) {
        QMessageBox::warning(this, "No Flight Data",
                            "No flight data loaded!\n\nPlease open an IGC file first.");
        return;
    }

    // Update UI state for analysis
    analyzeButton->setEnabled(false);
    progressBar->setVisible(true);
    progressBar->setValue(0);

    statusBar()->showMessage("Analyzing flight for thermal activity...");

    double minClimbRate = climbRateSpinBox->value();
    double thermalRadius = radiusSpinBox->value();

    // Run analysis
    analyzer->analyzeForThermals(minClimbRate, thermalRadius);
}

void MainWindow::saveWaypoints() {
    if (analyzer->getThermals().empty()) {
        QMessageBox::warning(this, "No Thermals Found",
                            "No thermals found to save!\n\nPlease analyze the flight first.");
        return;
    }

    QFileInfo fileInfo(currentFileName);
    QString defaultName = QString("%1_thermals.wpt").arg(fileInfo.baseName());

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save Thermal Waypoints - Paragliding",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultName,
        "Waypoint Files (*.wpt);;Cup Files (*.cup);;All Files (*)"
        );

    if (!fileName.isEmpty()) {
        analyzer->generateWaypointFile(fileName);
        QMessageBox::information(this, "Waypoints Saved",
                                 QString("Thermal waypoints saved successfully!\n\n"
                                         "File: %1\n"
                                         "Waypoints: %2 thermals + takeoff + landing")
                                     .arg(QFileInfo(fileName).fileName())
                                     .arg(analyzer->getThermals().size()));
    }
}

void MainWindow::exportReport() {
    if (analyzer->getFlightData().empty()) {
        QMessageBox::warning(this, "No Flight Data", "No flight data to export!");
        return;
    }

    QFileInfo fileInfo(currentFileName);
    QString defaultName = QString("%1_flight_report.html").arg(fileInfo.baseName());

    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Export Flight Report - Paragliding",
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + defaultName,
        "HTML Report (*.html);;Text Report (*.txt);;All Files (*)"
        );

    if (!fileName.isEmpty()) {
        generateDetailedReport(fileName);
    }
}

void MainWindow::generateDetailedReport(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not create report file!");
        return;
    }

    QTextStream out(&file);

    if (fileName.endsWith(".html")) {
        // HTML Report
        out << "<!DOCTYPE html>\n<html>\n<head>\n";
        out << "<title>Türkay Biliyor Paragliding - Flight Analysis Report</title>\n";
        out << "<style>\n";
        out << "body { font-family: Arial, sans-serif; margin: 20px; }\n";
        out << ".header { background: #3182ce; color: white; padding: 20px; border-radius: 8px; }\n";
        out << ".section { margin: 20px 0; padding: 15px; border: 1px solid #e2e8f0; border-radius: 6px; }\n";
        out << ".thermal { background: #f7fafc; margin: 10px 0; padding: 10px; border-radius: 4px; }\n";
        out << "table { width: 100%; border-collapse: collapse; }\n";
        out << "th, td { border: 1px solid #e2e8f0; padding: 8px; text-align: left; }\n";
        out << "th { background: #edf2f7; }\n";
        out << "</style>\n</head>\n<body>\n";

        out << "<div class='header'>\n";
        out << "<h1>🪂 Türkay Biliyor Paragliding - Flight Analysis Report</h1>\n";
        out << "<p>Generated: " << QDateTime::currentDateTime().toString() << "</p>\n";
        out << "</div>\n";

        out << "<div class='section'>\n";
        out << analyzer->getFlightInfo();
        out << "</div>\n";

        if (!analyzer->getThermals().empty()) {
            out << "<div class='section'>\n";
            out << analyzer->getThermalSummary();
            out << "</div>\n";
        }

        out << "</body>\n</html>\n";
    } else {
        // Text Report
        out << "Türkay Biliyor PARAGLIDING - FLIGHT ANALYSIS REPORT\n";
        out << "========================================\n\n";
        out << "Generated: " << QDateTime::currentDateTime().toString() << "\n\n";

        // Convert HTML to plain text (simplified)
        QString flightInfo = analyzer->getFlightInfo();
        flightInfo.remove(QRegularExpression("<[^>]*>"));
        out << flightInfo << "\n\n";

        if (!analyzer->getThermals().empty()) {
            QString thermalInfo = analyzer->getThermalSummary();
            thermalInfo.remove(QRegularExpression("<[^>]*>"));
            out << thermalInfo << "\n";
        }
    }

    QMessageBox::information(this, "Report Exported",
                             QString("Flight analysis report exported successfully!\n\nFile: %1")
                                 .arg(QFileInfo(fileName).fileName()));
}

void MainWindow::showAbout() {
    QMessageBox::about(this, "About Türkay Biliyor Paragliding Analyzer",
                       "<h3>Türkay Biliyor Paragliding - IGC Flight Analyzer v1.0</h3>"
                       "<p><b>Professional paragliding flight analysis software</b></p>"
                       "<p>Features:</p>"
                       "<ul>"
                       "<li>🌪️ Advanced thermal detection and analysis</li>"
                       "<li>🏁 Cross-country distance optimization</li>"
                       "<li>📊 Comprehensive flight statistics</li>"
                       "<li>💾 Waypoint generation for flight planning</li>"
                       "<li>📄 Detailed flight reports</li>"
                       "</ul>"
                       "<p><b>Designed for paragliding pilots and instructors</b></p>"
                       "<p>© 2025 Türkay Biliyor Paragliding. All rights reserved.</p>");
}

void MainWindow::onAnalysisProgress(int percentage) {
    progressBar->setValue(percentage);
    statusBar()->showMessage(QString("Analyzing flight... %1%").arg(percentage));
}

void MainWindow::onAnalysisComplete() {
    progressBar->setVisible(false);
    analyzeButton->setEnabled(true);

    updateThermalTable();
    updateThermalStats();
    updateXCAnalysis();
    updateStatusBar();

    if (!analyzer->getThermals().empty()) {
        saveButton->setEnabled(true);
        exportButton->setEnabled(true);
        saveWaypointsMenuAction->setEnabled(true);
        exportReportMenuAction->setEnabled(true);

        // Switch to thermal analysis tab
        tabWidget->setCurrentIndex(1);

        // Calculate statistics for the message
        const auto &thermals = analyzer->getThermals();
        double bestClimb = 0.0;
        double totalGain = 0.0;

        if (!thermals.empty()) {
            // Find best climb rate
            for (const auto &thermal : thermals) {
                bestClimb = std::max(bestClimb, thermal.maxClimbRate);
                totalGain += thermal.totalAltitudeGain;
            }
        }

        QMessageBox::information(this, "Thermal Analysis Complete",
                                 QString("🌪️ Thermal analysis completed successfully!\n\n"
                                         "Results:\n"
                                         "• %1 thermals detected\n"
                                         "• Best climb rate: %2 m/s\n"
                                         "• Total altitude gained: %3 m\n\n"
                                         "Check the Thermal Analysis tab for detailed results.")
                                     .arg(thermals.size())
                                     .arg(bestClimb, 0, 'f', 1)
                                     .arg(totalGain, 0, 'f', 0));

        statusBar()->showMessage(QString("Analysis complete - %1 thermals found").arg(thermals.size()), 5000);
    } else {
        statusBar()->showMessage("Analysis complete - No thermals found with current criteria", 5000);
        QMessageBox::information(this, "Analysis Complete",
                                 "No thermals found with the current criteria.\n\n"
                                 "Suggestions:\n"
                                 "• Try lowering the minimum climb rate\n"
                                 "• Check if the flight includes thermal activity\n"
                                 "• Verify the IGC file contains valid GPS data");
    }
}

void MainWindow::updateThermalTable() {
    const auto &thermals = analyzer->getThermals();
    thermalTable->setRowCount(thermals.size());

    for (size_t i = 0; i < thermals.size(); i++) {
        const auto &thermal = thermals[i];

        thermalTable->setItem(i, 0, new QTableWidgetItem(thermal.name));
        thermalTable->setItem(i, 1, new QTableWidgetItem(thermal.startTime.toString("hh:mm:ss")));

        auto duration = thermal.startTime.secsTo(thermal.endTime);
        thermalTable->setItem(i, 2, new QTableWidgetItem(QTime(0,0).addSecs(duration).toString("mm:ss")));

        thermalTable->setItem(i, 3, new QTableWidgetItem(QString::number(thermal.averageClimbRate, 'f', 2)));
        thermalTable->setItem(i, 4, new QTableWidgetItem(QString::number(thermal.maxClimbRate, 'f', 2)));
        thermalTable->setItem(i, 5, new QTableWidgetItem(QString::number(thermal.totalAltitudeGain, 'f', 0)));
        thermalTable->setItem(i, 6, new QTableWidgetItem(QString::number(thermal.radius, 'f', 0)));

        // Quality rating
        QString quality;
        if (thermal.strength >= 5) quality = "⭐⭐⭐⭐⭐ Excellent";
        else if (thermal.strength >= 4) quality = "⭐⭐⭐⭐ Very Good";
        else if (thermal.strength >= 3) quality = "⭐⭐⭐ Good";
        else if (thermal.strength >= 2) quality = "⭐⭐ Fair";
        else quality = "⭐ Weak";

        thermalTable->setItem(i, 7, new QTableWidgetItem(quality));

        // Color coding based on thermal strength
        QColor rowColor;
        if (thermal.strength >= 5) rowColor = QColor(34, 197, 94, 40);      // Green - Excellent
        else if (thermal.strength >= 4) rowColor = QColor(101, 163, 13, 40); // Light Green - Very Good
        else if (thermal.strength >= 3) rowColor = QColor(234, 179, 8, 40);  // Yellow - Good
        else if (thermal.strength >= 2) rowColor = QColor(249, 115, 22, 40); // Orange - Fair
        else rowColor = QColor(239, 68, 68, 40);                            // Red - Weak

        for (int j = 0; j < 8; j++) {
            thermalTable->item(i, j)->setBackground(rowColor);

            // Center align numeric columns
            if (j >= 3 && j <= 6) {
                thermalTable->item(i, j)->setTextAlignment(Qt::AlignCenter);
            }
        }
    }

    thermalTable->resizeColumnsToContents();
    thermalTable->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::updateThermalStats() {
    const auto &thermals = analyzer->getThermals();

    if (thermals.empty()) {
        thermalCountLabel->setText("Thermals: 0");
        bestClimbLabel->setText("Best: 0.0 m/s");
        totalGainLabel->setText("Total Gain: 0 m");
        return;
    }

    // Calculate statistics
    double bestClimb = 0.0;
    double totalGain = 0.0;

    for (const auto &thermal : thermals) {
        bestClimb = std::max(bestClimb, thermal.maxClimbRate);
        totalGain += thermal.totalAltitudeGain;
    }

    thermalCountLabel->setText(QString("Thermals: %1").arg(thermals.size()));
    bestClimbLabel->setText(QString("Best: %1 m/s").arg(bestClimb, 0, 'f', 1));
    totalGainLabel->setText(QString("Total Gain: %1 m").arg(totalGain, 0, 'f', 0));
}

void MainWindow::updateFlightInfo() {
    flightInfoBrowser->setHtml(analyzer->getFlightInfo());
}

void MainWindow::updateOverview() {
    QString overview;
    QTextStream stream(&overview);

    stream << "<h3>🪂 Flight Overview</h3>";
    stream << analyzer->getFlightInfo();

    if (!analyzer->getFlightData().empty()) {
        stream << "<br><h4>📈 Performance Summary</h4>";
        stream << "<table border='1' cellpadding='5' cellspacing='0' style='border-collapse: collapse; width: 100%;'>";
        stream << "<tr style='background-color: #f0f0f0;'>";
        stream << "<th>Metric</th><th>Value</th><th>Performance</th>";
        stream << "</tr>";

        // XC Speed rating
        double xcSpeed = analyzer->getXCSpeed();
        QString xcRating;
        if (xcSpeed >= 40) xcRating = "🏆 Excellent";
        else if (xcSpeed >= 30) xcRating = "🥇 Very Good";
        else if (xcSpeed >= 20) xcRating = "🥈 Good";
        else if (xcSpeed >= 15) xcRating = "🥉 Fair";
        else xcRating = "📈 Learning";

        stream << "<tr><td>XC Speed</td><td>" << QString::number(xcSpeed, 'f', 1) << " km/h</td><td>" << xcRating << "</td></tr>";

        // Max vario rating
        double maxVario = analyzer->getMaxVario();
        QString varioRating;
        if (maxVario >= 6.0) varioRating = "🌪️ Exceptional";
        else if (maxVario >= 4.0) varioRating = "💨 Strong";
        else if (maxVario >= 2.5) varioRating = "🌤️ Good";
        else if (maxVario >= 1.5) varioRating = "⛅ Moderate";
        else varioRating = "🌫️ Weak";

        stream << "<tr><td>Max Vario</td><td>" << QString::number(maxVario, 'f', 1) << " m/s</td><td>" << varioRating << "</td></tr>";

        // Distance rating
        double distance = analyzer->getStraightLineDistance();
        QString distRating;
        if (distance >= 200) distRating = "🚀 Epic";
        else if (distance >= 100) distRating = "✈️ Excellent";
        else if (distance >= 50) distRating = "🎯 Good";
        else if (distance >= 25) distRating = "📍 Decent";
        else distRating = "🏠 Local";

        stream << "<tr><td>Distance</td><td>" << QString::number(distance, 'f', 1) << " km</td><td>" << distRating << "</td></tr>";

        stream << "</table>";
    }

    overviewBrowser->setHtml(overview);
}

void MainWindow::updateXCAnalysis() {
    QString xcAnalysis;
    QTextStream stream(&xcAnalysis);

    stream << "<h3>🏁 Cross-Country Performance Analysis</h3>";

    if (analyzer->getFlightData().empty()) {
        stream << "<p>No flight data available for XC analysis.</p>";
        xcBrowser->setHtml(xcAnalysis);
        return;
    }

    // Basic XC Statistics
    stream << "<h4>📊 Distance Analysis</h4>";
    stream << "<table border='1' cellpadding='8' cellspacing='0' style='border-collapse: collapse; width: 100%;'>";
    stream << "<tr style='background-color: #e2e8f0;'>";
    stream << "<th>Distance Type</th><th>Value</th><th>Speed</th><th>Rating</th>";
    stream << "</tr>";

    double duration = analyzer->getFlightDurationSeconds() / 3600.0;

    // Straight line distance
    double straightDist = analyzer->getStraightLineDistance();
    double straightSpeed = duration > 0 ? straightDist / duration : 0;
    stream << "<tr><td>Straight Line</td><td>" << QString::number(straightDist, 'f', 1) << " km</td>";
    stream << "<td>" << QString::number(straightSpeed, 'f', 1) << " km/h</td>";
    stream << "<td>" << getDistanceRating(straightDist) << "</td></tr>";

    // Maximum distance
    double maxDist = analyzer->calculateMaximumDistance();
    double maxSpeed = duration > 0 ? maxDist / duration : 0;
    stream << "<tr><td>Maximum Distance</td><td>" << QString::number(maxDist, 'f', 1) << " km</td>";
    stream << "<td>" << QString::number(maxSpeed, 'f', 1) << " km/h</td>";
    stream << "<td>" << getDistanceRating(maxDist) << "</td></tr>";

    // OLC distance
    double olcDist = analyzer->getOLCDistance();
    double olcSpeed = duration > 0 ? olcDist / duration : 0;
    double olcPoints = analyzer->calculateOLCPoints();
    stream << "<tr><td>OLC Optimized</td><td>" << QString::number(olcDist, 'f', 1) << " km</td>";
    stream << "<td>" << QString::number(olcSpeed, 'f', 1) << " km/h</td>";
    stream << "<td>" << getOLCRating(olcPoints) << "</td></tr>";

    stream << "</table>";

    // OLC Scoring
    stream << "<h4>🏆 Competition Scoring</h4>";
    stream << "<p><b>OLC Points:</b> " << QString::number(olcPoints, 'f', 1) << " points</p>";
    stream << "<p><b>Flight Category:</b> " << getFlightCategory(straightDist) << "</p>";

    // Performance recommendations
    stream << "<h4>💡 Performance Insights</h4>";
    stream << "<ul>";

    if (straightSpeed < 25) {
        stream << "<li>🎯 <b>Speed Improvement:</b> Focus on finding stronger thermals and optimizing glide paths</li>";
    }

    if (olcDist - straightDist > 20) {
        stream << "<li>📈 <b>Route Optimization:</b> Good XC strategy with effective use of multiple waypoints</li>";
    } else {
        stream << "<li>📍 <b>Route Planning:</b> Consider exploring wider areas to maximize XC distance</li>";
    }

    const auto &thermals = analyzer->getThermals();
    if (!thermals.empty()) {
        int strongThermals = std::count_if(thermals.begin(), thermals.end(),
                                         [](const ThermalPoint &t) { return t.maxClimbRate >= 4.0; });
        if (strongThermals >= 5) {
            stream << "<li>⭐ <b>Thermal Skills:</b> Excellent thermal finding and centering ability</li>";
        } else {
            stream << "<li>🌪️ <b>Thermal Skills:</b> Practice thermal centering to maximize climb rates</li>";
        }
    }

    stream << "</ul>";

    xcBrowser->setHtml(xcAnalysis);
}

QString MainWindow::getDistanceRating(double distance) {
    if (distance >= 200) return "🚀 Epic XC";
    else if (distance >= 150) return "✈️ Excellent";
    else if (distance >= 100) return "🎯 Very Good";
    else if (distance >= 50) return "📍 Good";
    else if (distance >= 25) return "🏃 Decent";
    else return "🏠 Local";
}

QString MainWindow::getOLCRating(double points) {
    if (points >= 500) return "🏆 Elite";
    else if (points >= 300) return "🥇 Expert";
    else if (points >= 200) return "🥈 Advanced";
    else if (points >= 100) return "🥉 Intermediate";
    else if (points >= 50) return "📈 Developing";
    else return "🌱 Beginner";
}

QString MainWindow::getFlightCategory(double distance) {
    if (distance >= 500) return "Epic Adventure (500+ km)";
    else if (distance >= 300) return "Long Distance XC (300+ km)";
    else if (distance >= 200) return "Major XC Flight (200+ km)";
    else if (distance >= 100) return "Significant XC (100+ km)";
    else if (distance >= 50) return "Standard XC (50+ km)";
    else if (distance >= 25) return "Short XC (25+ km)";
    else return "Local Flight (< 25 km)";
}

void MainWindow::updateStatusBar() {
    if (analyzer->getFlightData().empty()) {
        flightStatusLabel->setText("No flight loaded");
        thermalStatusLabel->setText("");
    } else {
        QString duration = QTime(0,0).addSecs(analyzer->getFlightDurationSeconds()).toString("hh:mm:ss");
        flightStatusLabel->setText(QString("Flight loaded: %1 points, %2 duration")
                                  .arg(analyzer->getFlightData().size())
                                  .arg(duration));

        if (!analyzer->getThermals().empty()) {
            thermalStatusLabel->setText(QString("%1 thermals analyzed")
                                       .arg(analyzer->getThermals().size()));
        } else {
            thermalStatusLabel->setText("Ready for analysis");
        }
    }
}

void MainWindow::onThermalTableSelectionChanged() {
    auto selection = thermalTable->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        int row = selection.first().row();
        const auto &thermals = analyzer->getThermals();
        if (row < (int)thermals.size()) {
            showThermalDetails(thermals[row]);
        }
    }
}

void MainWindow::showThermalDetails(const ThermalPoint &thermal) {
    QString details;
    QTextStream stream(&details);

    stream << "<div style='background-color: #f7fafc; padding: 15px; border-radius: 8px;'>";
    stream << "<h4 style='color: #2d5016; margin-top: 0;'>🌪️ " << thermal.name << "</h4>";

    stream << "<table style='width: 100%; border-collapse: collapse;'>";
    stream << "<tr><td style='padding: 4px; font-weight: bold; width: 40%;'>⏰ Start Time:</td>";
    stream << "<td style='padding: 4px;'>" << thermal.startTime.toString("hh:mm:ss") << "</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>⏱️ End Time:</td>";
    stream << "<td style='padding: 4px;'>" << thermal.endTime.toString("hh:mm:ss") << "</td></tr>";

    auto duration = thermal.startTime.secsTo(thermal.endTime);
    stream << "<tr><td style='padding: 4px; font-weight: bold;'>⏲️ Duration:</td>";
    stream << "<td style='padding: 4px;'>" << QTime(0,0).addSecs(duration).toString("mm:ss") << "</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>📍 Latitude:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.centerLatitude, 'f', 6) << "°</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>📍 Longitude:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.centerLongitude, 'f', 6) << "°</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>📈 Average Climb:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.averageClimbRate, 'f', 2) << " m/s</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>🚀 Maximum Climb:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.maxClimbRate, 'f', 2) << " m/s</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>⬆️ Altitude Gain:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.totalAltitudeGain, 'f', 0) << " m</td></tr>";

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>📏 Thermal Radius:</td>";
    stream << "<td style='padding: 4px;'>" << QString::number(thermal.radius, 'f', 0) << " m</td></tr>";

    QString strengthText, strengthEmoji;
    if (thermal.strength >= 5) { strengthText = "Excellent"; strengthEmoji = "⭐⭐⭐⭐⭐"; }
    else if (thermal.strength >= 4) { strengthText = "Very Good"; strengthEmoji = "⭐⭐⭐⭐"; }
    else if (thermal.strength >= 3) { strengthText = "Good"; strengthEmoji = "⭐⭐⭐"; }
    else if (thermal.strength >= 2) { strengthText = "Fair"; strengthEmoji = "⭐⭐"; }
    else { strengthText = "Weak"; strengthEmoji = "⭐"; }

    stream << "<tr><td style='padding: 4px; font-weight: bold;'>🏆 Quality:</td>";
    stream << "<td style='padding: 4px;'>" << strengthEmoji << " " << strengthText << "</td></tr>";

    stream << "</table>";
    stream << "</div>";

    thermalDetailsBrowser->setHtml(details);
}

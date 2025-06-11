#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QFont>

void setDarkFlightTheme(QApplication *app) {
    // Set dark fusion style
    app->setStyle(QStyleFactory::create("Fusion"));

    // Create dark palette
    QPalette darkPalette;

    // Window colors - Deep dark aviation theme
    darkPalette.setColor(QPalette::Window, QColor(25, 25, 30));             // Very dark background
    darkPalette.setColor(QPalette::WindowText, QColor(220, 220, 225));      // Light text
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 35));               // Input backgrounds
    darkPalette.setColor(QPalette::AlternateBase, QColor(35, 35, 42));      // Alternate rows

    // Button colors - Dark aviation theme
    darkPalette.setColor(QPalette::Button, QColor(40, 40, 48));             // Button background
    darkPalette.setColor(QPalette::ButtonText, QColor(220, 220, 225));      // Button text

    // Selection colors - Aviation orange/blue
    darkPalette.setColor(QPalette::Highlight, QColor(255, 140, 0));         // Orange selection
    darkPalette.setColor(QPalette::HighlightedText, QColor(0, 0, 0));       // Black selected text

    // Text colors
    darkPalette.setColor(QPalette::Text, QColor(210, 210, 215));            // Input text
    darkPalette.setColor(QPalette::BrightText, QColor(255, 255, 255));      // Bright text
    darkPalette.setColor(QPalette::PlaceholderText, QColor(120, 120, 130)); // Placeholder text

    // Special elements
    darkPalette.setColor(QPalette::Link, QColor(100, 180, 255));            // Links
    darkPalette.setColor(QPalette::LinkVisited, QColor(180, 140, 255));     // Visited links

    // Disabled colors
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(80, 80, 90));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(80, 80, 90));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(80, 80, 90));

    app->setPalette(darkPalette);

    // Set professional aviation font
    QFont font("Segoe UI", 9);
    font.setStyleHint(QFont::SansSerif);
    app->setFont(font);

    // Set application-wide REAL dark style sheet
    app->setStyleSheet(R"(
        /* Professional Deep Dark Aviation Theme */

        QMainWindow {
            background-color: #191920;
            color: #dcdce0;
        }

        /* Menu Bar - Deep Dark Cockpit */
        QMenuBar {
            background-color: #0f0f12;
            border-bottom: 2px solid #ff8c00;
            color: #dcdce0;
            padding: 4px;
        }

        QMenuBar::item {
            background-color: transparent;
            padding: 8px 14px;
            border-radius: 4px;
            color: #dcdce0;
        }

        QMenuBar::item:selected {
            background-color: #ff8c00;
            color: #000000;
            font-weight: bold;
        }

        QMenu {
            background-color: #1a1a20;
            border: 2px solid #404050;
            border-radius: 6px;
            padding: 6px;
            color: #dcdce0;
        }

        QMenu::item {
            padding: 10px 18px;
            border-radius: 4px;
            color: #dcdce0;
        }

        QMenu::item:selected {
            background-color: #ff8c00;
            color: #000000;
        }

        /* Tool Bar - Dark Flight Panel */
        QToolBar {
            background-color: #0f0f12;
            border: 2px solid #303040;
            border-radius: 8px;
            padding: 6px;
            spacing: 10px;
        }

        QToolButton {
            background-color: #2a2a35;
            color: #dcdce0;
            border: 2px solid #404050;
            border-radius: 6px;
            padding: 10px 14px;
            font-weight: bold;
        }

        QToolButton:hover {
            background-color: #ff8c00;
            border-color: #ffaa33;
            color: #000000;
        }

        QToolButton:pressed {
            background-color: #cc7000;
            color: #000000;
        }

        /* Group Box - Dark Instrument Panels */
        QGroupBox {
            font-weight: bold;
            border: 3px solid #404050;
            border-radius: 10px;
            margin: 12px 0px;
            padding-top: 18px;
            background-color: #1a1a20;
            color: #dcdce0;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 10px 0 10px;
            color: #ff8c00;
            font-size: 13px;
            font-weight: bold;
        }

        /* Buttons - Dark Aviation Controls */
        QPushButton {
            background-color: #2a2a35;
            color: #dcdce0;
            border: 2px solid #404050;
            border-radius: 8px;
            padding: 10px 18px;
            font-weight: bold;
            min-height: 25px;
        }

        QPushButton:hover {
            background-color: #ff8c00;
            border-color: #ffaa33;
            color: #000000;
        }

        QPushButton:pressed {
            background-color: #cc7000;
            border-color: #ff8c00;
            color: #000000;
        }

        QPushButton:disabled {
            background-color: #151518;
            border-color: #252530;
            color: #505060;
        }

        /* Primary Action Buttons */
        QPushButton#PrimaryButton {
            background-color: #ff8c00;
            border-color: #ffaa33;
            color: #000000;
            font-size: 15px;
            font-weight: bold;
        }

        QPushButton#PrimaryButton:hover {
            background-color: #ffaa33;
            border-color: #ffcc66;
            color: #000000;
        }

        QPushButton#ActionButton {
            background-color: #00aa44;
            border-color: #00cc55;
            color: #ffffff;
            font-weight: bold;
        }

        QPushButton#ActionButton:hover {
            background-color: #00cc55;
            border-color: #00ee66;
            color: #ffffff;
        }

        QPushButton#ActionButton:disabled {
            background-color: #151518;
            border-color: #252530;
            color: #505060;
        }

        /* Input Controls - Dark Avionics */
        QDoubleSpinBox, QSpinBox {
            background-color: #2a2a35;
            border: 2px solid #404050;
            border-radius: 6px;
            padding: 8px;
            color: #dcdce0;
            selection-background-color: #ff8c00;
            selection-color: #000000;
        }

        QDoubleSpinBox:focus, QSpinBox:focus {
            border-color: #ff8c00;
            background-color: #1a1a20;
        }

        /* Tables - Dark Flight Data */
        QTableWidget {
            background-color: #1a1a20;
            alternate-background-color: #151518;
            selection-background-color: #ff8c00;
            selection-color: #000000;
            gridline-color: #404050;
            border: 2px solid #404050;
            border-radius: 8px;
            color: #dcdce0;
        }

        QTableWidget::item {
            padding: 10px;
            border-bottom: 1px solid #2a2a35;
        }

        QTableWidget::item:selected {
            background-color: #ff8c00;
            color: #000000;
        }

        QHeaderView::section {
            background-color: #0f0f12;
            color: #ff8c00;
            padding: 10px;
            border: 1px solid #404050;
            font-weight: bold;
            font-size: 11px;
        }

        /* Text Browser - Dark Flight Logs */
        QTextBrowser {
            background-color: #1a1a20;
            border: 2px solid #404050;
            border-radius: 8px;
            padding: 15px;
            color: #dcdce0;
            selection-background-color: #ff8c00;
            selection-color: #000000;
        }

        /* Tab Widget - Dark MFD Style */
        QTabWidget::pane {
            border: 3px solid #404050;
            border-radius: 10px;
            background-color: #1a1a20;
        }

        QTabBar::tab {
            background-color: #2a2a35;
            border: 2px solid #404050;
            padding: 12px 20px;
            margin-right: 3px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            color: #dcdce0;
            font-weight: bold;
        }

        QTabBar::tab:selected {
            background-color: #ff8c00;
            border-bottom-color: #ff8c00;
            color: #000000;
        }

        QTabBar::tab:hover {
            background-color: #3a3a45;
            color: #ffffff;
        }

        /* Progress Bar - Dark Progress */
        QProgressBar {
            border: 2px solid #404050;
            border-radius: 8px;
            text-align: center;
            background-color: #2a2a35;
            color: #ffffff;
            font-weight: bold;
            min-height: 20px;
        }

        QProgressBar::chunk {
            background-color: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0,
                stop: 0 #ff8c00, stop: 1 #ffaa33);
            border-radius: 6px;
        }

        /* Labels - Dark Display Labels */
        QLabel {
            color: #dcdce0;
        }

        QLabel#SectionLabel {
            font-weight: bold;
            color: #ff8c00;
            font-size: 12px;
            margin: 8px 0px;
        }

        QLabel#StatLabel {
            background-color: #2a2a35;
            border: 2px solid #404050;
            padding: 10px;
            border-radius: 8px;
            font-weight: bold;
            color: #ffffff;
        }

        /* Status Bar - Dark Status */
        QStatusBar {
            background-color: #0f0f12;
            border-top: 2px solid #ff8c00;
            color: #dcdce0;
            padding: 4px;
        }

        /* Scroll Bars - Dark Aviation */
        QScrollBar:vertical {
            background-color: #2a2a35;
            width: 14px;
            border-radius: 7px;
        }

        QScrollBar::handle:vertical {
            background-color: #404050;
            border-radius: 7px;
            min-height: 25px;
        }

        QScrollBar::handle:vertical:hover {
            background-color: #ff8c00;
        }

        QScrollBar:horizontal {
            background-color: #2a2a35;
            height: 14px;
            border-radius: 7px;
        }

        QScrollBar::handle:horizontal {
            background-color: #404050;
            border-radius: 7px;
            min-width: 25px;
        }

        QScrollBar::handle:horizontal:hover {
            background-color: #ff8c00;
        }

        /* Splitter - Dark Dividers */
        QSplitter::handle {
            background-color: #404050;
        }

        QSplitter::handle:hover {
            background-color: #ff8c00;
        }

        /* Special Thermal Stats Widget */
        QWidget#ThermalStats {
            background-color: #0f0f12;
            border: 3px solid #ff8c00;
            border-radius: 10px;
            padding: 10px;
        }
    )");
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Türkay Biliyor Paragliding - IGC Flight Analyzer");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Türkay Biliyor Paragliding");
    app.setApplicationDisplayName("IGC Flight Analyzer");

    // Apply professional dark flight theme
    setDarkFlightTheme(&app);

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}

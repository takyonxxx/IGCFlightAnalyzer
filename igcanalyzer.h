#ifndef IGCANALYZER_H
#define IGCANALYZER_H

#include <QObject>
#include <QString>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QTime>
#include <QDate>
#include <QtMath>
#include <vector>

struct IGCPoint {
    QDateTime timestamp;
    double latitude = 0.0;
    double longitude = 0.0;
    int pressureAltitude = 0;
    int gpsAltitude = 0;
    double verticalSpeed = 0.0;  // m/s
    double groundSpeed = 0.0;    // m/s
    double course = 0.0;         // degrees
    bool isValid = false;
};

struct ThermalPoint {
    QString name;
    QDateTime startTime;
    QDateTime endTime;
    double centerLatitude = 0.0;
    double centerLongitude = 0.0;
    double averageClimbRate = 0.0;  // m/s
    double maxClimbRate = 0.0;      // m/s
    double totalAltitudeGain = 0.0; // meters
    double radius = 0.0;            // meters
    int strength = 0;               // 1-5 scale
};

class IGCAnalyzer : public QObject
{
    Q_OBJECT

public:
    explicit IGCAnalyzer(QObject *parent = nullptr);

    // Core functionality
    bool loadIGCFile(const QString &fileName);
    void analyzeForThermals(double minClimbRate = 1.0, double thermalRadius = 200.0);
    void generateWaypointFile(const QString &fileName);

    // Getters for flight data
    const std::vector<IGCPoint>& getFlightData() const { return flightData; }
    const std::vector<ThermalPoint>& getThermals() const { return thermals; }

    // Flight information
    QString getFlightInfo() const;
    QString getThermalSummary() const;

    // Individual flight statistics
    QString getPilotName() const { return pilotName; }
    QString getGliderType() const { return gliderType; }
    QString getGliderID() const { return gliderID; }
    QDateTime getFlightDate() const { return flightDate; }

    // Enhanced flight statistics
    double getMaxVario() const { return maxVario; }
    double getMinVario() const { return minVario; }
    double getMaxGroundSpeed() const { return maxGroundSpeed; } // m/s
    double getAverageGroundSpeed() const { return averageGroundSpeed; } // m/s
    double getTotalFlightDistance() const { return totalFlightDistance; } // km
    double getStraightLineDistance() const { return straightLineDistance; } // km
    int getTakeoffAltitude() const { return takeoffAltitude; }
    int getFlightDurationSeconds() const { return flightDurationSeconds; }

    // XC calculations
    double getXCSpeed() const {
        if (flightDurationSeconds > 0) {
            return straightLineDistance / (flightDurationSeconds / 3600.0);
        }
        return 0.0;
    }

    // OLC calculations
    double calculateOLCDistance();
    double getOLCDistance() const { return olcDistance; }
    double calculateOLCPoints() const { return olcDistance * 1.5; } // Basic OLC scoring

    // Distance optimization
    double calculateMaximumDistance(); // Maximum distance from takeoff

signals:
    void analysisProgress(int percentage);
    void analysisComplete();

private:
    // Core data
    std::vector<IGCPoint> flightData;
    std::vector<ThermalPoint> thermals;

    // Flight metadata
    QString pilotName;
    QString gliderType;
    QString gliderID;
    QDateTime flightDate;

    // Enhanced flight statistics
    double maxVario = 0.0;           // m/s
    double minVario = 0.0;           // m/s
    double maxGroundSpeed = 0.0;     // m/s
    double averageGroundSpeed = 0.0; // m/s
    double totalFlightDistance = 0.0; // km
    double straightLineDistance = 0.0; // km
    int takeoffAltitude = 0;         // m
    int flightDurationSeconds = 0;   // seconds
    double olcDistance = 0.0;        // km
    double maximumDistance = 0.0;    // km

    // Private methods
    IGCPoint parseIGCLine(const QString &line);
    QDateTime parseIGCTime(const QString &timeStr, const QDate &date);
    double parseCoordinate(const QString &coord, bool isLatitude);

    void calculateVerticalSpeeds();
    void calculateGroundSpeeds();
    void calculateFlightStatistics(); // New method

    bool detectThermalTurning(const std::vector<IGCPoint> &points, int startIdx, int endIdx);
    ThermalPoint calculateThermalCenter(const std::vector<IGCPoint> &points, int startIdx, int endIdx);

    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    double calculateBearing(double lat1, double lon1, double lat2, double lon2);

    QString generateThermalName(const ThermalPoint &thermal, int index);
    QString formatCoordinate(double coord, bool isLatitude);
};

#endif // IGCANALYZER_H

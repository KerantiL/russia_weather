#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QTextStream>

#define DBName "russiaweaterdb.sqlite"
#define DBPath "./" DBName

#define DBRegionTable "region"
#define DBRegionName "name"

#define DBCityTable "city"
#define DBCityName "name"
#define DBCityLatitude "latitude"
#define DBCityLongitude "longitude"
#define DBCityRegionRef "region_id"

#define DBTemperatureTable "temperature"
#define DBTemperatureValue "value"
#define DBTemperatureDate "date"
#define DBTemperatureCityRef "city_id"

struct CityRecord {
    int id;
    QString name;
    double latitude;
    double longitude;
    int region_id;
};

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);
    ~Database() override;
    void connectToDatabase();
    bool fillDatabase();
    QVector<CityRecord> cities();
    double getTemperatureFromCityId(int id);
    void addTemp(double temp, int date, int city_id);

signals:
    void updateTemperatures();
    void searched(double latitude, double longitude);

public slots:
    int insertRegionRecord(const QString& name);
    bool insertCityRecord(const QString& name, double latitude, double longitude, int region_id);
    bool insertTemperatureRecord(double value, int date, int city_id);

private:
    bool openDatabase();
    bool restoreDatabase();
    bool createTables();
    void closeDatabase();

    bool createRegionTable();
    bool createCityTable();
    bool createTemperatureTable();

    bool fillRegion(const QString& region);

    QSqlDatabase db_;
};

#endif // DATABASE_H

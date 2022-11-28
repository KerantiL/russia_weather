#include "database.h"

Database::Database(QObject *parent) : QObject(parent)
{
    if (QSqlDatabase::contains("QSQLITE")) {
        db_ = QSqlDatabase::database("QSQLITE");
    }
}

Database::~Database() {
    if (db_.isOpen()) {
        db_.close();
    }
}

void Database::connectToDatabase()
{
    if(!QFile(DBPath).exists()){
        restoreDatabase();
    } else {
        openDatabase();
    }
}

QVector<CityRecord> Database::cities()
{
    QSqlQuery query;
    query.prepare("SELECT * FROM " DBCityTable);

    if (!query.exec()) {
        qDebug() << "Не удалось сделать выборку всех городов";
    }

    QVector<CityRecord> res;

    while (query.next()) {
        res.push_back({query.value("id").toInt(),
                       query.value("name").toString(),
                       query.value("latitude").toDouble(),
                       query.value("longitude").toDouble(),
                       query.value("region_id").toInt()});
    }

    return res;
}

void Database::addTemp(double temp, int date, int city_id) {
    insertTemperatureRecord(temp, date, city_id);
}


double Database::getTemperatureFromCityId(int id) {
    QSqlQuery query;
    query.prepare(" SELECT value FROM " DBTemperatureTable
                  " WHERE city_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Не удалось узнать температуру";
    }

    query.next();

    return query.value("value").toDouble();
}

bool Database::restoreDatabase()
{
    if (openDatabase()) {
        return createTables() && fillDatabase();
    } else {
        qDebug() << "Не удалось восстановить базу данных";
        return false;
    }
}

bool Database::openDatabase()
{
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName(DBPath);

    return db_.open();
}

void Database::closeDatabase()
{
    db_.close();
}

bool Database::createTables()
{
    return createRegionTable() && createCityTable() && createTemperatureTable();
}

bool Database::createRegionTable() {
    QSqlQuery query;

    bool res = query.exec( "CREATE TABLE " DBRegionTable " ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               DBRegionName " VARCHAR(255) NOT NULL"
                           ");");
    if (!res) {
        qDebug() << "DataBase: error of create " << DBRegionTable;
        qDebug() << query.lastError().text();
    }

    return res;
}

bool Database::createCityTable() {
    QSqlQuery query;

    bool res = query.exec( "CREATE TABLE " DBCityTable " ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               DBCityName " VARCHAR(255) NOT NULL, "
                               DBCityLatitude " REAL NOT NULL, "
                               DBCityLongitude " REAL NOT NULL, "
                               DBCityRegionRef " INTEGER, "
                               "FOREIGN KEY(" DBCityRegionRef ") REFERENCES region(id)"
                           ");");
    if (!res) {
        qDebug() << "DataBase: error of create " << DBCityTable;
        qDebug() << query.lastError().text();
    }

    return res;
}

bool Database::createTemperatureTable() {
    QSqlQuery query;

    bool res = query.exec( "CREATE TABLE " DBTemperatureTable " ("
                               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                               DBTemperatureValue " VARCHAR(255) NOT NULL, "
                               DBTemperatureDate " INTEGER NOT NULL, "
                               DBTemperatureCityRef " INTEGER, "
                               "FOREIGN KEY(" DBTemperatureCityRef ") REFERENCES region(id)"
                           ");");
    if (!res) {
        qDebug() << "DataBase: error of create " << DBTemperatureTable;
        qDebug() << query.lastError().text();
    }

    return res;
}

int Database::insertRegionRecord(const QString& name)
{
    QSqlQuery query;

    query.prepare("INSERT INTO " DBRegionTable " ( " DBRegionName ") "
                  "VALUES (\"" + name + "\")");

    if (!query.exec()) {
        qDebug() << "error insert into" << DBRegionTable;
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();
        return -1;
    } else {
        return query.lastInsertId().toInt();
    }
}

bool Database::insertCityRecord(const QString& name, double latitude, double longitude, int region_ref)
{
    QSqlQuery query;

    query.prepare("INSERT INTO " DBCityTable " ( " DBCityName ", "
                                                   DBCityLatitude ", "
                                                   DBCityLongitude ", "
                                                   DBCityRegionRef ") "
                  "VALUES (:name, :latitude, :longitude, :region_ref)");

    query.bindValue(":name", name);
    query.bindValue(":latitude", latitude);
    query.bindValue(":longitude", longitude);
    query.bindValue(":region_ref", region_ref);

    if (!query.exec()) {
        qDebug() << "error insert into " << DBCityTable;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
}

bool Database::insertTemperatureRecord(double value, int date, int city_id)
{
    QSqlQuery query;

    query.prepare("INSERT INTO " DBTemperatureTable " ( " DBTemperatureValue ", "
                                                          DBTemperatureDate ", "
                                                          DBTemperatureCityRef ") "
                  "VALUES (:value, :date, :city_id)");

    query.bindValue(":value", value);
    query.bindValue(":date", date);
    query.bindValue(":city_id", city_id);

    if (!query.exec()) {
        qDebug() << "error insert into " << DBTemperatureTable;
        qDebug() << query.lastError().text();
        return false;
    } else {
        return true;
    }
}

bool Database::fillDatabase() {
    QDir dir(":/regions");
    for (auto& i : dir.entryList()) {
        fillRegion(i);
    }

    return false;
}

bool Database::fillRegion(const QString &region) {
    QFile file(":/regions/" + region);
    QTextStream s(&file);
    file.open(QFile::ReadOnly);
    QString regionName = region.chopped(4);

    auto region_id = insertRegionRecord(regionName);

    QString cityName;
    double latitude, longitude;


    while (!(s >> cityName >> latitude >> longitude).atEnd()) {
        insertCityRecord(cityName, latitude, longitude, region_id);
    }

    return false;
}

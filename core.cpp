#include "core.h"

Core::Core(QObject *parent)
    : QObject{parent}
{

}

void Core::init() {
    db_.connectToDatabase();
}

void Core::updateTemp() {
    QNetworkRequest request;
    auto cities = db_.cities();
    counter_ = cities.size();

    for (auto& i : cities) {
       request.setUrl(QString("http://api.weatherapi.com/v1/current.json?key=42cd1d95637c4da889f94816221711&q=%1,%2&aqi=no").arg(i.latitude).arg(i.longitude));
       auto reply = nam_.get(request);
       QObject::connect(reply, &QNetworkReply::finished, [=](){
           counter_--;
           auto json = QJsonDocument::fromJson(reply->readAll()).object();
           temps_.push_back(json["current"].toObject()["temp_c"].toDouble());

           if (counter_ == 0) {
               updateTemp2();
           }
       });
    }
}

void Core::updateTemp2() {
    qDebug() << "update temps";
    auto cities = db_.cities();

    for (int i = 0; i < temps_.size(); i++) {
        db_.addTemp(temps_[i], std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()), cities[i].id);
    }
}

void Core::search(double latitude, double longitude) {
    QNetworkRequest request(QString("http://api.weatherapi.com/v1/current.json?key=42cd1d95637c4da889f94816221711&q=%1,%2&aqi=no").arg(latitude).arg(longitude));
    auto reply = nam_.get(request);

    QObject::connect(reply, &QNetworkReply::finished, [=](){
        auto json = QJsonDocument::fromJson(reply->readAll()).object();
        emit searched(json["current"].toObject()["temp_c"].toDouble());
    });
}

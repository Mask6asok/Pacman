#ifndef HTTP_H
#define HTTP_H
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QEventLoop>
#include <QUrlQuery>
#include <iostream>
#include <string>
using namespace std;
class http
{
public:
    http();
    QString post(QString name, int score);

private:
};

#endif // HTTP_H

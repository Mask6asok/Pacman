#include "http.h"

http::http()
{
}

QString http::post(QString name, int score)
{
    char baseUrl[100];
    char *n;
    string a = name.toStdString();
    n = new char[a.size() + 1];
    strcpy(n, a.c_str());
    sprintf(baseUrl, "http://s.edgeless.top/pacman.php?name=%s&score=%d", n, score);
    QString url = QString(baseUrl);
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::UserAgentHeader, "PACMAN");
    request.setUrl(QUrl(baseUrl));
    QNetworkAccessManager *manager = new QNetworkAccessManager();
    QNetworkReply *pReplay = manager->post(request, QByteArray());
    QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, &eventLoop, &QEventLoop::quit);
    eventLoop.exec();
    QByteArray bytes = pReplay->readAll();
    qDebug() << bytes;
    QString s = QString(bytes);
    s.replace(QString("\\n"), QString("\n"));
    qDebug() << s;
    return s;
}

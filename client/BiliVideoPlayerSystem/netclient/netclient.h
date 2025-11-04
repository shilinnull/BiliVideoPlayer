#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

namespace netclient {

class netclient : public QObject
{
    Q_OBJECT
public:
    netclient(QObject *parent = nullptr);
    void hello();	// 	发送hello请求
private:
    static QString makeRequeId();
private:
    const QString HTTP_URL = "http://127.0.0.1:8000";

    QNetworkAccessManager httpClient;
};

}	// end netclient
#endif // NETCLIENT_H

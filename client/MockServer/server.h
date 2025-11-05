#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QHttpServerRequest>

class MockServer: public QObject
{
    Q_OBJECT
public:
    ~MockServer();
public:
    bool init();
    static MockServer* getInstance();
private:
    MockServer();
    static MockServer* instance;
    QHttpServer httpServer;
private:
    QHttpServerResponse hello(const QHttpServerRequest& req);
};
#endif // SERVER_H

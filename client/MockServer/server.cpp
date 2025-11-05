#include "server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include "util.h"

MockServer::MockServer()
{}

MockServer* MockServer::instance = nullptr;

QHttpServerResponse MockServer::hello(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[hello] 收到 hello 请求， requestId = " << jsonReq["requestId"].toString();

    // 构造响应正文
    QJsonObject jsonBody;
    jsonBody["hello"] = "world";
    QJsonObject jsonResp;
    jsonResp["errmsg"] = "";
    jsonResp["data"] = jsonBody;
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

MockServer::~MockServer() {}

bool MockServer::init()
{
    int ret = httpServer.listen(QHostAddress::Any, 8000);
    httpServer.route("/hello", [=](const QHttpServerRequest& req){
        return this->hello(req);
    });
    return 8000 == ret;
}

MockServer* MockServer::getInstance()
{
    if(instance == nullptr) {
        instance = new MockServer();
        return instance;
    }
    return instance;
}

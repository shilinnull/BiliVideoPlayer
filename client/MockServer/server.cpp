#include "server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include "util.h"

MockServer::MockServer()
{}

MockServer* MockServer::instance = nullptr;

MockServer::~MockServer() {}

bool MockServer::init()
{
    int ret = httpServer.listen(QHostAddress::Any, 8000);

    // 临时用户登录
    httpServer.route("/HttpService/tempLogin", [=](const QHttpServerRequest& req){
        return this->tempLogin(req);
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

QHttpServerResponse MockServer::tempLogin(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[tempLogin] 收到 tempLogin 请求, requestId=" << jsonReq["requestId"].toString();

    roleType = TempUser;		// 设置临时用户登录

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;

    QJsonObject sessionJson;
    QString sessionId = QUuid::createUuid().toString();
    sessionId = sessionId.mid(25, 12);
    sessionJson["sessionId"] = sessionId;

    jsonBody["result"] = sessionJson;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

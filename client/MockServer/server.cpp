#include "server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
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

    // 获取所有视频列表
    httpServer.route("/HttpService/allVideoList", [=](const QHttpServerRequest& req){
        return this->allVideoList(req);
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

QHttpServerResponse MockServer::allVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[allVideoList] 收到 allVideoList 请求, requestId=" << jsonReq["requestId"].toString();

    // 构造正文
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    // 构造result部分
    int pageCount = jsonReq["pageCount"].toInt();
    QJsonObject allVideoResultObj;
    allVideoResultObj["totalCount"] = pageCount;

    int videoId = 10000;
    int userId = 10000;
    int resourceId = 1000;
    int fileId = 10000;

    QJsonArray videoLists;
    for (int i = 0; i < pageCount; ++i) {
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["nickname"] = "这是一个用户名";
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(fileId++);
        videoJsonObj["likeCount"] = 1234;
        videoJsonObj["playCount"] = 23456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoJsonObj["videoTitle"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9-16 12:28:58";

        // 将视频对象添加到数组中
        videoLists.append(videoJsonObj);
    }
    allVideoResultObj["videoList"] = videoLists;
    jsonBody["result"] = allVideoResultObj;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    // 构造HTTP响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-type", "application/json; charset=utf-8");
    return httpResp;
}

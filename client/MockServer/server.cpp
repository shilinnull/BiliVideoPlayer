#include "server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtGlobal>
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

    // 获取分类视频列表
    httpServer.route("/HttpService/typeVideoList", [=](const QHttpServerRequest& req){
        return this->typeVideoList(req);
    });

    // 获取标签视频列表
    httpServer.route("/HttpService/tagVideoList", [=](const QHttpServerRequest& req){
        return this->tagVideoList(req);
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

QHttpServerResponse MockServer::allVideoList(const QHttpServerRequest &request)
{
    // 1. 获取到请求中数据
    QJsonDocument docReq = QJsonDocument::fromJson(request.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[allVideoList] 收到 allVideoList 请求， requestId = "<<jsonReq["requestId"].toString();

    // 2. 构造响应体
    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    int pageCount = jsonReq["pageCount"].toInt();
    QJsonObject resultJsonObj;
    resultJsonObj["totalCount"] = 100;

    int videoId = 10000;
    int userId = 10000;
    int resourceId = 10000;
    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i){
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["nickname"] = "用户昵称";
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234;
        videoJsonObj["playCount"] = 3456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩~";
        videoJsonObj["videoTitle"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9-16 12:28:58";

        videoLists.append(videoJsonObj);
    }
    resultJsonObj["videoList"] = videoLists;
    jsonResp["result"] = resultJsonObj;

    // 3. 返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::typeVideoList(const QHttpServerRequest &request)
{
    // 1. 获取到请求中数据
    QJsonDocument docReq = QJsonDocument::fromJson(request.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[typeVideoList] 收到 typeVideoList 请求， requestId = "<<jsonReq["requestId"].toString();

    // 2. 构造响应体
    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 10;
    QJsonObject resultJsonObj;
    resultJsonObj["totalCount"] = 100;

    int videoId = 20000;
    int userId = 20000;
    int resourceId = 20000;
    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i){
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["nickname"] = "用户昵称";
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234;
        videoJsonObj["playCount"] = 23456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩~";
        videoJsonObj["videoTitle"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9-16 12:28:58";

        videoLists.append(videoJsonObj);
    }
    resultJsonObj["videoList"] = videoLists;
    jsonResp["result"] = resultJsonObj;

    // 3. 返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::tagVideoList(const QHttpServerRequest &req)
{
    // 1. 获取到请求中数据
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[tagVideoList] 收到 tagVideoList 请求， requestId = "<<jsonReq["requestId"].toString();

    // 2. 构造响应体
    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 13;
    QJsonObject resultJsonObj;
    resultJsonObj["totalCount"] = 100;

    int videoId = 30000;
    int userId = 30000;
    int resourceId = 30000;
    QJsonArray videoLists;
    for(int i = 0; i < pageCount; ++i){
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["nickname"] = "用户昵称";
        videoJsonObj["photoFileId"] = QString::number(resourceId++);
        videoJsonObj["videoFileId"] = QString::number(resourceId++);
        videoJsonObj["likeCount"] = 1234;
        videoJsonObj["playCount"] = 23456;
        videoJsonObj["videoSize"] = 10240;
        videoJsonObj["videoDesc"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩一条视频告诉你去了北京该怎么玩"
                                    "一条视频告诉你去了北京该怎么玩~";
        videoJsonObj["videoTitle"] = "【北京旅游攻略】一条视频告诉你去了北京该怎么玩";
        videoJsonObj["videoDuration"] = 10;
        videoJsonObj["videoUpTime"] = "9-16 12:28:58";

        videoLists.append(videoJsonObj);
    }
    resultJsonObj["videoList"] = videoLists;
    jsonResp["result"] = resultJsonObj;

    // 3. 返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

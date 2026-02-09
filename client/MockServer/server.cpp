#include "server.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QtGlobal>
#include <QDir>
#include "util.h"

MockServer::MockServer()
{}

MockServer* MockServer::instance = nullptr;

MockServer::~MockServer() {}

bool MockServer::init()
{
    const bool ok = httpServer.listen(QHostAddress::Any, 8000);

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

    // 根据搜索文本获取视频列表
    httpServer.route("/HttpService/keyVideoList", [=](const QHttpServerRequest& req){
        return this->keyVideoList(req);
    });

    // 先构造数据
    buildResponseData();
    // 获取图片
    httpServer.route("/HttpService/downloadPhoto", [=](const QHttpServerRequest& req){
        return this->downloadPhoto(req);
    });

    // 获取图片
    httpServer.route("/HttpService/uploadPhoto", [=](const QHttpServerRequest& req){
        return this->uploadPhoto(req);
    });

    // 下载视频
    httpServer.route("/HttpService/downloadVideo", [=](const QHttpServerRequest& req){
        return this->downloadVideo(req);
    });

    // 模拟videos⽬录下视频文件下载
    httpServer.route("/videos/<arg>", [=](const QString& req){
        return this->downloadVideoSeg(req);
    });

    // 上传视频
    httpServer.route("/HttpService/uploadVideo", [=](const QHttpServerRequest& req){
        return this->uploadVideo(req);
    });


    // 删除视频
    httpServer.route("/HttpService/removeVideo", [=](const QHttpServerRequest& req){
        return this->removeVideo(req);
    });

    // 获取弹幕数据
    httpServer.route("/HttpService/getBarrage", [=](const QHttpServerRequest& req){
        return this->getBarrage(req);
    });

    // 获取弹幕数据
    httpServer.route("/HttpService/setPlay", [=](const QHttpServerRequest& req){
        return this->setPlay(req);
    });

    // 检测是否点赞
    httpServer.route("/HttpService/judgeLike", [=](const QHttpServerRequest& req){
        return this->judgeLike(req);
    });

    // 设置点赞
    httpServer.route("/HttpService/setLike", [=](const QHttpServerRequest& req){
        return this->setLike(req);
    });

    // 新增弹幕
    httpServer.route("/HttpService/newBarrage", [=](const QHttpServerRequest& req){
        return this->newBarrage(req);
    });

    // 获取个人信息
    httpServer.route("/HttpService/getUserInfo", [=](const QHttpServerRequest& req){
        return this->getUserInfo(req);
    });

    // 获修改头像
    httpServer.route("/HttpService/setAvatar", [=](const QHttpServerRequest& req){
        return this->setAvatar(req);
    });

    // 获取指定用户视频列表
    httpServer.route("/HttpService/userVideoList", [=](const QHttpServerRequest& req){
        return this->userVideoList(req);
    });

    // 新增关注
    httpServer.route("/HttpService/newAttention", [=](const QHttpServerRequest& req){
        return this->newAttention(req);
    });

    // 取消关注
    httpServer.route("/HttpService/delAttention", [=](const QHttpServerRequest& req){
        return this->delAttention(req);
    });

    // 获取验证码
    httpServer.route("/HttpService/getCode", [=](const QHttpServerRequest& req){
        return this->getCode(req);
    });

    // 验证码登录
    httpServer.route("/HttpService/vcodeLogin", [=](const QHttpServerRequest& req){
        return this->vcodeLogin(req);
    });

    // 账号密码登录
    httpServer.route("/HttpService/passwdLogin", [=](const QHttpServerRequest& req){
        return this->passwdLogin(req);
    });

    // session登录
    httpServer.route("/HttpService/sessionLogin", [=](const QHttpServerRequest& req){
        return this->sessionLogin(req);
    });

    // 退出登录
    httpServer.route("/HttpService/logout", [=](const QHttpServerRequest& req){
        return this->logout(req);
    });

    // 设置用户密码
    httpServer.route("/HttpService/setPassword", [=](const QHttpServerRequest& req){
        return this->setPassword(req);
    });

    // 设置用户昵称
    httpServer.route("/HttpService/setNickname", [=](const QHttpServerRequest& req){
        return this->setNickname(req);
    });

    // 新增视频
    httpServer.route("/HttpService/newVideo", [=](const QHttpServerRequest& req){
        return this->newVideo(req);
    });

    // 获取状态视频列表
    httpServer.route("/HttpService/statusVideoList", [=](const QHttpServerRequest& req){
        return this->statusVideoList(req);
    });

    // 获取状态视频列表
    httpServer.route("/HttpService/checkVideo", [=](const QHttpServerRequest& req){
        return this->checkVideo(req);
    });

    // 上架视频
    httpServer.route("/HttpService/saleVideo", [=](const QHttpServerRequest& req){
        return this->saleVideo(req);
    });

    // 下架视频
    httpServer.route("/HttpService/haltVideo", [=](const QHttpServerRequest& req){
        return this->haltVideo(req);
    });

    // 通过手机号获取管理员信息
    httpServer.route("/HttpService/getAdminByPhone", [=](const QHttpServerRequest& req){
        return this->getAdminByPhone(req);
    });
    // 通过管理员状态获取管理员信息
    httpServer.route("/HttpService/getAdminListByStatus", [=](const QHttpServerRequest& req){
        return this->getAdminListByStatus(req);
    });

    // 新增管理员
    httpServer.route("/HttpService/newAdministrator", [=](const QHttpServerRequest& req){
        return this->newAdministrator(req);
    });

    // 新增管理员
    httpServer.route("/HttpService/setAdministrator", [=](const QHttpServerRequest& req){
        return this->setAdministrator(req);
    });

    // 新增管理员
    httpServer.route("/HttpService/setStatus", [=](const QHttpServerRequest& req){
        return this->setStatus(req);
    });

    // 删除管理员
    httpServer.route("/HttpService/delAdministrator", [=](const QHttpServerRequest& req){
        return this->delAdministrator(req);
    });

    return ok;
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
        videoJsonObj["playCount"] = 666;
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

QHttpServerResponse MockServer::keyVideoList(const QHttpServerRequest &req)
{
    // 1. 获取到请求中数据
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[keyVideoList] 收到 keyVideoList 请求， requestId = "<<jsonReq["requestId"].toString();
    LOG()<<jsonReq["searchKey"].toString();

    // 2. 构造响应体
    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    int pageCount = jsonReq["pageCount"].toInt();
    pageCount = 1;
    QJsonObject resultJsonObj;
    resultJsonObj["totalCount"] = 100;

    int videoId = 40000;
    int userId = 40000;
    int resourceId = 40000;
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

void MockServer::buildResponseData()
{
    int resourceId = 10000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
    resourceId = 20000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang1.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage1.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
    resourceId = 30000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang2.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage2.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
    resourceId = 40000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage3.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
    resourceId = 50000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage3.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
    idPathTable.insert(QString::number(60000), "/videos/");
    resourceId = 70000;
    for(int i = 0; i < 100; i++) {
        idPathTable.insert(QString::number(resourceId++), "/images/touxiang3.png");
        idPathTable.insert(QString::number(resourceId++), "/images/videoImage3.png");
        idPathTable.insert(QString::number(resourceId++), "/videos/111.m3u8");
    }
}

QHttpServerResponse MockServer::downloadPhoto(const QHttpServerRequest &req)
{
    QUrlQuery query(QUrl(req.url()));
    QString requestId = query.queryItemValue("requestId");
    QString fileId = query.queryItemValue("fileId");
    QDir dir(QDir::currentPath());
    dir.cdUp(); dir.cdUp();
    QString imagePath = dir.absolutePath();
    imagePath += idPathTable[fileId];
    // LOG()<<"图片ID："<<fileId<<"--"<<imagePath;
    QByteArray imageData = loadFileToByteArray(imagePath);
    QHttpServerResponse httpResp(imageData, QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/octet-stream");
    return httpResp;
}

QHttpServerResponse MockServer::uploadPhoto(const QHttpServerRequest &req)
{
    QUrlQuery query(QUrl(req.url()));
    QString requestId = query.queryItemValue("requestId");
    QString sessionId = query.queryItemValue("sessionId");
    LOG() << "[uploadPhoto] 收到 uploadPhoto 请求, requestId=" << requestId << "sessionId: " << sessionId;
    // 解析图片数据
    const QByteArray imageData = req.body();
    // 临时保存
    QDir dir(QDir::currentPath());
    dir.cdUp(); dir.cdUp();
    QString imagePath = dir.absolutePath();
    imagePath += "/images/temp.png";
    LOG() << imagePath;
    writeByteArrayToFile(imagePath, imageData); // 写文件
    idPathTable["1000"] = "/images/temp.png";

    QJsonObject resultObj;
    resultObj["fileId"] = "1000";

    QJsonObject jsonBody;
    jsonBody["requestId"] = requestId;
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";
    jsonBody["result"] = resultObj;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json");
    return httpResp;
}

QHttpServerResponse MockServer::downloadVideo(const QHttpServerRequest &req)
{
    QUrlQuery query(req.url());
    QString requestId = query.queryItemValue("requestId");
    QString fileId = query.queryItemValue("fileId");
    LOG()<<"[downloadVideo] 收到 downloadVideo 请求， requestId="<<requestId;
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString imagePath = dir.absolutePath();
    imagePath += idPathTable[fileId];
    // LOG()<<"视频id："<<fileId<<"--"<<imagePath;

    if (!QFileInfo::exists(imagePath)) {
        LOG() << "视频文件不存在:" << imagePath;
    }

    QByteArray imageData = loadFileToByteArray(imagePath);

    QHttpServerResponse  httpResp(imageData, QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-type", "text/plain");
    return httpResp;
}

QHttpServerResponse MockServer::removeVideo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[removeVideo] 收到 removeVideo 请求, requestId=" << jsonReq["requestId"].toString()
          << "videoId: " << jsonReq["videoId"].toString();

    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::checkVideo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[checkVideo] 收到 checkVideo 请求, requestId=" << jsonReq["requestId"].toString()
          << "videoId: " << jsonReq["videoId"].toString();

    bool checkResult = jsonReq["checkResult"].isBool();
    QString videoId = jsonReq["videoId"].toString();
    if(checkResult) {
        LOG() << "视频: " << videoId  << "审核通过";
    } else {
        LOG() << "视频: " << videoId  << "审核驳回";
    }
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::saleVideo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[saleVideo] 收到 saleVideo 请求, requestId="
          << jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();
    LOG()<<"视频 "<< videoId <<"上架";

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";
    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::haltVideo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[haltVideo] 收到 haltVideo 请求, requestId="
          << jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();
    LOG()<<"视频 "<< videoId <<"下架";

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";
    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::newAttention(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[newAttention] 收到 newAttention 请求, requestId=" << jsonReq["requestId"].toString()
          << "videoId: " << jsonReq["videoId"].toString();

    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::delAttention(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[delAttention] 收到 delAttention 请求, requestId=" << jsonReq["requestId"].toString()
          << "videoId: " << jsonReq["videoId"].toString();

    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::downloadVideoSeg(const QString &fileName)
{
    QString normalizedName = fileName;
    if (normalizedName.startsWith('/')) {
        normalizedName = normalizedName.mid(1);
    }

    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString filePath = dir.absolutePath();
    filePath += "/videos/" + normalizedName;

    QByteArray fileData = loadFileToByteArray(filePath);

    if (!QFileInfo::exists(filePath)) {
        LOG() << "视频分片不存在:" << filePath;
    }

    QHttpServerResponse response(fileData, QHttpServerResponse::StatusCode::Ok);
    response.setHeader("Content-type", "application/octet-stream");
    return response;
}

QHttpServerResponse MockServer::uploadVideo(const QHttpServerRequest &req)
{
    QUrlQuery query(req.url());
    QString requestId = query.queryItemValue("requestId");
    LOG()<<"[downloadVideo] 收到 downloadVideo 请求， requestId="<<requestId;


    // 构造视频路径
    QDir dir(QDir::currentPath());
    dir.cdUp();
    dir.cdUp();
    QString videoPath = dir.absolutePath();
    videoPath += idPathTable["60000"];
    videoPath += "222.mp4";
    LOG() << videoPath;

    // 写入文件
    QByteArray videoData = req.body();
    writeByteArrayToFile(videoPath, videoData);

    // 构造http响应
    QJsonObject jsonBody;
    jsonBody["requestId"] = requestId;
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";
    QJsonObject resultJson;
    resultJson["fileId"] = "60000";
    jsonBody["result"] = resultJson;

    // 序列化
    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse  httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::newVideo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();

    LOG() << "[newVideo] 收到 newVideo 请求, requestId=" << jsonReq["requestId"].toString();

    // 解析上传视频信息
    QJsonObject videoInfo = jsonReq["videoInfo"].toObject();
    QString videoFileID = videoInfo["videoFileId"].toString();
    QString photoFileID = videoInfo["photoFileId"].toString();
    QString videoTitle = videoInfo["videoTitle"].toString();
    int videoType = videoInfo["videoType"].toInteger();
    QString videoDesc = videoInfo["videoDesc"].toString();
    int64_t duration = videoInfo["duration"].toInteger();
    LOG()<<"videoFileID : "<< videoFileID;
    LOG()<<"photoFileID : "<< photoFileID;
    LOG()<<"videoTitle : "<< videoTitle;
    LOG()<<"videoDesc : "<< videoDesc;
    LOG()<<"videoType : "<< videoType;
    LOG()<<"duration : "<< duration;

    QJsonArray tagIds = videoInfo["videoTag"].toArray();
    for(int i = 0; i < tagIds.size(); ++i){
        QJsonObject tagJson = tagIds[i].toObject();
    }

    // 构造正文
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    // 构造 HTTP 响应
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::statusVideoList(const QHttpServerRequest &req)
{
    // 1. 获取到请求中数据
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[statusVideoList] 收到 statusVideoList 请求， requestId = "<<jsonReq["requestId"].toString();

    int videoId = 70000;
    int userId = 70000;
    int resourceId = 70000;
    QJsonArray videoLists;
    int pageCount = jsonReq["pageCount"].toInt();
    for(int i = 0; i < pageCount; ++i){
        QJsonObject videoJsonObj;
        videoJsonObj["videoId"] = QString::number(videoId++);
        videoJsonObj["userId"] = QString::number(userId++);
        videoJsonObj["userAvatarId"] = QString::number(resourceId++);
        videoJsonObj["nickname"] = "王五";
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
        int videoStatus = jsonReq["videoStatus"].toInt();
        if(0 == videoStatus){
            videoJsonObj["videoStatus"] = rand()%4+1;
        }else{
            videoJsonObj["videoStatus"] = videoStatus;
        }

        videoJsonObj["checkerId"] = "12345";
        videoJsonObj["checkerName"] = "李四";
        videoJsonObj["checkerAvatar"] = "50000";

        videoLists.append(videoJsonObj);
    }

    // 2. 构造响应体
    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    QJsonObject resultJson;
    resultJson["videoList"] = videoLists;
    resultJson["totalCount"] = 200;
    jsonResp["result"] = resultJson;

    // 3. 返回响应
    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::getBarrage(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[getBarrage] 收到 getBarrage 请求, requestId=" << jsonReq["requestId"].toString();

    // 构造不同时间的弹幕
    QList<BarrageInfo> barrageList;
    for(int i = 0; i < 3; i++) {
        int64_t playTime = i + 2;
        BarrageInfo barrage("10000001", i + 2, "我是弹幕" + QString::number(i));
        barrageList.append(barrage);
        barrages.insert(playTime, barrageList);
        barrages.clear();
    }
    // 构造相同时间的弹幕
    for(int i = 0; i < 3; i++) {
        int64_t playTime = 3;
        BarrageInfo barrage("10000001", playTime, "我是第" + QString::number(playTime) + "秒弹幕" + QString::number(i));
        barrageList.append(barrage);
    }

    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonArray barrageArray;
    for(auto& barrage : barrageList) {
        QJsonObject barrageObj;
        barrageObj["barrageContent"] = barrage.text;
        barrageObj["barrageTime"] = barrage.playTime;
        barrageObj["userId"] = barrage.userId;

        barrageArray.append(barrageObj);
    }
    QJsonObject resultJson;
    resultJson["barrageList"] = barrageArray;
    jsonBody["result"] = resultJson;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setPlay(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[setPlay] 收到 setPlay 请求， requestId = "<<jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();
    LOG() <<"视频"<<videoId<<"播放次数更新成功";

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::judgeLike(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[judgeLike] 收到 judgeLike 请求， requestId = "<<jsonReq["requestId"].toString();

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    QJsonObject resultJson;
    resultJson["isLike"] = true;            // 设置成 true 给客户端测试响应
    jsonResp["result"] = resultJson;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setLike(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[setLike] 收到 setLike 请求， requestId = "<<jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();
    LOG() << "视频" << videoId << "点赞成功";

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::newBarrage(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[newBarrage] 收到 newBarrage 请求， requestId = "<<jsonReq["requestId"].toString();

    QString videoId = jsonReq["videoId"].toString();

    // 解析弹幕信息
    BarrageInfo barrageInfo;
    QJsonObject barrageObj = jsonReq["barrageInfo"].toObject();
    barrageInfo.playTime = barrageObj["barrageTime"].toInteger();
    barrageInfo.text = barrageObj["barrageContent"].toString();
    LOG()<<"视频 "<<videoId<<" 插入弹幕 "<< "播放时间：" << barrageInfo.playTime << "内容：" <<barrageInfo.text;
    barrages[barrageInfo.playTime].append(barrageInfo);

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::getUserInfo(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    const QString userId = jsonReq["userId"].toString();
    LOG()<<"[getUserInfo] 收到 getUserInfo 请求， requestId = "
          <<jsonReq["requestId"].toString() << "userId: " << userId;

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    // 构造用户信息
    QJsonObject userInfoObj;
    if(userId.isEmpty()) {
        // 获取个人信息
        userInfoObj["userId"] = "1000001";    // 用户id
        userInfoObj["phoneNum"] = "15012345678";   // 手机号码
        userInfoObj["nickname"] = "张三";    // 用户昵称

        // 角色类型：0-未知，1-超级管理员，2-普通管理员，3-普通用户，4-临时用户
        QJsonArray roleTypeArray;
        roleTypeArray.append(3);
        userInfoObj["roleType"] = roleTypeArray;

        // 身份类型：0-未知，1-C端用户，2-B端用户
        // 临时用户的身份类型为空
        QJsonArray identityArray;
        identityArray.append(2);
        userInfoObj["identityType"] = identityArray;

        userInfoObj["likeCount"] = 12345;    // 点赞数
        userInfoObj["playCount"] = 12346;    // 播放数
        userInfoObj["followedCount"] = 123;    // 关注数
        userInfoObj["followerCount"] = 123;    // 粉丝数
        userInfoObj["userStatus"] = 0;      // 用户状态
        userInfoObj["isFollowing"] = 0;      // 是否关注
        userInfoObj["userMemo"] = "";       // 用户备注信息
        userInfoObj["userTime"] = "2038-12-18 12:28:58"; // 用户创建时间
        userInfoObj["avatarFileID"] = "10001";    // 用户图像id
    } else {
        // 获取其他用户信息
        userInfoObj["userId"] = "1000002";
        userInfoObj["phoneNum"] = "15022345678";
        userInfoObj["nickname"] = "李四";

        // 角色类型：0-未知，1-超级管理员，2-普通管理员，3-普通用户，4-临时用户
        QJsonArray roleTypeArray;
        roleTypeArray.append(3);
        userInfoObj["roleType"] = roleTypeArray;

        // 身份类型：0-未知，1-C端用户，2-B端用户
        // 临时用户的身份类型为空
        QJsonArray identityArray;
        identityArray.append(2);
        userInfoObj["identityType"] = identityArray;

        userInfoObj["likeCount"] = 12345;
        userInfoObj["playCount"] = 12346;
        userInfoObj["followedCount"] = 123;
        userInfoObj["followerCount"] = 123;
        userInfoObj["userStatus"] = 0;
        userInfoObj["isFollowing"] = 0;
        userInfoObj["userMemo"] = "";
        userInfoObj["userCTime"] = "2038-12-18 12:28:58";
        userInfoObj["avatarFileIdx"] = "10001";
    }

    QJsonObject resultObj;
    resultObj["userInfo"] = userInfoObj;
    jsonResp["result"] = resultObj;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setAvatar(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[setAvatar] 收到 setAvatar 请求， requestId = "
          <<jsonReq["requestId"].toString() << ", fileId: " << jsonReq["fileId"].toString();

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::userVideoList(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[userVideoList] 收到 userVideoList 请求， requestId = "<<jsonReq["requestId"].toString();

    QJsonArray videoListObject;
    int videoId = 10000;
    int userId = 50000;
    int resourceId = 50000;
    QJsonArray videoLists;
    for(int i = 0; i < jsonReq["pageCount"].toInt(); ++i){
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
        videoJsonObj["videoStatus"] = 2;
        videoJsonObj["checkerId"] = "1234";
        videoJsonObj["checkerName"] = "张三";
        videoJsonObj["checkerAvatar"] = "";

        videoListObject.append(videoJsonObj);
    }
    QJsonObject resultObject;
    resultObject["videoList"] = videoListObject;
    resultObject["totalCount"] = 50;

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["result"] = resultObject;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::getCode(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[getCode] 收到 getCode 请求， requestId = "
          <<jsonReq["requestId"].toString();

    QString phoneNum = jsonReq["phoneNumber"].toString();
    LOG() << "给手机号: " << phoneNum << "发送验证码: 123456";


    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] = "";

    QJsonObject authcodeJson;
    authcodeJson["codeId"] = "111111";
    jsonResp["result"] = authcodeJson;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::vcodeLogin(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[vcodeLogin] 收到 vcodeLogin 请求， requestId = "
          <<jsonReq["requestId"].toString();

    QString phoneNum = jsonReq["phoneNumber"].toString();
    QString authcode = jsonReq["verifyCode"].toString();
    QString codeId = jsonReq["codeId"].toString();
    LOG() << "给手机号: " << phoneNum << "发送验证码: " << authcode << "验证码id: " << codeId;

    int errorCode = 0;
    QString errorMsg;
    if(phoneNum != "10000000000") {
        errorCode = 600;
        errorMsg = "手机号输入有误！";
    }
    if(authcode != "123456") {
        errorCode  = 601;
        errorMsg = "验证码输入有误！";
    }
    if(codeId != "111111") {
        errorCode = 602;
        errorMsg = "验证码id输入有误！";
    }

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = errorCode;
    jsonResp["errorMsg"] = errorMsg;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::passwdLogin(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[passwdLogin] 收到 passwdLogin 请求， requestId = "
          <<jsonReq["requestId"].toString();

    QString phoneNum = jsonReq["phoneNumber"].toString();
    QString password = jsonReq["password"].toString();
    LOG() << "passwdLogin 收到账户: " << phoneNum << "登录密码: " << password;

    int errorCode = 0;
    QString errorMsg;
    if(phoneNum != "10000000000") {
        errorCode = 603;
        errorMsg = "手机号输入有误！";
    }
    if(password != "hello123"){
        errorCode = 604;
        errorMsg = "密码输入有误！";
    }

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = errorCode;
    jsonResp["errorMsg"] = errorMsg;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::sessionLogin(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[sessionLogin] 收到 sessionLogin 请求， requestId = "
          <<jsonReq["requestId"].toString() << "sessionId" << jsonReq["sessionId"].toString();

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] ="";

    QJsonObject resultObj;
    resultObj["isGuest"] = false;
    jsonResp["result"] = resultObj;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::logout(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[logout] 收到 logout 请求， requestId = "
          <<jsonReq["requestId"].toString() << "sessionId" << jsonReq["sessionId"].toString();

    roleType = TempUser;    // 切换到临时用户身份

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] ="";

    QJsonObject sessionObj;
    sessionObj["sessionObj"] = jsonReq["sessionId"].toString();
    jsonResp["result"] = sessionObj;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setPassword(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[setPassword] 收到 setPassword 请求， requestId = "
          <<jsonReq["requestId"].toString() << "password: " << jsonReq["password"].toString();

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] ="";

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setNickname(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG()<<"[setNickname] 收到 setNickname 请求， requestId = "
          <<jsonReq["requestId"].toString() << "nNickname: " << jsonReq["nickname"].toString();

    QJsonObject jsonResp;
    jsonResp["requestId"] = jsonReq["requestId"].toString();
    jsonResp["errorCode"] = 0;
    jsonResp["errorMsg"] ="";

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::getAdminByPhone(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[getAdminByPhone] 收到 getAdminByPhone 请求, requestId="
          << jsonReq["requestId"].toString();
    QString phoneNumber = jsonReq["phoneNumber"].toString();
    LOG()<<"管理员手机号: "<< phoneNumber;

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    int userId = 1234;
    QJsonObject userInfoJson;
    userInfoJson["userId"] = QString::number(userId);
    userInfoJson["nickname"] = "平台管理员";
    userInfoJson["roleType"] = 2; // 平台管理员
    userInfoJson["phoneNumber"] = phoneNumber;
    userInfoJson["userStatu"] = rand()%2 + 1; // 状态为1-启用 2-禁止
    userInfoJson["userMemo"] = "我是一个平台管理员，我拥有审核视频等权限";

    QJsonObject resultJson;
    resultJson["userInfo"] = userInfoJson;
    jsonBody["result"] = resultJson;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::getAdminListByStatus(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[getAdminListByStatus] 收到 getAdminListByStatus 请求, requestId="
          << jsonReq["requestId"].toString();

    int adminStatus = jsonReq["userStatus"].toInt();
    if(0 == adminStatus){
        LOG()<<"获取所有管理员信息列表";
    }else if(1 == adminStatus){
        LOG()<<"获取启用管理员信息列表";
    }else{
        LOG()<<"获取禁止管理员信息列表";
    }
    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonObject resultJson;
    resultJson["totalCount"] = 199;
    // 构造一页的管理员信息
    QJsonArray adminListJson;
    int pageCount = jsonReq["pageCount"].toInt();
    QStringList remarks = {"视频审核", "运营人员", "后台工作人员"};
    QStringList nickNames = {"张三", "李四", "王五", "赵六", "田七", "七七", "小小七"};
    int userId = 1234;
    long long phoneNumber = 17612344321;
    for(int i = 0; i < pageCount;++i){
        QJsonObject adminInfoJson;
        adminInfoJson["userId"] = QString::number(userId++);
        adminInfoJson["nickname"] = nickNames[rand()%7];
        adminInfoJson["roleType"] = (rand() % 2) + 1; // 1-超级管理员 2-平台管理员
        adminInfoJson["phoneNumber"] = QString::number(phoneNumber++);
        if(0 == adminStatus){
            adminInfoJson["userStatu"] = rand()%2 + 1; // 状态为1-启用 2-禁止
        }else{
            // 获取所有状态为禁止 或者 启用 的管理员
            adminInfoJson["userStatu"] = adminStatus;
        }
        adminInfoJson["userMemo"] = remarks[rand()%3];
        adminListJson.append(adminInfoJson);
    }
    resultJson["userList"] = adminListJson;
    jsonBody["result"] = resultJson;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(),
                                 QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::newAdministrator(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[newAdministrator] 收到 newAdministrator 请求, requestId="
          << jsonReq["requestId"].toString();

    QJsonObject adminJson = jsonReq["userInfo"].toObject();
    LOG()<<"新增管理员信息：";
    LOG()<<"nickName: "<<adminJson["nickname"].toString();

    int roleType = adminJson["roleType"].toInt();
    if(1 == roleType){
        LOG()<<"roleType: 超级管理员";
    }else if(2 == roleType){
        LOG()<<"roleType: 普通管理员";
    }

    int userStatue = adminJson["userStatu"].toInt();
    if(1 == userStatue){
        LOG()<<"userStatu: 启用";
    }else if(2 == userStatue){
        LOG()<<"userStatu: 禁止";
    }
    LOG()<<"userMemo: "<<adminJson["userMemo"].toString();
    LOG()<<"phoneNum: "<<adminJson["phoneNumber"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonObject resultJson;
    resultJson["userId"] = "1234";
    jsonBody["result"] = resultJson;

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setAdministrator(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[setAdministrator] 收到 setAdministrator 请求, requestId=" <<
        jsonReq["requestId"].toString();
    QJsonObject adminJson = jsonReq["userInfo"].toObject();
    LOG()<<"编辑管理员"<<adminJson["userId"].toString()<<"信息:";
    LOG()<<"nickName: "<<adminJson["nickname"].toString();

    int userStatus = adminJson["userStatus"].toInt();
    if(1 == userStatus){
        LOG()<<"userStatus: 启用";
    }else if(2 == userStatus){
        LOG()<<"userStatus: 禁止";
    }
    LOG()<<"userMemo: "<<adminJson["userMemo"].toString();

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::setStatus(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[setStatus] 收到 setStatus 请求, requestId=" <<
        jsonReq["requestId"].toString();

    int adminStatus = jsonReq["userStatus"].toInt();
    QString adminId = jsonReq["userId"].toString();
    QString status = "启用";
    if(adminStatus == 2){
        status = "禁用";
    }
    LOG()<<"管理员: "<<adminId<<"状态修改为"<<status;

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

QHttpServerResponse MockServer::delAdministrator(const QHttpServerRequest &req)
{
    QJsonDocument docReq = QJsonDocument::fromJson(req.body());
    const QJsonObject& jsonReq = docReq.object();
    LOG() << "[delAdministrator] 收到 delAdministrator 请求, requestId=" <<
        jsonReq["requestId"].toString();
    QString adminId = jsonReq["userId"].toString();
    LOG()<<"管理员 "<<adminId<<"删除成功";

    // 构造响应体
    QJsonObject jsonBody;
    jsonBody["requestId"] = jsonReq["requestId"].toString();
    jsonBody["errorCode"] = 0;
    jsonBody["errorMsg"] = "";

    QJsonDocument docResp;
    docResp.setObject(jsonBody);
    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}


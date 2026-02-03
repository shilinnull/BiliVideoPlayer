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

    // 下载视频
    httpServer.route("/HttpService/downloadVideo", [=](const QHttpServerRequest& req){
        return this->downloadVideo(req);
    });

    // 模拟videos⽬录下视频文件下载
    httpServer.route("/videos/<arg>", [=](const QString& request){
        return this->downloadVideoSegmentation(request);
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
    LOG() << "[tempLogin] 收到 tempLogin 请求, requestId=" << jsonReq["requ estId"].toString();

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
}

QHttpServerResponse MockServer::downloadPhoto(const QHttpServerRequest &req)
{
    QUrlQuery query(QUrl(req.url()));
    QString requestId = query.queryItemValue("requestId");
    QString fileId = query.queryItemValue("fileId");
    LOG() << "[downloadPhoto] 收到 downloadPhoto 请求, requestId=" << requestId;
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

QHttpServerResponse MockServer::downloadVideoSegmentation(const QString &fileName)
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
        roleTypeArray.append(1);
        userInfoObj["roleType"] = roleTypeArray;

        // 身份类型：0-未知，1-C端用户，2-B端用户
        // 临时用户的身份类型为空
        QJsonArray identityArray;
        identityArray.append(1);
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

    LOG() << resultObj;

    QJsonDocument docResp;
    docResp.setObject(jsonResp);

    QHttpServerResponse httpResp(docResp.toJson(), QHttpServerResponse::StatusCode::Ok);
    httpResp.setHeader("Content-Type", "application/json; charset=utf-8");
    return httpResp;
}

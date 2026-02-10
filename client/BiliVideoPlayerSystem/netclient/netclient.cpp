#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QUrl>
#include <QUrlQuery>

#include "netclient.h"
#include "../util.h"
#include "model/datacenter.h"

namespace network {

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{
}

void NetClient::setServerUrl(const QString &url)
{
    HTTP_URL = url;
}

void NetClient::tempLogin()
{
    // 1. 构造请求
    QJsonObject reqJsonObj;
    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/tempLogin", reqJsonObj);

    connect(httpReply, &QNetworkReply::finished, this, [=]{
        // 1. 解析响应
        bool ok = false;
        QString reason;
        // 处理
        QJsonObject bodyObject = handleHttpResponse(httpReply, &ok, &reason);
        // 2. 判断是否会出错
        if(!ok){
            LOG() << "tempLogin 请求出错, reason: " << reason << ", requestId=" << bodyObject["requestId"].toString();
            return ;
        }
        // 3. 解析sessionId
        QJsonObject sessionJson = bodyObject["result"].toObject();
        auto dataCenter = model::DataCenter::getInstance();
        dataCenter->setSessionId(sessionJson["sessionId"].toString());
        LOG() << "tempLogin	登录成功, requestId=" << bodyObject["requestId"].toString();
        emit dataCenter->loginTempUserDone();
    });

}

void NetClient::getAllVideoList()
{
    // 1. 构造请求
    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getVideoListPtr();	// 获取视频列表的指针
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    reqJsonObj["pageIndex"] = videoListPtr->getPageIndex();
    reqJsonObj["pageCount"] = model::VideoList::PAGE_COUNT;
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);
    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/allVideoList", reqJsonObj);
    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);
        if(!ok) {
            LOG()<<"typeList请求出错: reason"<<reason;
            return ;
        }

        // 保存获取到的视频到数据中心
        dataCenter->setVideoList(resultObject["result"].toObject());
        // 发送信号，通知界面进行更新
        emit dataCenter->getAllVideoListDone();
        LOG() << "allVideoList 请求结束，获取视频列表成功, requestId: " << resultObject["requestId"].toString();
    });
}

void NetClient::getAllVideosInKind(int kindId)
{
    // 1. 构造请求
    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getVideoListPtr();	// 获取视频列表的指针
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    reqJsonObj["videoTypeId"] = kindId;			// 添加Id
    reqJsonObj["pageIndex"] = videoListPtr->getPageIndex();
    reqJsonObj["pageCount"] = model::VideoList::PAGE_COUNT;
    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);
    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/typeVideoList", reqJsonObj);
    // 3. 异步处理响应
    connect(httpReply, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);
        if(!ok) {
            LOG()<<"typeVideoList 请求出错: reason"<<reason;
            return ;
        }

        // 保存获取到的视频到数据中心
        dataCenter->setVideoList(resultObject["result"].toObject());
        // 发送信号，通知界面进行更新
        emit dataCenter->getAllVideoInKindDone();
        LOG() << "typeVideoList 请求结束，获取视频列表成功, requestId: " << resultObject["requestId"].toString();
    });
}

void NetClient::getAllVideosInTag(int tagId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoTag"] = tagId;
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;

    videoListPtr->setPageIndex(videoListPtr->getPageIndex() + 1);

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/tagVideoList", reqBody);

    // 3. 异步处理tagVideoList请求的响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        // a. 解析Http响应
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        // b. 判定响应是否出错
        if(!ok){
            LOG()<<"tagVideoList 请求出错，reason = "<<reason;
            return;
        }

        // c. 解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = replyObj["result"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // d. 统计界面显示视频信息
        emit dataCenter->getAllVideoInTagDone();
        LOG()<<"tagVideoList 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getVideosBySearchText(const QString &searchText)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    auto videoListPtr = dataCenter->getVideoListPtr();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["searchKey"] = searchText;
    reqBody["pageIndex"] = videoListPtr->getPageIndex();
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;

    videoListPtr->setPageIndex(videoListPtr->getPageIndex()+1);

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/keyVideoList", reqBody);

    // 3. 异步处理keyVideoList请求的响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        // a. 解析Http响应
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        // b. 判定响应是否出错
        if(!ok){
            LOG()<<"keyVideoList 请求出错，reason = "<<reason;
            return;
        }

        // c. 解析响应体中服务端交给客户端的具体数据
        QJsonObject resultObj = replyObj["result"].toObject();
        // 将获取到的视频信息保存到视频列表
        dataCenter->setVideoList(resultObj);

        // d. 统计界面显示视频信息
        emit dataCenter->getAllVideoListSearchTextDone();
        LOG()<<"keyVideoList 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::downloadPhoto(const QString &photoFileId)
{
    // 1. 构造请求
    auto dataCenter = model::DataCenter::getInstance();
    QString queryStr;
    queryStr += "requestId=";
    queryStr += makeRequeId();
    queryStr += "&";
    queryStr += "sessionId=";
    queryStr += dataCenter->getLoginSessionId();
    queryStr += "&";
    queryStr += "fileId=";
    queryStr += photoFileId;

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + "/HttpService/downloadPhoto?" + queryStr));
    QNetworkReply* httpReply = httpClient.get(httpReq);

    // 3. 异步响应处理
    connect(httpReply, &QNetworkReply::finished, this, [=]{
        if(httpReply->error() != QNetworkReply::NoError) {
            LOG() << httpReply->errorString();
            httpReply->deleteLater();
            return ;
        }
        // 获取图片数据，发送信号通知界面显示
        QByteArray imageData = httpReply->readAll();
        emit dataCenter->downloadPhotoDone(photoFileId, imageData);
        httpReply->deleteLater();
    });
}

void NetClient::uploadPhoto(const QByteArray &photoData, QWidget* wndPtr)
{
    // 1. 构造请求
    auto dataCenter = model::DataCenter::getInstance();
    QString queryStr;
    queryStr += "requestId=";
    queryStr += makeRequeId();
    queryStr += "&";
    queryStr += "sessionId=";
    queryStr += dataCenter->getLoginSessionId();

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + "/HttpService/uploadPhoto?" + queryStr));
    httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/octet-stream");
    QNetworkReply* httpReply = httpClient.post(httpReq, photoData);

    // 3. 异步响应处理
    connect(httpReply, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"uploadPhoto 请求出错，reason = "<<reason;
            return;
        }
        const QJsonObject resultObj = resultObject["result"].toObject();
        const QString fileId = resultObj["fileId"].toString();
        emit dataCenter->uploadPhotoDone(fileId, wndPtr);

        const QString requestId = resultObject["requestId"].toString();
        LOG() << "uploadPhoto 请求结束, 图片上传成功, requestId= " << requestId;

        httpReply->deleteLater();
    });
}

void NetClient::downloadVideo(const QString &videoFileId)
{
    // 1. 构造请求
    QString queryString;
    queryString += "fileId=";
    queryString += videoFileId;

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + "/HttpService/downloadVideo?" + queryString));
    QNetworkReply* httpReply = httpClient.get(httpReq);

    connect(httpReply, &QNetworkReply::finished, this, [=]{
        if(httpReply->error() != QNetworkReply::NoError) {
            LOG() << httpReply->errorString();
            httpReply->deleteLater();
            return ;
        }
        QByteArray videoFileContent = httpReply->readAll(); // 读取内容

        QString videoFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
        videoFilePath += "/";
        videoFilePath += videoFileId;
        videoFilePath += ".m3u8";
        LOG() << "文件路径"<<videoFilePath;
        writeByteArrayToFile(videoFilePath, videoFileContent);  // 写入文件
        auto dataCenter = model::DataCenter::getInstance();
        emit dataCenter->downloadVideoDone(videoFilePath, videoFileId);
        LOG() << "downloadVideo 请求结束，视频下载成功";
        httpReply->deleteLater();
    });

}

void NetClient::uploadVideo(const QString &videoPath)
{
    QUrl url(HTTP_URL + "/HttpService/uploadVideo");

    // 将请求参数添加到请求url中
    auto dataCenter = model::DataCenter::getInstance();
    QUrlQuery query;
    query.addQueryItem("requestId", makeRequeId());
    query.addQueryItem("sessionId", dataCenter->getLoginSessionId());
    url.setQuery(query);

    // 设置请求对象的url
    QNetworkRequest httpReq(url);
    QFileInfo fileInfo(videoPath);
    httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data");

    // 读取视频数据
    QByteArray videoData = loadFileToByteArray(videoPath);

    // 发送请求
    QNetworkReply* httpReply = httpClient.post(httpReq, videoData);
    connect(httpReply, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"uploadVideo 请求出错，reason = "<<reason;
            httpReply->deleteLater();
            return;
        }

        const QString& requestId = resultObject["requestId"].toString();
        const QJsonObject resultObj = resultObject["result"].toObject();
        const QString& fileId = resultObj["fileId"].toString();
        emit  dataCenter->uploadVideoDone(fileId);
        LOG() << "uploadVideo 请求结束, 视频上传成功, requestId= " << requestId << ", fileId=" << fileId;
    });
}

void NetClient::uploadVideoDesc(const model::VideoDesc &videoDesc)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();

    QJsonObject videoJson;
    videoJson["videoFileId"] = videoDesc.videoId;
    videoJson["photoFileId"] = videoDesc.photoId;
    videoJson["videoTitle"] = videoDesc.title;
    videoJson["videoDesc"] = videoDesc.desc;
    videoJson["duration"] = videoDesc.duration;

    const model::KindAndTag* kindAndTag = dataCenter->getKindAndTagsClassPtr();
    if("" != videoDesc.kind) {
        // 用户没有选择分类，标签肯定也就没有
        videoJson["videoType"] = kindAndTag->getKindId(videoDesc.kind);

        QJsonArray tagArray;
        for(auto& tag : videoDesc.tags) {
            tagArray.append(kindAndTag->getTagId(videoDesc.kind, tag));
        }
        videoJson["videoTag"] = tagArray;
    }
    reqBody["videoInfo"] = videoJson;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/newVideo", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"uploadVideoDesc 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->uploadVideoDescDone();
        LOG()<<"uploadVideoDesc 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::deleteVideo(const QString &videoId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/removeVideo", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"deleteVideo 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->deleteVideoDone(videoId);
        LOG()<<"deleteVideo 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::checkVideo(const QString &videoId, bool result)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;
    reqBody["checkResult"] = result;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/checkVideo", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"checkVideo 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->checkVideoDone();
        LOG()<<"checkVideo 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::putawayVideo(const QString &videoId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/saleVideo", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"putawayVideo 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->putawayVideoDone();
        LOG()<<"putawayVideo 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::discardVideo(const QString &videoId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/haltVideo", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"haltVideo 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->discardVideoDone();
        LOG()<<"haltVideo 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::newAttention(const QString &userId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["userId"] = userId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/newAttention", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"newAttention 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->newAttentionDone(userId);
        LOG()<<"newAttention 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::delAttention(const QString &userId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["userId"] = userId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/delAttention", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"delAttention 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->delAttentionDone(userId);
        LOG()<<"delAttention 成功, resquestId = "<<resultObject["requestId"].toString();
    });
}

void NetClient::getVideoBarrage(const QString &videoId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/getBarrage", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"getBarrage 请求出错，reason = "<<reason;
            return;
        }

        QJsonObject resultObj = resultObject["result"].toObject();
        dataCenter->setBarragesData(resultObj["barrageList"].toArray());

        emit dataCenter->getVideoBarrageDone(videoId);
        LOG()<<"getBarrage 成功, resquestId = "<<resultObj["requestId"].toString();
    });
}

void NetClient::setPlayNumber(const QString &videoId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setPlay", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setPlay 请求出错，reason = "<<reason;
            return;
        }
        LOG()<<"setPlay 成功, 播放次数成功，resquestId = "<< resultObject["requestId"].toString();
   });
}

void NetClient::getIsLikeVideo(const QString &videoId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/judgeLike", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject resultObject = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"judgeLike 请求出错，reason = "<<reason;
            return;
        }
        QJsonObject result = resultObject["result"].toObject();
        emit dataCenter->getIsLikeVideoDone(videoId, result["isLike"].toBool());
        LOG()<<"judgeLike 成功，resquestId = "<< resultObject["requestId"].toString();
    });
}

void NetClient::setLikeNumber(const QString &videoId)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setLike", reqBody);

    // 3. 异步处理 setLike 请求的响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setLike 请求出错，reason = "<<reason;
            return;
        }

        LOG()<<"setLike 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::loadupBarrages(const QString &videoId, const model::BarrageInfo &barrageInfo)
{
    // 1. 构造请求体
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["videoId"] = videoId;

    QJsonObject barrageObj;
    barrageObj["barrageContent"] = barrageInfo.text;
    barrageObj["barrageTime"] = barrageInfo.playTime;
    reqBody["barrageInfo"] = barrageObj;

    // 2. 发送请求
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/newBarrage", reqBody);

    // 3. 异步处理 newBarrage 请求的响应
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"newBarrage 请求出错，reason = "<<reason;
            return;
        }

        LOG()<<"newBarrage 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getUserInfo(const QString &userId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    if(!userId.isEmpty()) {  // 获取其他用户信息
        reqBody["userId"] = userId;
    }

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/getUserInfo", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"getUserInfo 请求出错，reason = "<<reason;
            return;
        }

        // 将信息保存到dataCenter中
        QJsonObject resultObj = replyObj["result"].toObject();
        QJsonObject userInfoObj = resultObj["userInfo"].toObject();
        if(userId.isEmpty()) {
            dataCenter->setMySelfInfo(userInfoObj);
            emit dataCenter->getMyselfInfoDone();
        } else {
            dataCenter->setOtherUserInfo(userInfoObj);
            emit dataCenter->getOtherUserInfoDone();
        }

        LOG()<<"getUserInfo 成功, resquestId = "<<replyObj["requestId"].toString() << "userId: " << userId;
    });
}

void NetClient::setAvatar(const QString &fileId)
{
    // 修改头像，图片已经上传过了，只设置fileId
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["fileId"] = fileId;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setAvatar", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG() << "setAvatar 请求出错! reason=" << reason << ", requestId=" << reqBody["requestId"].toString();
            return;
        }

        dataCenter->setAvatar(fileId);
        emit dataCenter->setAvatarDone();

        LOG()<<"setAvatar 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getUserVideoList(const QString &userId, int pageIndex, const QString& whichPage)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    if(!userId.isEmpty()) {  // 获取其他用户视频列表
        reqBody["userId"] = userId;
    }
    reqBody["pageIndex"] = pageIndex;
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/userVideoList", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"userVideoList 请求出错，reason = "<<reason;
            return;
        }

        // 将信息保存到dataCenter中
        QJsonObject resultObj = replyObj["result"].toObject();
        if(whichPage == "myPage"){
            dataCenter->setUserVideoList(resultObj);
            emit dataCenter->getUserVideoListDone(userId, whichPage);
        } else if(whichPage == "checkPage"){
            dataCenter->setStatusVideoList(resultObj);
            emit dataCenter->getUserVideoListDone(userId, whichPage);
        }

        LOG()<<"userVideoList 成功, resquestId = "<<replyObj["requestId"].toString() << "userId: " << userId;
    });
}

void NetClient::getStatusVideoList(int videoStatus, int pageIndex)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["status"] = videoStatus;
    reqBody["pageIndex"] = pageIndex;
    reqBody["pageCount"] = model::VideoList::PAGE_COUNT;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/statusVideoList", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"statusVideoList 请求出错，reason = "<<reason;
            return;
        }
        // 通知界面进行更新
        QJsonObject resultObject = replyObj["result"].toObject();
        dataCenter->setStatusVideoList(resultObject);
        emit dataCenter->getStatusVideoListDone();

        LOG()<<"statusVideoList 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getAuthcode(const QString &email)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["email"] = email;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/getCode", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"getCode 请求出错，reason = "<<reason;
            return;
        }

        // 解析验证码id
        QJsonObject authcodeJson = replyObj["result"].toObject();
        QString authcodeId = authcodeJson["codeId"].toString();
        emit dataCenter->getAuthcodeDone(authcodeId);

        LOG()<<"getCode 成功, resquestId = "<<replyObj["requestId"].toString() << "codeId: " << authcodeId;
    });
}

void NetClient::loginWithEmail(const QString &email, const QString &authcode, const QString &authcodeId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["email"] = email;
    reqBody["verifyCode"] = authcode;
    reqBody["codeId"] = authcodeId;


    QNetworkReply* httpReply = sendHttpRequest("/HttpService/vcodeLogin", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"vcodeLogin 请求出错，reason = "<<reason;
            emit dataCenter->loginWithEmailFailed(reason);
            return;
        }

        emit dataCenter->loginWithEmailDone();
        LOG()<<"vcodeLogin 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::loginWithPassword(const QString &userName, const QString &password)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["username"] = userName;
    reqBody["password"] = password;


    QNetworkReply* httpReply = sendHttpRequest("/HttpService/passwdLogin", reqBody);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"passwdLogin 请求出错，reason = "<<reason;
            emit dataCenter->loginWithPasswordFailed(reason);
            return;
        }

        emit dataCenter->loginWithPasswordDone();
        LOG()<<"passwdLogin 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::loginSession()
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    LOG() << "sessionId: " << dataCenter->getLoginSessionId();
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/sessionLogin", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"loginSession 请求出错，reason = "<<reason;
            emit dataCenter->loginSessionFailed(reason);
            return;
        }

        QJsonObject jsonObj = replyObj["result"].toObject();
        bool isGuest = jsonObj["isGuest"].toBool();
        emit dataCenter->loginSessionDone(isGuest);
        LOG()<<"loginSession 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::logout()
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    QNetworkReply* httpReply = sendHttpRequest("/HttpService/logout", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"loginSession 请求出错，reason = "<<reason;
            return;
        }

        QJsonObject sessionObj = replyObj["result"].toObject();
        dataCenter->setSessionId(replyObj["sessionId"].toString());
        emit dataCenter->logoutDone();
        LOG()<<"logout 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::setPassword(const QString& newPassword)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["password"] = newPassword;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setPassword", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setPassword 请求出错，reason = "<<reason;
            return;
        }
        emit dataCenter->setPasswordDone();
        LOG()<<"setPassword 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::setNickName(const QString &nickName)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["nickname"] = nickName;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setNickname", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setNickName 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->setNickNameDone(nickName);
        LOG()<<"setNickName 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getAdminByEmail(const QString &email)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["email"] = email;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/getAdminByEmail", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"getAdminByEmail 请求出错，reason = "<<reason;
            return;
        }
        dataCenter->setAdminsList(replyObj["result"].toObject(), false);

        emit dataCenter->getAdminByEmailDone();
        LOG()<<"getAdminByEmail 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::getAdminListByStatus(int pageIndex, model::AdminStatus adminStatus)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqBody;
    reqBody["sessionId"] = dataCenter->getLoginSessionId();
    reqBody["pageIndex"] = pageIndex;
    reqBody["pageCount"] = model::AdminList::PAGE_COUNT;
    reqBody["userStatus"] = static_cast<int>(adminStatus);

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/getAdminListByStatus", reqBody);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"getAdminListByStatus 请求出错，reason = "<<reason;
            return;
        }
        dataCenter->setAdminsList(replyObj["result"].toObject(), true);

        emit dataCenter->getAdminListByStatusDone();
        LOG()<<"getAdminListByStatus 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::newAdmin(const model::AdminInfo &adminInfo)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    QJsonObject adminJson;
    adminJson["nickname"] = adminInfo.nickName;
    adminJson["roleType"] = static_cast<int>(adminInfo.roleType);
    adminJson["userStatus"] = static_cast<int>(adminInfo.userStatu);
    adminJson["userMemo"] = adminInfo.remark;
    adminJson["email"] = adminInfo.email;
    reqJsonObj["userInfo"] = adminJson;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/newAdministrator", reqJsonObj);

    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"newAdministrator 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->newAdminDone();
        LOG()<<"newAdministrator 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::editAdmin(const model::AdminInfo &userInfo)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    QJsonObject adminJson;
    adminJson["userId"] = userInfo.userId;
    adminJson["nickname"] = userInfo.nickName;
    adminJson["userStatus"] = static_cast<int>(userInfo.userStatu);
    adminJson["userMemo"] = userInfo.remark;
    reqJsonObj["userInfo"] = adminJson;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setAdministrator", reqJsonObj);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setAdministrator 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->editAdminDone();
        LOG()<<"setAdministrator 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::setAdminStatus(const model::AdminInfo &userInfo)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    reqJsonObj["userId"] = userInfo.userId;
    reqJsonObj["userStatus"] = static_cast<int>(userInfo.userStatu);

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/setStatus", reqJsonObj);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"setStatus 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->setAdminStatusDone();
        LOG()<<"setStatus 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

void NetClient::delAdmin(const QString &adminId)
{
    auto dataCenter = model::DataCenter::getInstance();
    QJsonObject reqJsonObj;
    reqJsonObj["sessionId"] = dataCenter->getLoginSessionId();
    reqJsonObj["userId"] = adminId;

    QNetworkReply* httpReply = sendHttpRequest("/HttpService/delAdministrator", reqJsonObj);
    connect(httpReply, &QNetworkReply::finished, this, [=](){
        bool ok = false;
        QString reason;
        QJsonObject replyObj = handleHttpResponse(httpReply, &ok, &reason);

        if(!ok){
            LOG()<<"delAdministrator 请求出错，reason = "<<reason;
            return;
        }

        emit dataCenter->delAdminDone();
        LOG()<<"delAdministrator 成功, resquestId = "<<replyObj["requestId"].toString();
    });
}

QString NetClient::makeRequeId()
{
    return "R" + QUuid::createUuid().toString().sliced(25, 12);
}

QNetworkReply *NetClient::sendHttpRequest(const QString &resourcePath, QJsonObject &jsonBody)
{
    QNetworkRequest httpReq;
    httpReq.setUrl(QUrl(HTTP_URL + resourcePath));
    httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf8");

    // 设置请求id
    jsonBody["requestId"] = makeRequeId();

    QJsonDocument document(jsonBody);
    QNetworkReply* httpResp = httpClient.post(httpReq, document.toJson());
    return httpResp;
}

QJsonObject NetClient::handleHttpResponse(QNetworkReply *httpResp, bool *ok, QString *reason)
{
    // 1. 判断是否会出错
    if(httpResp->error() != QNetworkReply::NoError) {
        *ok = false;
        *reason = httpResp->errorString();
        httpResp->deleteLater();
        return QJsonObject();
    }
    // 2. 获取响应的body
    QByteArray respBody = httpResp->readAll();
    // 3. 针对body进行反序列化
    QJsonDocument jsonDoc = QJsonDocument::fromJson(respBody);
    if(jsonDoc.isNull()) {
        *ok = false;
        *reason = "解析 Json 文件失败！ Json 文件格式有错误!";
        httpResp->deleteLater();
        return QJsonObject();
    }

    QJsonObject respObj = jsonDoc.object();
    // 4. 判断业务上的结果是否正确
    // 错误码为0表示没有错误
    if(0 != respObj["errorCode"].toInt()) {
        *ok = false;
        *reason = respObj["errorMsg"].toString();
        httpResp->deleteLater();
        return respObj;
    }
    *ok = true;
    httpResp->deleteLater();
    return respObj;
}



} // end netclient

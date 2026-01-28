#include "netclient.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "../util.h"
#include "../model/datacenter.h"

namespace network {

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{}

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
    auto dataCenter = model::DataCenter::getInstance();
    // 1. 构造请求
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

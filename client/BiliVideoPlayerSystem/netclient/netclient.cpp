#include "netclient.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "../util.h"

namespace netclient {

NetClient::NetClient(QObject *parent)
    : QObject{parent}
{}

void NetClient::hello()
{
    // 1. 构造请求体 body
    QJsonObject reqBody;

    // 2. 发送请求
    QNetworkReply* httpResp = sendHttpRequest("/hello", reqBody);

    // 3. 异步响应处理
    connect(httpResp, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpResp, &ok, &reason);
        if(!ok) {
            LOG() << "hello 请求出错, reason: " << reason;
            return ;
        }

        QJsonObject resoBody = respObj["data"].toObject();
        LOG() << resoBody["hello"].toString();
    });
}

void NetClient::ping()
{
    // 1. 构造请求体 body
    QJsonObject reqBody;

    // 2. 发送请求
    QNetworkReply* httpResp = sendHttpRequest("/ping", reqBody);

    // 3. 异步响应处理
    connect(httpResp, &QNetworkReply::finished, this, [=]{
        bool ok = false;
        QString reason;
        QJsonObject respObj = handleHttpResponse(httpResp, &ok, &reason);
        if(!ok) {
            LOG() << "ping 请求出错, reason: " << reason;
            return ;
        }

        QJsonObject resoBody = respObj["data"].toObject();
        LOG() << resoBody["ping"].toString();
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

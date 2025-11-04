#include "netclient.h"

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include "../util.h"

namespace netclient {

netclient::netclient(QObject *parent)
    : QObject{parent}
{}

void netclient::hello()
{
    // 1. 构造请求体 body
    QJsonObject reqBody;
    reqBody["requestId"] = makeRequeId();

    // 2. 发送请求
    QNetworkRequest httpReq;
    httpReq.setUrl(HTTP_URL + "/hello");
    httpReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf8");

    QJsonDocument document(reqBody);
    QNetworkReply* httpResp = httpClient.post(httpReq, document.toJson());

    // 3. 异步响应处理
    connect(httpResp, &QNetworkReply::finished, this, [=]{
        if(httpResp->error() != QNetworkReply::NoError) {
            LOG() << httpResp->errorString();
            httpResp->deleteLater();
            return ;
        }
        // 获取响应到的body
        QByteArray respBody = httpResp->readAll();

        // 针对body反序列化
        QJsonDocument jsonDoc = QJsonDocument::fromJson(respBody);
        if(jsonDoc.isNull()) {
            LOG() << "解析Json失败! Json文件格式有误!";
            httpResp->deleteLater();
            return ;
        }

        // 判定业务逻辑
        QJsonObject respObj = jsonDoc.object();
        if(0 != respObj["errorCode"].toInt()) {
            LOG() << respObj["errorMsg"].toString();
            httpResp->deleteLater();
            return ;
        }

        // 解析响应数据
        QJsonObject resoBody = respObj["data"].toObject();
        LOG() << resoBody["hello"].toString();
        httpResp->deleteLater();
    });
}

QString netclient::makeRequeId()
{
    return "R" + QUuid::createUuid().toString().sliced(24, 12);
}

} // end netclient

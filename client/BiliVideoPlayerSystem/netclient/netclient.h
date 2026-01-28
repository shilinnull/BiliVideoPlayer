#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>

namespace network {

class NetClient : public QObject
{
    Q_OBJECT
public:
    NetClient(QObject *parent = nullptr);
    void tempLogin();						// 临时用户登录请求
    void getAllVideoList();					// 获取所有视频
    void getAllVideosInKind(int kindId);	// 获取分类下的所有视频
    void getAllVideosInTag(int tagId);      // 获取标签下的所有视频
    void getVideosBySearchText(const QString& searchText);  // 根据文本获取视频
    void downloadPhoto(const QString& photoFileId);         // 下载图片
private:
    static QString makeRequeId();
    QNetworkReply* sendHttpRequest(const QString& resourcePath, QJsonObject& jsonBody);
    QJsonObject handleHttpResponse(QNetworkReply* httpResp, bool* ok, QString *reason);
private:
    const QString HTTP_URL = "http://127.0.0.1:8000";
    QNetworkAccessManager httpClient;
};

}	// end netclient
#endif // NETCLIENT_H

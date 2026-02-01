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
    void setServerUrl(const QString& url);         // 设置服务器地址
    void tempLogin();						// 临时用户登录请求
    void getAllVideoList();					// 获取所有视频
    void getAllVideosInKind(int kindId);	// 获取分类下的所有视频
    void getAllVideosInTag(int tagId);      // 获取标签下的所有视频
    void getVideosBySearchText(const QString& searchText);  // 根据文本获取视频
    void downloadPhoto(const QString& photoFileId);         // 下载图片
    void downloadVideo(const QString& videoFileId);         // 下载视频
    void getVideoBarrage(const QString& videoId);           // 获取弹幕
    void setPlayNumber(const QString& videoId);             // 更新播放次数
    void getIsLikeVideo(const QString& videoId);            // 检测是否点赞
    void setLikeNumber(const QString& videoId);             // 更新点赞
private:
    static QString makeRequeId();
    QNetworkReply* sendHttpRequest(const QString& resourcePath, QJsonObject& jsonBody);
    QJsonObject handleHttpResponse(QNetworkReply* httpResp, bool* ok, QString *reason);
private:
    QString HTTP_URL;
    QNetworkAccessManager httpClient;
};

}	// end netclient
#endif // NETCLIENT_H

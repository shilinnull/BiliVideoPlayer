#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include "model/data.h"

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
    void uploadPhoto(const QByteArray& photoData,
                     QWidget* wndPtr);                      // 上传图片
    void downloadVideo(const QString& videoFileId);         // 下载视频
    void uploadVideo(const QString& videoPath);             // 上传视频
    void uploadVideoDesc(const model::VideoDesc& videoDesc);// 上传视频描述信息
    void deleteVideo(const QString& videoId);               // 删除视频
    void newAttention(const QString& userId);               // 新增关注
    void delAttention(const QString& userId);               // 取消关注
    void getVideoBarrage(const QString& videoId);           // 获取弹幕
    void setPlayNumber(const QString& videoId);             // 更新播放次数
    void getIsLikeVideo(const QString& videoId);            // 检测是否点赞
    void setLikeNumber(const QString& videoId);             // 更新点赞
    void loadupBarrages(const QString& videoId,
                const model::BarrageInfo& barrageInfo);     // 新增弹幕
    void getUserInfo(const QString& userId);                // 获取用户信息
    void setAvatar(const QString& fileId);                  // 设置用户头像
    void getUserVideoList(const QString& userId,
                          int pageIndex);                   // 获取指定用户视频列表
    void getAuthcode(const QString& phoneNum);              // 获取验证码
    void loginWithMessage(const QString& phoneNum,
                          const QString& authcode,
                          const QString& authcodeId);       // 验证码登录
    void loginWithPassword(const QString& phoneNum,
                           const QString& password);        // 账号密码登录
    void loginSession();                                    // 会话登录
    void logout();                                          // 退出登录
    void setPassword(const QString& newPassword);           // 设置密码
    void setNickName(const QString& nickName);              // 修改昵称
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

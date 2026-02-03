#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

#include "data.h"
#include "../netclient/netclient.h"

namespace model {

class DataCenter : public QObject
{
    Q_OBJECT
public:
    static DataCenter* getInstance();
    QString& getServerUrl();            // 获取服务器地址
    const KindAndTag* getKindAndTagsClassPtr();		// 获取所有分类
    const QString& getLoginSessionId() const ;		// 获取sessionId
    void setSessionId(const QString& sessionId);	// 设置sessionId
    void loginTempUserAsync();						// 临时用户登录
    void getAllVideoListAsync();					// 获取所有视频列表
    void setVideoList(const QJsonObject& videoListJsonObj);     // 解析响应中的所有视频对象，放到videoList
    VideoList* getVideoListPtr();                               // 通过这个指针将视频信息更新到界面
    void getAllVideoInKindAsync(int kindId);                    // 获取分类视频列表
    void getAllVideoInTagAsync(int tagId);                      // 获取标签视频列表
    void getVideosBySearchTextAsync(const QString& searchText); // 获取搜索视频列表
    void downloadPhotoAsync(const QString& photoFileId);        // 下载图片
    void downloadVideoAsync(const QString& videoFileId);        // 下载视频
    void getVideoBarrageAsync(const QString& videoId);          // 获取弹幕
    void setBarragesData(const QJsonArray& barrageArray);       // 设置弹幕
    QHash<int64_t, QList<BarrageInfo>>& getBarragesData();      // 获取弹幕内容
    void setPlayNumberAsync(const QString& videoId);            // 更新播放数
    void getIsLikeVideoAsync(const QString& videoId);           // 检测是否点赞
    void setLikeNumberAsync(const QString &videoId);            // 更新点赞
    void loadupBarragesAsync(const QString& videoId,
                             const BarrageInfo& barrageInfo);   // 新增弹幕
    void setMySelfInfo(const QJsonObject& mySelfInfoObj);       // 设置个人信息
    const UserInfo* getMyselfInfo() const;                      // 获取个人信息
    void setOtherUserInfo(const QJsonObject& otherInfoObj);     // 设置他人信息
    UserInfo* getOtherUserInfo();                               // 获取他人信息
    void getMyselfInfoAsync();                                  // 获取用户个人信息
    void getOtherUserInfoAsync(const QString& userId);          // 获取其他用户信息
private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter* instance;
    QString serverURL = "http://127.0.0.1:8000";
    network::NetClient netClient;
    KindAndTag* kindsAndTags = nullptr;
    QString loginSessionId = "";					// 当前客户端登录到服务器会话的id
    VideoList* videoListPtr = nullptr;				// 管理首页获取的视频信息
    QHash<int64_t, QList<BarrageInfo>> barrages;    // 弹幕信息
    UserInfo* myselfInfo = nullptr;                 // 保存当前用户个人信息
    UserInfo* otherUserInfo = nullptr;              // 保存其他用户个人信息
signals:
    void loginTempUserDone();						// 临时用户登录
    void getAllVideoListDone();						// 获取所有视频信息处理完毕
    void getAllVideoInKindDone();					// 获取分类下所有视频信息处理完毕
    void getAllVideoInTagDone();					// 获取标签下所有视频信息处理完毕
    void getAllVideoListSearchTextDone();			// 根据搜索内容获取视频处理完毕
    void downloadPhotoDone(const QString& imageId,
                           QByteArray imageData);   // 下载图片处理完毕
    void downloadVideoDone(const QString& videoFilePath,
                           const QString& videoFileId); // 下载视频处理完毕
    void getVideoBarrageDone(const QString& videoId);   // 过去弹幕信息完毕
    void getIsLikeVideoDone(const QString& videoId, bool isLike);   // 检测是否点赞成功
    void getMyselfInfoDone();                                  // 获取用户个人信息完毕
    void getOtherUserInfoDone();                               // 获取其他用户信息完毕

};

}	// namespace model
#endif // DATACENTER_H

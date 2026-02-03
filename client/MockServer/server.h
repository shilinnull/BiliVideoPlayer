#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QHttpServer>
#include <QHttpServerResponse>
#include <QHttpServerRequest>

enum RoleType{
    SuperAdmin = 1, // 超级管理员
    Admin,			// 管理员
    User, 			// 用户
    TempUser		// 临时用户
};

class BarrageInfo {
public:
    QString barrageId;      // 弹幕id
    QString userId;         // 发送弹幕用户
    int64_t playTime;       // 发送弹幕时播放时间
    QString text;           // 弹幕内容

    BarrageInfo(const QString& userId = "", int64_t playTime = 0, const QString& text = "")
        : userId(userId)
        , playTime(playTime)
        , text(text)
    {}
};

class MockServer: public QObject
{
    Q_OBJECT
public:
    ~MockServer();
public:
    bool init();
    static MockServer* getInstance();

private:
    QHttpServerResponse tempLogin(const QHttpServerRequest& req);		// 临时用户登录响应
    QHttpServerResponse allVideoList(const QHttpServerRequest& req);	// 获取所有视频列表
    QHttpServerResponse typeVideoList(const QHttpServerRequest& req);	// 获取分类视频列表
    QHttpServerResponse tagVideoList(const QHttpServerRequest& req);    // 获取标签视频列表
    QHttpServerResponse keyVideoList(const QHttpServerRequest& req);    // 根据搜索文本获取视频内容
    void buildResponseData();
    QHttpServerResponse downloadPhoto(const QHttpServerRequest& req);   // 下载图片
    QHttpServerResponse downloadVideo(const QHttpServerRequest& req);   // 下载视频
    QHttpServerResponse downloadVideoSegmentation(const QString& fileName);   // 下载视频分片
    QHttpServerResponse getBarrage(const QHttpServerRequest& req);      // 获取弹幕数据
    QHttpServerResponse setPlay(const QHttpServerRequest& req);         // 更新播放次数
    QHttpServerResponse judgeLike(const QHttpServerRequest& req);       // 检测是否点赞
    QHttpServerResponse setLike(const QHttpServerRequest& req);         // 更新点赞
    QHttpServerResponse newBarrage(const QHttpServerRequest& req);      // 新增弹幕
    QHttpServerResponse getUserInfo(const QHttpServerRequest& req);     // 获取个人信息

private:
    MockServer();
    static MockServer* instance;
    QHttpServer httpServer;
    RoleType roleType = TempUser;
    QMap<QString, QString> idPathTable;
    QHash<int64_t, QList<BarrageInfo>> barrages;
};
#endif // SERVER_H

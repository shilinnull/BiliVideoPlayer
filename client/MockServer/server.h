#ifndef SERVER_H
#define SERVER_H

#include <QHttpServer>
#include <QHttpServerRequest>
#include <QHttpServerResponse>
#include <QHash>
#include <QList>
#include <QMap>
#include <QObject>
#include <QString>
#include <QTcpServer>

#include <cstdint>

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
    QHttpServerResponse uploadPhoto(const QHttpServerRequest& req);     // 上传图片
    QHttpServerResponse downloadVideo(const QHttpServerRequest& req);   // 下载视频
    QHttpServerResponse downloadVideoSeg(const QString& fileName);      // 下载视频分片
    QHttpServerResponse uploadVideo(const QHttpServerRequest& req);     // 上传视频
    QHttpServerResponse newVideo(const QHttpServerRequest& req);        // 新增视频
    QHttpServerResponse statusVideoList(const QHttpServerRequest& req); // 获取状态视频列表
    QHttpServerResponse removeVideo(const QHttpServerRequest& req);     // 删除视频
    QHttpServerResponse checkVideo(const QHttpServerRequest &req);      // 审核视频
    QHttpServerResponse saleVideo(const QHttpServerRequest &req);       // 上架视频
    QHttpServerResponse haltVideo(const QHttpServerRequest &req);       // 下架视频
    QHttpServerResponse newAttention(const QHttpServerRequest& req);    // 新增关注
    QHttpServerResponse delAttention(const QHttpServerRequest& req);    // 取消关注
    QHttpServerResponse getBarrage(const QHttpServerRequest& req);      // 获取弹幕数据
    QHttpServerResponse setPlay(const QHttpServerRequest& req);         // 更新播放次数
    QHttpServerResponse judgeLike(const QHttpServerRequest& req);       // 检测是否点赞
    QHttpServerResponse setLike(const QHttpServerRequest& req);         // 更新点赞
    QHttpServerResponse newBarrage(const QHttpServerRequest& req);      // 新增弹幕
    QHttpServerResponse getUserInfo(const QHttpServerRequest& req);     // 获取个人信息
    QHttpServerResponse setAvatar(const QHttpServerRequest& req);       // 设置用户头像
    QHttpServerResponse userVideoList(const QHttpServerRequest& req);   // 获取视频列表
    QHttpServerResponse getCode(const QHttpServerRequest& req);         // 获取验证码
    QHttpServerResponse vcodeLogin(const QHttpServerRequest& req);      // 验证码登录
    QHttpServerResponse passwdLogin(const QHttpServerRequest& req);     // 账号密码登录
    QHttpServerResponse sessionLogin(const QHttpServerRequest& req);    // 会话登录
    QHttpServerResponse logout(const QHttpServerRequest& req);          // 退出登录
    QHttpServerResponse setPassword(const QHttpServerRequest& req);     // 设置用户密码
    QHttpServerResponse setNickname(const QHttpServerRequest& req);     // 设置用户昵称
    QHttpServerResponse getAdminByEmail(const QHttpServerRequest &req); // 通过邮箱获取管理员信息
    QHttpServerResponse getAdminListByStatus(const QHttpServerRequest &req);// 通过管理员状态获取管理员信息
    QHttpServerResponse newAdministrator(const QHttpServerRequest &req);// 新增管理员
    QHttpServerResponse setAdministrator(const QHttpServerRequest &req);// 编辑管理员
    QHttpServerResponse setStatus(const QHttpServerRequest &req);       // 设置管理员状态
    QHttpServerResponse delAdministrator(const QHttpServerRequest &req);// 删除管理员
private:
    MockServer();
    static MockServer* instance;
    QHttpServer httpServer;
    QTcpServer tcpServer;
    RoleType roleType = TempUser;
    QMap<QString, QString> idPathTable;
    QHash<int64_t, QList<BarrageInfo>> barrages;
};
#endif // SERVER_H

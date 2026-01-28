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

private:
    MockServer();
    static MockServer* instance;
    QHttpServer httpServer;
    RoleType roleType = TempUser;
    QMap<QString, QString> idPathTable;
};
#endif // SERVER_H

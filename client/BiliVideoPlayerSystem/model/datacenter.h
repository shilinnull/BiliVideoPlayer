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
private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter* instance;
    network::NetClient netClient;
    KindAndTag* kindsAndTags = nullptr;
    QString loginSessionId = "";					// 当前客户端登录到服务器会话的id
    VideoList* videoListPtr = nullptr;				// 管理首页获取的视频信息
signals:
    void loginTempUserDone();						// 临时用户登录
    void getAllVideoListDone();						// 获取所有视频信息处理完毕
    void getAllVideoInKindDone();					// 获取分类下所有视频信息处理完毕
    void getAllVideoInTagDone();					// 获取标签下所有视频信息处理完毕
    void getAllVideoListSearchTextDone();			// 根据搜索内容获取视频处理完毕

};

}	// namespace model
#endif // DATACENTER_H

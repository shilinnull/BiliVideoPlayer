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
private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter* instance;
    network::NetClient netClient;
    KindAndTag* kindsAndTags = nullptr;
    QString loginSessionId = "";					// 当前客户端登录到服务器会话的id
signals:
    void loginTempUserDone();						// 临时用户登录
};

}	// namespace model
#endif // DATACENTER_H

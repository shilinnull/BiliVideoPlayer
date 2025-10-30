#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

#include "data.h"

namespace model {

class DataCenter : public QObject
{
    Q_OBJECT
public:
    static DataCenter* getInstance();
    const KindAndTag* getKindAndTagsClassPtr();		// 获取所有分类
private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter* instance;
    KindAndTag* kindsAndTags = nullptr;
signals:
};

}	// namespace model
#endif // DATACENTER_H

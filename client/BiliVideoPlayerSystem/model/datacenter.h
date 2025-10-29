#ifndef DATACENTER_H
#define DATACENTER_H

#include <QObject>

namespace model {

class DataCenter : public QObject
{
    Q_OBJECT
public:
    static DataCenter* getInstance();

private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter* instance;
signals:
};

}	// namespace model
#endif // DATACENTER_H

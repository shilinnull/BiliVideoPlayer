#include "datacenter.h"

namespace model {

DataCenter* DataCenter::instance = nullptr;

DataCenter *DataCenter::getInstance()
{
    if(instance == nullptr) {
        instance = new DataCenter();
    }
    return instance;
}

const KindAndTag *DataCenter::getKindAndTagsClassPtr()
{
    if(nullptr == kindsAndTags) {
        kindsAndTags = new KindAndTag();
    }
    return kindsAndTags;
}

const QString &DataCenter::getLoginSessionId() const
{
    return loginSessionId;
}

void DataCenter::setSessionId(const QString &sessionId)
{
    loginSessionId = sessionId;
}

void DataCenter::loginTempUserAsync()
{
    netClient.tempLogin();
}

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{}
}	// namespace model

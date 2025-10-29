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

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{}
}	// namespace model

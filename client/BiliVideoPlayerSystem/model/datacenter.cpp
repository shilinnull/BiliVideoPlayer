#include "datacenter.h"

#include <QJsonArray>

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

void DataCenter::getAllVideoListAsync()
{
    netClient.getAllVideoList();		// 从网络中获取请求
}

void DataCenter::setVideoList(const QJsonObject &videoListJsonObj)
{
    // 先创建对象
    getVideoListPtr();
    // 获取视频列表
    QJsonArray videoListArray = videoListJsonObj["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); i++) {
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);

        // 添加
        videoListPtr->addVideo(videoInfo);
    }
    videoListPtr->setVideoTotalCount(videoListJsonObj["totalCount"].toInt());
}

VideoList *DataCenter::getVideoListPtr()
{
    if(nullptr == videoListPtr)
        videoListPtr = new VideoList();
    return videoListPtr;
}

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{}
}	// namespace model

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

QString &DataCenter::getServerUrl()
{
    return serverURL;
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

void DataCenter::getAllVideoInKindAsync(int kindId)
{
    netClient.getAllVideosInKind(kindId);
}

void DataCenter::getAllVideoInTagAsync(int tagId)
{
    netClient.getAllVideosInTag(tagId);
}

void DataCenter::getVideosBySearchTextAsync(const QString &searchText)
{
    netClient.getVideosBySearchText(searchText);
}

void DataCenter::downloadPhotoAsync(const QString &photoFileId)
{
    netClient.downloadPhoto(photoFileId);
}

void DataCenter::downloadVideoAsync(const QString &videoFileId)
{
    netClient.downloadVideo(videoFileId);
}

void DataCenter::getVideoBarrageAsync(const QString &videoId)
{
    netClient.getVideoBarrage(videoId);
}

void DataCenter::setBarragesData(const QJsonArray &barrageArray)
{
    barrages.clear();   // 清空之前的
    QList<BarrageInfo> barrageList;
    for(int i = 0; i < barrageArray.size(); i++) {
        QJsonObject barrageObj = barrageArray[i].toObject();
        BarrageInfo barrageInfo; barrageInfo.loadBarrageInfo(barrageObj);
        // 该条弹幕是该时间下的第一条弹幕
        //此时将上个时间点的弹幕先保存起来，然后处理新的弹幕
        if(!barrageList.isEmpty() && barrageList[0].playTime != barrageInfo.playTime) {
            barrages.insert(barrageList[0].playTime, barrageList);
            barrageList.clear();
        }
        barrageList.append(barrageInfo);
    }
    if(!barrageList.isEmpty())
        barrages.insert(barrageList[0].playTime, barrageList);
}

QHash<int64_t, QList<BarrageInfo>> &DataCenter::getBarragesData()
{
    return barrages;
}

void DataCenter::setPlayNumberAsync(const QString &videoId)
{
    netClient.setPlayNumber(videoId);
}

void DataCenter::getIsLikeVideoAsync(const QString &videoId)
{
    netClient.getIsLikeVideo(videoId);
}

void DataCenter::setLikeNumberAsync(const QString &videoId)
{
    netClient.setLikeNumber(videoId);
}

void DataCenter::loadupBarragesAsync(const QString &videoId, const BarrageInfo &barrageInfo)
{
    netClient.loadupBarrages(videoId, barrageInfo);
}

void DataCenter::setMySelfInfo(const QJsonObject &mySelfInfoObj)
{
    if(this->myselfInfo == nullptr) {
        this->myselfInfo = new UserInfo;
    }
    this->myselfInfo->loadUserInfo(mySelfInfoObj);
}

const UserInfo *DataCenter::getMyselfInfo() const
{
    return myselfInfo;
}

void DataCenter::setOtherUserInfo(const QJsonObject &otherInfoObj)
{
    if(this->otherUserInfo == nullptr) {
        this->otherUserInfo = new UserInfo;
    }
    this->otherUserInfo->loadUserInfo(otherInfoObj);
}

UserInfo *DataCenter::getOtherUserInfo()
{
    return otherUserInfo;
}

void DataCenter::getMyselfInfoAsync()
{
    // 不传 userId, 就是获取自己信息，服务端可以通过session拿到当前登录用户的id
    netClient.getUserInfo("");
}

void DataCenter::getOtherUserInfoAsync(const QString& userId)
{
    netClient.getUserInfo(userId);
}

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{
    netClient.setServerUrl(serverURL);
}
}	// namespace model

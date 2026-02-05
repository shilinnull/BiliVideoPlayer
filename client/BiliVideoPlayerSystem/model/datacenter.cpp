#include "datacenter.h"

#include <QJsonArray>
#include "util.h"

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

void DataCenter::uploadPhotoAsync(const QByteArray &photoData)
{
    netClient.uploadPhoto(photoData);
}

void DataCenter::downloadVideoAsync(const QString &videoFileId)
{
    netClient.downloadVideo(videoFileId);
}

void DataCenter::deleteVideoAsync(const QString &videoId)
{
    netClient.deleteVideo(videoId);
}

void DataCenter::newAttentionAsync(const QString &userId)
{
    netClient.newAttention(userId);
}

void DataCenter::delAttentionAsync(const QString &userId)
{
    netClient.delAttention(userId);
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

void DataCenter::buildTempUserInfo()
{
    if(this->myselfInfo == nullptr) {
        this->myselfInfo = new UserInfo;
    }
    myselfInfo->buildTempUser();
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

void DataCenter::setAvatar(const QString &fileId)
{
    myselfInfo->avatarFileId = fileId;
}

void DataCenter::setAvatarAsync(const QString &fileId)
{
    netClient.setAvatar(fileId);
}

void DataCenter::setUserVideoList(const QJsonObject &videoListObj)
{
    getUserVideoList();
    QJsonArray videoListArray = videoListObj["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); i++) {
        // 解析单个视频
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);

        // 视频信息放到用户视频列表中
        userVideoList->videoInfos.append(videoInfo);
    }
    int videoTotalCount = videoListObj["totalCount"].toInt();
    LOG() << "视频总个数: " << videoTotalCount;
    userVideoList->setVideoTotalCount(videoTotalCount);

    // 如果本次未获取到视频，说明后续已经没有视频了，页面索引不需要+1
    if(0 == videoListArray.size()) {
        userVideoList->setPageIndex(userVideoList->getPageIndex() - 1);
    }
}

VideoList *DataCenter::getUserVideoList()
{
    if(nullptr == userVideoList) {
        userVideoList = new VideoList();
    }
    return userVideoList;
}

void DataCenter::getUserVideoListAsync(const QString &userId, int pageIndex)
{
    netClient.getUserVideoList(userId, pageIndex);
}

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{
    netClient.setServerUrl(serverURL);
}
}	// namespace model

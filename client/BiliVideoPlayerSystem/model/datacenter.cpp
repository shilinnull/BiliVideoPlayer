#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>

#include "datacenter.h"
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

void DataCenter::initDataFile()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/BiliPlayer.json";
    LOG() << "initDataFile:" << filePath;
    QDir dir;
    // 路径不存在就创建
    if(!dir.exists(basePath)) {
        dir.mkpath(basePath);
    }
    // 写方式打开
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG() << "打开文件失败: " << file.errorString();
        return ;
    }
    // 写入空数据
    QString data = "{\n\n}";
    file.write(data.toUtf8());
    file.close();
}

void DataCenter::saveDataFile()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/BiliPlayer.json";
    LOG() << "saveDataFile: " << filePath;
    // 写方式打开
    QFile file(filePath);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        LOG() << "打开文件失败: " << file.errorString();
        return ;
    }

    // 构造json数据
    QJsonObject jsonObj;
    jsonObj["loginSessionId"] = loginSessionId;

    QJsonArray roleTypeArray;
    for(auto& idType : myselfInfo->roleType) {
        roleTypeArray.append(idType);
    }
    jsonObj["roleType"] = roleTypeArray;

    QJsonArray identityTypeArray;
    for(auto& idType : myselfInfo->identityType) {
        identityTypeArray.append(idType);
    }
    jsonObj["identityType"] = identityTypeArray;

    // 写入文件
    QJsonDocument jsonDoc(jsonObj);
    QString s = jsonDoc.toJson();
    file.write(s.toUtf8());
    LOG() << "保存session文件成功";
    file.close();
}

void DataCenter::loadDataFile()
{
    QString basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString filePath = basePath + "/BiliPlayer.json";
    LOG() << "loadDataFile: " << filePath;
    // 查看文件是否存在
    QFile fileInfo(filePath);
    if(!fileInfo.exists()) {
        initDataFile();
    }

    // 读方式打开
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG() << "打开文件失败: " << file.errorString();
        return ;
    }

    // 读文件
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll());
    if(jsonDoc.isNull()) {
        LOG() << "json文件解析失败";
        file.close();
        return ;
    }

    file.close();

    // 读入内存
    QJsonObject jsonObj = jsonDoc.object();
    this->loginSessionId = jsonObj["loginSessionId"].toString();
    if(myselfInfo == nullptr) {
        myselfInfo = new UserInfo();
    }

    QJsonArray roleTypeArray = jsonObj["roleType"].toArray();
    for(int i = 0; i < roleTypeArray.count(); i++) {
        myselfInfo->roleType.append(roleTypeArray[i].toInt());
    }

    QJsonArray identityTypeArray = jsonObj["identityType"].toArray();
    for(int i = 0; i < identityTypeArray.count(); i++) {
        myselfInfo->identityType.append(identityTypeArray[i].toInt());
    }
    LOG() << "读入内存的数据:" << jsonObj;
}

const QString &DataCenter::getServerUrl() const
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
    if(videoListArray.isEmpty()) {
        videoListPtr->setPageIndex(videoListPtr->getPageIndex() - 1);
    }
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

void DataCenter::uploadPhotoAsync(const QByteArray &photoData, QWidget* wndPtr)
{
    netClient.uploadPhoto(photoData, wndPtr);
}

void DataCenter::downloadVideoAsync(const QString &videoFileId)
{
    netClient.downloadVideo(videoFileId);
}

void DataCenter::uploadVideoAsync(const QString &videoPath)
{
    netClient.uploadVideo(videoPath);

}

void DataCenter::uploadVideoDescAsync(const model::VideoDesc &videoDesc)
{
    netClient.uploadVideoDesc(videoDesc);
}

void DataCenter::deleteVideoAsync(const QString &videoId)
{
    netClient.deleteVideo(videoId);
}

void DataCenter::checkVideoAsync(const QString &videoId, bool result)
{
    netClient.checkVideo(videoId, result);
}

void DataCenter::putawayVideoAsync(const QString &videoId)
{
    netClient.putawayVideo(videoId);
}

void DataCenter::discardVideoAsync(const QString &videoId)
{
    netClient.discardVideo(videoId);
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

UserInfo *DataCenter::getMyselfInfo()
{
    return myselfInfo;
}

void DataCenter::clearUserInfo()
{
    if(myselfInfo) {
        delete myselfInfo;
        myselfInfo = nullptr;
    }
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

void DataCenter::setStatusVideoList(const QJsonObject &videoListObj)
{
    getStatusVideoList();
    // 设置总数页
    QJsonArray videoListArray = videoListObj["videoList"].toArray();
    for(int i = 0; i < videoListArray.size(); i++) {
        QJsonObject videoInfoObj = videoListArray[i].toObject();
        VideoInfo videoInfo;
        videoInfo.loadVideoInfo(videoInfoObj);
        statusVideoList->videoInfos.push_back(videoInfo);
    }
    statusVideoList->setVideoTotalCount(videoListObj["totalCount"].toInt());
    LOG() << "状态视频列表总共有：" << statusVideoList->getVideoCount();
}

VideoList *DataCenter::getStatusVideoList()
{
    if(nullptr == statusVideoList) {
        statusVideoList = new VideoList();
    }
    return statusVideoList;
}

void DataCenter::getUserVideoListAsync(const QString &userId, int pageIndex, const QString& whichPage)
{
    netClient.getUserVideoList(userId, pageIndex, whichPage);
}

void DataCenter::getStatusVideoListAsync(int videoStatus, int pageIndex)
{
    netClient.getStatusVideoList(videoStatus, pageIndex);
}

void DataCenter::getAuthcodeAsync(const QString &emial)
{
    netClient.getAuthcode(emial);
}

void DataCenter::loginWithEmailAsync(const QString &email, const QString &authcode, const QString &authcodeId)
{
    netClient.loginWithEmail(email, authcode, authcodeId);
}

void DataCenter::loginWithPasswordAsync(const QString &userName, const QString &password)
{
    netClient.loginWithPassword(userName, password);
}

void DataCenter::loginSessionAsync()
{
    netClient.loginSession();
}

void DataCenter::logoutAsync()
{
    netClient.logout();
}

void DataCenter::setPasswordAsync(const QString &password)
{
    netClient.setPassword(password);
}

void DataCenter::setNickNameAsync(const QString &nickName)
{
    netClient.setNickName(nickName);
}

void DataCenter::setAdminsList(const QJsonObject &adminsJson, bool isAdminStatus)
{
    getAdminsList();
    if(isAdminStatus){
        // 通过状态获取管理员列表
        // 设置总的页数
        int totalCount = adminsJson["totalCount"].toInt();
        adminListPtr->totalCount = totalCount;
        QJsonArray adminListArray = adminsJson["userList"].toArray();
        for(int i = 0; i < adminListArray.size(); i++) {
            QJsonObject adminInfoObj = adminListArray[i].toObject();
            AdminInfo adminInfo;
            adminInfo.loadAdminInfo(adminInfoObj);
            adminListPtr->addAdminInfo(adminInfo);
        }
    } else {
        // 通过邮箱获取管理员信息，目前只有一个管理员信息
        AdminInfo adminInfo;
        adminInfo.loadAdminInfo(adminsJson["userInfo"].toObject());
        adminListPtr->addAdminInfo(adminInfo);
        adminListPtr->totalCount = 1;
    }
}

AdminList *DataCenter::getAdminsList()
{
    if (adminListPtr == nullptr) {
        adminListPtr = new AdminList();
    }
    return adminListPtr;
}

void DataCenter::getAdminByEmailAsync(const QString &email)
{
    netClient.getAdminByEmail(email);
}

void DataCenter::getAdminListByStatusAsync(int pageIndex, AdminStatus adminStatus)
{
    netClient.getAdminListByStatus(pageIndex, adminStatus);
}

void DataCenter::newAdminAsync(const AdminInfo &userInfo)
{
    netClient.newAdmin(userInfo);
}

void DataCenter::editAdminAsync(const AdminInfo &userInfo)
{
    netClient.editAdmin(userInfo);
}

void DataCenter::setAdminStatusAsync(const AdminInfo &userInfo)
{
    netClient.setAdminStatus(userInfo);
}

void DataCenter::delAdminAsync(const QString &adminId)
{
    netClient.delAdmin(adminId);
}

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{
    netClient.setServerUrl(serverURL);
    loadDataFile(); // 加载session文件数据
}

DataCenter::~DataCenter()
{
}
}	// namespace model

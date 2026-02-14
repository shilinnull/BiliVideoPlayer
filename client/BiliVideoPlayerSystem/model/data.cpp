#include <QJsonArray>

#include "data.h"

namespace model{

int KindAndTag::id = 10000;

KindAndTag::KindAndTag()
{
    QList<QString> kinds = {"历史", "美食", "游戏", "科技", "运动", "动物", "旅行", "电影"};
    for(auto& kind : kinds) {
        kindIds.insert(kind, id++);
    }

    QHash<QString, QList<QString>> kindsAndTags = {
        {"历史", {"中国史", "世界史", "历史人物", "艺术", "文化", "奇闻"}},
        {"美食", {"美食测评", "美食制作", "美食攻略", "美食记录", "探店", "水果", "海鲜"}},
        {"游戏", {"游戏攻略", "单机游戏", "电子竞技", "手机游戏", "网络游戏", "游戏赛事","桌游棋牌"}},
        {"科技", {"数码", "软件应用", "智能家居", "手机", "电脑", "人工智能", "基础设施"}},
        {"运动", {"篮球", "足球", "乒乓球", "羽毛球", "健身", "竞技体育", "运动装备"}},
        {"动物", {"喵星人", "汪星人", "宠物知识", "动物资讯", "野生动物", "动物世界", "萌宠"}},
        {"旅行", {"旅游攻略", "旅行Vlog", "自驾游", "交通", "环球旅行", "露营", "野外生存"}},
        {"电影", {"电影解说", "电影推荐", "电影剪辑", "搞笑", "吐槽", "悬疑", "经典"}}
    };

    // 构建分类下的各个标签以及对应id
    for(auto& kind : kinds) {
        QList<QString>& tags = kindsAndTags[kind];
        QHash<QString, int> tagIdsOfKind;
        for(auto& tag : tags) {
            tagIdsOfKind.insert(tag, id++);
        }
        tagIds.insert(kind, tagIdsOfKind);
    }
}

KindAndTag::~KindAndTag()
{
    delete kindsAndTags;
}

const QList<QString> KindAndTag::getAllKinds() const
{
    return kindIds.keys();
}

const QHash<QString, int> KindAndTag::getTagsByKind(QString kind) const
{
    return tagIds[kind];
}

int KindAndTag::getKindId(QString kind) const
{
    return kindIds[kind];
}

int KindAndTag::getTagId(QString kind, QString tag) const
{
    return tagIds[kind][tag];
}

const KindAndTag *KindAndTag::getKindAndTagsClassPtr()
{
    if(nullptr == kindsAndTags)
        kindsAndTags = new KindAndTag();
    return kindsAndTags;
}

void VideoInfo::loadVideoInfo(const QJsonObject &jsonObj)
{
    videoId = jsonObj.value("videoId").toString();
    userId = jsonObj.value("userId").toString();
    userAvatarId = jsonObj.value("userAvatarId").toString();
    nickName = jsonObj.value("nickname").toString();
    videoFileId = jsonObj.value("videoFileId").toString();
    photoFileId = jsonObj.value("photoFileId").toString();
    likeCount = jsonObj.value("likeCount").toInt();
    playCount = jsonObj.value("playCount").toInt();
    videoSize = jsonObj.value("videoSize").toInt();
    videoDesc = jsonObj.value("videoDesc").toString();
    videoTitle = jsonObj.value("videoTitle").toString();
    videoDuration = jsonObj.value("videoDuration").toInt();
    videoUpTime = jsonObj.value("videoUpTime").toString();

    videoStatus = static_cast<VideoStatus>(jsonObj.value("videoStatus").toInt());
    checkerId = jsonObj.value("checkerId").toString();
    checkerName = jsonObj.value("checkerName").toString();
    checkerAvatar = jsonObj.value("checkerAvatar").toString();
}

VideoList::VideoList()
    : pageIndex(1)
    , videoTotalCount(0)
{
}

void VideoList::setPageIndex(int pageIndex)
{
    this->pageIndex = pageIndex;
}

int VideoList::getPageIndex() const
{
    return pageIndex;
}

int VideoList::getVideoCount() const
{
    return videoInfos.size();
}

void VideoList::setVideoTotalCount(int videoTotalCount)
{
    this->videoTotalCount = videoTotalCount;
}

int VideoList::getVideoTotalCount() const
{
    return videoTotalCount;
}

void VideoList::addVideo(const VideoInfo &videoInfo)
{
    videoInfos.append(videoInfo);
}

const QList<VideoInfo> &VideoList::getVideoList() const
{
    return videoInfos;
}

void VideoList::clearVideoList()
{
    videoInfos.clear();
    videoTotalCount = 0;
    pageIndex = 1;
}

void VideoList::incrementPlayNum(const QString& videoId)
{
    for(auto& videoInfo : videoInfos) {
        if(videoInfo.videoId == videoId) {
            videoInfo.playCount++;
            return ;
        }
    }
}

void VideoList::updateLikeNum(const QString &videoId, int64_t likeCount)
{
    for(auto& videoInfo : videoInfos) {
        if(videoInfo.videoId == videoId) {
            videoInfo.likeCount = likeCount;
            return ;
        }
    }
}

void VideoList::updateVideoCheckInfo(const QString &videoId, VideoStatus videoStatus,
                                     const QString &nickname, const QString &checkerId,
                                     const QString &checkerAvatarId)
{
    for(auto& videoInfo : videoInfos){
        if(videoInfo.videoId == videoId){
            videoInfo.videoStatus = videoStatus;
            videoInfo.checkerId = checkerId;
            videoInfo.checkerName = nickname;
            videoInfo.checkerAvatar = checkerAvatarId;
            return;
        }
    }
}

void BarrageInfo::loadBarrageInfo(const QJsonObject &barrageJson)
{
    barrageId = barrageJson["barrageId"].toString();
    userId = barrageJson["userId"].toString();
    playTime = barrageJson["barrageTime"].toInt();
    text = barrageJson["barrageContent"].toString();
}

void UserInfo::loadUserInfo(const QJsonObject &jsonObj)
{
    userId = jsonObj["userId"].toString();
    email = jsonObj["email"].toString();
    nickname = jsonObj["nickname"].toString();
    roleType.clear();
    identityType.clear();
    // ⻆⾊类型：超级管理员-1、普通管理员-2、普通用户-3，临时用户-4
    QJsonArray roleTypeArray = jsonObj["roleType"].toArray();
    for(int i = 0; i < roleTypeArray.count(); i++) {
        roleType.append(roleTypeArray[i].toInt());
    }
    // ⾝份类型：C端用户 B端用户
    QJsonArray identityTypeArray = jsonObj["identityType"].toArray();
    for(int i = 0; i < identityTypeArray.count(); i++) {
        identityType.append(identityTypeArray[i].toInt());
    }

    likeCount = jsonObj["likeCount"].toInt();
    playCount = jsonObj["playCount"].toInt();
    followedCount = jsonObj["followedCount"].toInt();
    followerCount = jsonObj["followerCount"].toInt();
    userStatus = jsonObj["userStatus"].toInt();
    isFollowing = jsonObj["isFollowing"].toBool();
    userMemo = jsonObj["userMemo"].toString();
    userCTime = jsonObj["userCTime"].toString();
    avatarFileId = jsonObj["avatarFileId"].toString();
}

bool UserInfo::isBUser() const
{
    for(auto idType : identityType){
        if(idType == BUser){
            return true;
        }
    }

    return false;
}

bool UserInfo::isTempUser() const
{
    for(auto role : roleType){
        if(role == TempUser){
            return true;
        }
    }
    return false;
}

void UserInfo::buildTempUser()
{
    userId = "";
    email = "";
    nickname = "临时用户";
    roleType.clear();
    identityType.clear();
    roleType.append(TempUser);
    identityType.append(CUser);
    likeCount = 0;
    playCount = 0;
    followedCount = 0;
    followerCount = 0;
    userStatus = 0;
    isFollowing = false;
    userMemo = "";
    userCTime = "";
    avatarFileId = "";
}

bool UserInfo::isAdminDisable() const
{
    return AdminStatus::Disable == userStatus;
}

void AdminInfo::loadAdminInfo(const QJsonObject &jsonObj)
{
    userId = jsonObj["userId"].toString();
    nickName = jsonObj["nickname"].toString();
    roleType = static_cast<RoleType>(jsonObj["roleType"].toInt());
    email = jsonObj["email"].toString();
    userStatu = static_cast<AdminStatus>(jsonObj["userStatus"].toInt());
    remark = jsonObj["userMemo"].toString();
}

void AdminList::addAdminInfo(const AdminInfo &adminInfo)
{
    adminList.append(adminInfo);
}

void AdminList::setAdminStatus(const QString &userId, AdminStatus adminStatus)
{
    for(auto& adminInfo : adminList){
        if(adminInfo.userId == userId){
            adminInfo.userStatu = adminStatus;
            return;
        }
    }
}

}

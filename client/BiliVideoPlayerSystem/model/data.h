#ifndef DATA_H
#define DATA_H

#include <QString>
#include <QList>
#include <QHash>
#include <QJsonObject>

namespace model{

// 视频状态
enum VideoStatus {
    noStatus = 0,   // 无状态
    waitForChecking,// 待审核
    putaway,        // 审核通过 or 上架
    reject,         // 审核驳回
    discard         // 已下架
};

// 视频信息结构
class VideoInfo{
public:
    QString videoId;    		// 视频Id
    QString userId;    			// 用户Id
    QString userAvatarId;    	// 用户图像Id
    QString nickName;    		// 用户昵称
    QString videoFileId;    	// 视频文件Id
    QString photoFileId;    	// 视频封面Id
    int64_t likeCount;    		// 点赞量
    int64_t playCount;    		// 播放量
    int64_t videoSize;    		// 视频大小
    QString videoDesc;    		// 视频描述信息
    QString videoTitle;    		// 视频标题
    int64_t videoDuration;    	// 持续时长
    QString videoUpTime;    	// 视频上传时间

    int videoStatus;            // 视频状态
    QString checkerId;          // 审核者ID
    QString checkerName;        // 审核者昵称
    QString checkerAvatar;      // 审核者用户头像ID

    // 通过 JSON 对象加载视频信息
    void loadVideoInfo(const QJsonObject& jsonObj);
};

// 视频列表
class VideoList{
public:
    VideoList();
    // 设置或者获取下一次要获取的视频页号
    void setPageIndex(int pageIndex);
    int getPageIndex() const ;

    // 获取视频列表中的视频个数
    int getVideoCount() const;
    // 设置或者特定条件下总视频的个数，视频审核页面用来计算分页器上总页数
    void setVideoTotalCount(int videoTotalCount);
    int getVideoTotalCount() const ;
    // 视频列表中添加视频
    void addVideo(const VideoInfo& videoInfo);
    // 获取排序后的视频列表
    const QList<VideoInfo>& getVideoList() const ;
    // 清空视频列表
    void clearVideoList();
    // 增加播放量
    void incrementPlayNum(const QString& videoId);
    // 更新播放数
    void updateLikeNum(const QString& videoId, int64_t likeCount);


    QList<VideoInfo> videoInfos;	// 从服务器上获取下来的视频数据
    int64_t pageIndex;				// 页面索引
    int64_t videoTotalCount;		// 当前模式下的视频个数
    const static int PAGE_COUNT = 20;
};

class KindAndTag {
public:
    KindAndTag();
    ~KindAndTag();
    const QList<QString> getAllKinds() const; 						// 获取所有分类
    const QHash<QString, int> getTagsByKind(QString kind) const;	// 获取一个分类下的所有id
    int getKindId(QString kind) const;								// 获取分类的id
    int getTagId(QString kind, QString tag) const;					// 获取kind分类下包含的tagid
    const KindAndTag* getKindAndTagsClassPtr();						// 获取所有分类

private:
    QHash<QString, int> kindIds;						// 分类名称, 分类id
    QHash<QString, QHash<QString, int>> tagIds;			// 分类名称, <标签名称, 标签id>
    KindAndTag* kindsAndTags = nullptr;					// 分类和标签实例指针
    static int id;
};



// 弹幕信息
class BarrageInfo {
public:
    QString barrageId;      // 弹幕id
    QString userId;         // 发送弹幕用户
    int64_t playTime;       // 发送弹幕时播放时间
    QString text;           // 弹幕内容

    void loadBarrageInfo(QJsonObject& barrageJson);
};

enum RoleTye{
    SuperAdmin = 1,
    Admid,
    User,
    TempUser
};

enum IdentityType{
    CUser = 1,      // C端用户
    BUser           // B端用户
};

class UserInfo{
public:
    QString userId;
    QString phoneNum;
    QString nickname;
    QList<int> roleType;
    QList<int> identityType;
    int64_t likeCount;
    int64_t playCount;
    int64_t followedCount;
    int64_t followerCount;
    int userStatus;
    int isFollowing;
    QString userMemo;
    QString userCTime;
    QString avatarFileId;

    void loadUserInfo(const QJsonObject& jsonObj);
    bool isBUser() const ;
    bool isTempUser() const;
};


};

#endif // DATA_H

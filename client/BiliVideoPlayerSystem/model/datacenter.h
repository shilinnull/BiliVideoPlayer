#ifndef DATACENTER_H
#define DATACENTER_H

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QString>

#include "../netclient/netclient.h"
#include "data.h"

namespace model {

class DataCenter : public QObject
{
    Q_OBJECT
public:
    static DataCenter* getInstance();
    void initDataFile();                // 初始化数据文件
    void saveDataFile();                // 保存数据文件
    void loadDataFile();                // 从文件加载数据
    const QString& getServerUrl() const;            // 获取服务器地址
    const KindAndTag* getKindAndTagsClassPtr();		// 获取所有分类
    const QString& getLoginSessionId() const ;		// 获取sessionId
    void setSessionId(const QString& sessionId);	// 设置sessionId
    void loginTempUserAsync();						// 临时用户登录
    void getAllVideoListAsync();					// 获取所有视频列表
    void setVideoList(const QJsonObject& videoListJsonObj);     // 解析响应中的所有视频对象，放到videoList
    VideoList* getVideoListPtr();                               // 通过这个指针将视频信息更新到界面
    void getAllVideoInKindAsync(int kindId);                    // 获取分类视频列表
    void getAllVideoInTagAsync(int tagId);                      // 获取标签视频列表
    void getVideosBySearchTextAsync(const QString& searchText); // 获取搜索视频列表
    void downloadPhotoAsync(const QString& photoFileId);        // 下载图片
    void uploadPhotoAsync(const QByteArray& photoData,
                          QWidget* wndPtr = nullptr);           // 上传图片
    void downloadVideoAsync(const QString& videoFileId);        // 下载视频
    void uploadVideoAsync(const QString& videoPath);            // 上传视频
    void uploadVideoDescAsync(const model::VideoDesc& videoDesc);// 上传视频描述信息
    void deleteVideoAsync(const QString& videoId);              // 删除视频
    void checkVideoAsync(const QString& videoId, bool result);  // 视频审核
    void putawayVideoAsync(const QString& videoId);             // 上架视频
    void discardVideoAsync(const QString& videoId);             // 下架视频
    void newAttentionAsync(const QString& userId);              // 新增关注
    void delAttentionAsync(const QString& userId);              // 取消关注
    void getVideoBarrageAsync(const QString& videoId);          // 获取弹幕
    void setBarragesData(const QJsonArray& barrageArray);       // 设置弹幕
    QHash<int64_t, QList<BarrageInfo>>& getBarragesData();      // 获取弹幕内容
    void setPlayNumberAsync(const QString& videoId);            // 更新播放数
    void getIsLikeVideoAsync(const QString& videoId);           // 检测是否点赞
    void setLikeNumberAsync(const QString &videoId);            // 更新点赞
    void loadupBarragesAsync(const QString& videoId,
                             const BarrageInfo& barrageInfo);   // 新增弹幕
    void setMySelfInfo(const QJsonObject& mySelfInfoObj);       // 设置个人信息
    UserInfo* getMyselfInfo();                                  // 获取个人信息
    void clearUserInfo();                                       // 清除用户信息
    void buildTempUserInfo();                                   // 构建临时用户
    void setOtherUserInfo(const QJsonObject& otherInfoObj);     // 设置他人信息
    UserInfo* getOtherUserInfo();                               // 获取他人信息
    void getMyselfInfoAsync();                                  // 获取用户个人信息
    void getOtherUserInfoAsync(const QString& userId);          // 获取其他用户信息
    void setAvatar(const QString& fileId);                      // 修改用户头像ID
    void setAvatarAsync(const QString& fileId);                 // 设置用户头像
    void setUserVideoList(const QJsonObject& videoListObj);     // 设置用户视频列表
    VideoList* getUserVideoList();                              // 获取用户视频列表
    void setStatusVideoList(const QJsonObject& videoListObj);   // 设置状态下的视频列表
    VideoList* getStatusVideoList();                            // 设置状态下的视频列表
    void getUserVideoListAsync(const QString& userId,
                               int pageIndex,
                               const QString& whichPage);       // 获取我的视频列表
    void getStatusVideoListAsync(int videoStatus,int pageIndex);// 获取状态视频列表
    void getAuthcodeAsync(const QString& email);                // 获取验证码
    void loginWithEmailAsync(const QString& email,
                                const QString& authcode,
                                const QString& authcodeId);     // 邮箱登录
    void loginWithPasswordAsync(const QString& userName,
                                const QString& password);       // 账号密码登录
    void loginSessionAsync();                                   // 会话登录
    void logoutAsync();                                         // 退出登录
    void setPasswordAsync(const QString& password);             // 设置密码
    void setNickNameAsync(const QString& nickName);             // 修改昵称
    void setAdminsList(const QJsonObject& adminJson,            // 设置管理员列表
                      bool isAdminStatus = true);               // 是否使用管理员状态进行获取，false: 使用邮箱
    AdminList* getAdminsList();                                 // 获取管理员列表
    void getAdminByEmailAsync(const QString& email);      // 通过邮箱获取管理员列表
    void getAdminListByStatusAsync(int pageIndex, AdminStatus adminStatus);  // 通过状态获取管理员列表
    void newAdminAsync(const AdminInfo& userInfo);              // 新增管理员信息
    void editAdminAsync(const AdminInfo& userInfo);             // 编辑管理员
    void setAdminStatusAsync(const AdminInfo& userInfo);        // 设置管理员用户状态
    void delAdminAsync(const QString& adminId);                 // 删除管理员

private:
    explicit DataCenter(QObject *parent = nullptr);
    ~DataCenter();
    static DataCenter* instance;
    QString serverURL;
    network::NetClient netClient;
    KindAndTag* kindsAndTags = nullptr;
    QString loginSessionId = "";					// 当前客户端登录到服务器会话的id
    VideoList* videoListPtr = nullptr;				// 管理首页获取的视频信息
    QHash<int64_t, QList<BarrageInfo>> barrages;    // 弹幕信息
    UserInfo* myselfInfo = nullptr;                 // 保存当前用户个人信息
    UserInfo* otherUserInfo = nullptr;              // 保存其他用户个人信息
    VideoList* userVideoList = nullptr;             // 保存指定用户视频列表：我的视频列表 或 其他用户视频列表
    VideoList* statusVideoList = nullptr;           // 状态视频列表
    AdminList* adminListPtr = nullptr;              // 管理员列表
signals:
    void loginTempUserDone();						// 临时用户登录
    void getAllVideoListDone();						// 获取所有视频信息处理完毕
    void getAllVideoInKindDone();					// 获取分类下所有视频信息处理完毕
    void getAllVideoInTagDone();					// 获取标签下所有视频信息处理完毕
    void getAllVideoListSearchTextDone();			// 根据搜索内容获取视频处理完毕
    void downloadPhotoDone(const QString& imageId,
                           QByteArray imageData);       // 下载图片处理完毕
    void downloadVideoDone(const QString& videoFilePath,
                           const QString& videoFileId); // 下载视频处理完毕
    void uploadVideoDone(const QString& videoId,
                         QWidget* wndPtr = nullptr);    // 上传视频处理完毕
    void uploadVideoDescDone();                         // 上传视频描述信息完毕
    void deleteVideoDone(const QString& videoId);       // 删除视频完毕
    void checkVideoDone();                              // 视频审核完毕
    void putawayVideoDone();                            // 上架视频完毕
    void discardVideoDone();                            // 下架视频完毕
    void newAttentionDone(const QString& userId);       // 新增关注完毕
    void delAttentionDone(const QString& userId);       // 取消关注完毕
    void getVideoBarrageDone(const QString& videoId);   // 获取弹幕信息完毕
    void getIsLikeVideoDone(const QString& videoId, bool isLike);   // 检测是否点赞成功
    void getMyselfInfoDone();                                       // 获取用户个人信息完毕
    void getOtherUserInfoDone();                                    // 获取其他用户信息完毕
    void uploadPhotoDone(const QString& fileId, QWidget* wndPtr);   // 上传图片完毕
    void setAvatarDone();                                           // 修改用户头像完毕
    void getUserVideoListDone(const QString& userId,
                              const QString& whichPage);            // 获取我的视频列表完毕
    void getStatusVideoListDone();                                  // 获取状态视频列表完成
    void getAuthcodeDone(const QString& authcodeId);                // 获取验证码完毕
    void loginWithEmailDone();                                      // 验证码登录完毕
    void loginWithEmailFailed(const QString& errorInfo);            // 验证码登录失败
    void loginWithPasswordDone();                                   // 账号密码登录成功
    void loginWithPasswordFailed(const QString& errorInfo);         // 账号密码登录失败
    void loginSessionDone(bool isTemper);                           // 会话登录成功
    void loginSessionFailed(const QString& errorInfo);              // 会话登录失败
    void logoutDone();                                              // 退出登录成功
    void setPasswordDone();                                         // 设置密码成功
    void setNickNameDone(const QString& nickName);                  // 修改昵称成功
    void getAdminByEmailDone();                                     // 通过邮箱获取管理员列表完毕
    void getAdminListByStatusDone();                                // 通过状态获取管理员列表完毕
    void newAdminDone();                                            // 新增管理员完毕
    void editAdminDone();                                           // 编辑管理员完毕
    void setAdminStatusDone();                                      // 设置用户状态完成
    void delAdminDone();                                            // 删除管理员完毕

};

}	// namespace model
#endif // DATACENTER_H

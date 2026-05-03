#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QString>
#include <QWidget>
#include <QScopedPointer>

#include "./mpv/mpvplayer.h"

#include <cstdint>

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();

    // 设置待上传文件名到界面
    void setVideoTitle(const QString& videoFilePath);

private:
    void addTagsByKind(const QString& kind);	// 将kind下的标签以按钮的形式展示到界面上

private slots:
    void onCommitBtnClicked();                  // 提交按钮

    // 输入字数发生变化
    void onLineEditTextChanged(const QString& text);
    void onPlainEditTextChanged();
    void onChangeBtnClicked();                  // 更改视频封面图
    void onUpdateTags(const QString& kind);		// QComoBox中分类选择改变槽函数
    void onUploadVideoDone(const QString& videoId); // 上传视频完成
    void uploadPhoto(const QString& photoPath);     // 上传封面图
    void resetPage();                               // 清空上传页面信息
    void getDurationDone(int64_t duration);     // 获取视频总时长成功

signals:
    void switchMySelfPage(int pageIndex);       // 切换到我的页面
private:
    Ui::UploadVideoPage *ui;
    QString videoFilePath;          // 上传视频路径
    bool isUploadVideoOk = false;   // 上传视频是否成功
    bool isUploadPhotoOk = false;   // 图片是否上传成功
    bool isDurationOk = false;      // 视频总时长获取成功
    QString coverImageId;           // 频封面上传成功后返回视频封⾯id，上传视频信息时需用到
    QString videoId;                // 视频ID
    QScopedPointer<MpvPlayer> mpvPlayer; // 这个实例是要获取视频总时长
    int64_t duration;               // 视频总时长
};

#endif // UPLOADVIDEOPAGE_H

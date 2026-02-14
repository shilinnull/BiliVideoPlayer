#ifndef BILIVIDEOPLAYER_H
#define BILIVIDEOPLAYER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>

QT_BEGIN_NAMESPACE
namespace Ui {
class BiliVideoPlayer;
}
QT_END_NAMESPACE


enum StackedWidgetPage {
    HomePage = 0,
    MyselfPage = 1,
    AdminPage = 2,
    UploadPage = 3
};

class BiliVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    ~BiliVideoPlayer();
    static BiliVideoPlayer* getInstance();
    void showSystemPageBtn(bool isShow = true);
    void switchToUserInfoPage(const QString& userId);   // 切换到其他用户主页

private:
    // 初始化UI
    void initUI();

    // 连接信号和槽
    void connectSignalAndSlot();
    void resetSwitchBtnInfo(int pageId);

private slots:
    void onSwitchStackedWidgetPage(int pageId);
protected:
    // 实现窗口拖拽
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
private:
    BiliVideoPlayer(QWidget *parent = nullptr);
    void onSwitchPageUI(int pageId);
private:
    QPoint dragPos;
    static BiliVideoPlayer *instance;
private:
    Ui::BiliVideoPlayer *ui;
};
#endif // BILIVIDEOPLAYER_H

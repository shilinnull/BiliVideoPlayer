#ifndef BILIVIDEOPLAYER_H
#define BILIVIDEOPLAYER_H

#include <QWidget>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

#include "util.h"

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
    static BiliVideoPlayer* getInstance();
    ~BiliVideoPlayer();
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
private:
    QPoint dragPos;
    static BiliVideoPlayer *instance;
private:
    Ui::BiliVideoPlayer *ui;
};
#endif // BILIVIDEOPLAYER_H

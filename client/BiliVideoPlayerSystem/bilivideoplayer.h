#ifndef BILIVIDEOPLAYER_H
#define BILIVIDEOPLAYER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class BiliVideoPlayer;
}
QT_END_NAMESPACE

class BiliVideoPlayer : public QWidget
{
    Q_OBJECT

public:
    BiliVideoPlayer(QWidget *parent = nullptr);
    ~BiliVideoPlayer();

private:
    // 初始化UI
    void initUI();

private:
    Ui::BiliVideoPlayer *ui;
};
#endif // BILIVIDEOPLAYER_H

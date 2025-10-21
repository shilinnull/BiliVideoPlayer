#ifndef VOLUME_H
#define VOLUME_H

#include <QWidget>

namespace Ui {
class Volume;
}

class Volume : public QWidget
{
    Q_OBJECT

public:
    explicit Volume(QWidget *parent = nullptr);
    ~Volume();

    bool eventFilter(QObject *watched, QEvent* event) override;
    void calcVolume();
signals:
    void setVolume(int volumeRatio);    // 通知PlayerPage页面，进行音量调节
private:
    Ui::Volume *ui;
    int volumeRatio;        // 保存音量的大小
};

#endif // VOLUME_H

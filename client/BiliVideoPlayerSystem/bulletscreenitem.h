#ifndef BULLETSCREENITEM_H
#define BULLETSCREENITEM_H

#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>

class BulletScreenInfo
{
public:
    QString userId;
    QString videoId;
    int64_t playTime;
    QString text;
    BulletScreenInfo(const QString& userId = "", int64_t playTime = 0,const QString& text = "");
};


class BulletScreenItem : public QFrame
{
    Q_OBJECT
public:
    explicit BulletScreenItem(QWidget *parent = nullptr);
    void setBulletScreenText(const QString &content);	// 设置弹幕文本
    void setBulletScreenIcon(QPixmap& pixmap);			// 设置弹幕用户头像
    void setBulletScreenAnimal(int x, int duration);	// 添加动画
    void startAnimal();									// 开启动画
private:
    QLabel* imageLabel; 			// 用户头像
    QLabel* text;					// 弹幕文本
    QPropertyAnimation* animal;		// 动画对象实例指针
};

#endif // BULLETSCREENITEM_H

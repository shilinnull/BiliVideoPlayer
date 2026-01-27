#ifndef HomePageWidget_H
#define HomePageWidget_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class HomePageWidget;
}

class HomePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HomePageWidget(QWidget *parent = nullptr);
    ~HomePageWidget();

    void connectSignalAndSlot();
    void initKindsAndTags();            // 初始化分类和标签
    void initRefreshAndTop();           // 初始化刷新和置顶按钮
    void initVideos();                  // 初始化视频列表
    void clearLayoutVideos();           // 清空视频列表
    void onSearchVideos(const QString& searchText);     // 搜索视频

    // 创建按钮函数
    QPushButton* buildSelectBtn(QWidget *parent, const QString &color, const QString &text);
    // 点击分类后需要显示的标签
    void resetTags(const QList<QString> &tags);
private slots:
    void onKindBtnClicked(QPushButton* clickKindBtn);   // 分类按钮点击
    void onTagBtnClicked(QPushButton* clickLabelBtn);   // 标签按钮点击
    void onRefreshBtnClicked();                         // 刷新按钮点击
    void onTopBtnClicked();                             // 置顶按钮点击
    void updateVideoList();								// 更新视频列表

private:
    Ui::HomePageWidget *ui;
    QString curKind;
    QString curTag;
};

#endif // HomePageWidget_H

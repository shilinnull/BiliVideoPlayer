#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class homePageWidget;
}

class homePageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit homePageWidget(QWidget *parent = nullptr);
    ~homePageWidget();

    void initKindsAndTags();            // 初始化分类和标签
    void initRefreshAndTop();        // 初始化刷新和置顶按钮

    // 创建按钮函数
    QPushButton* buildSelectBtn(QWidget *parent, const QString &color, const QString &text);
    // 点击分类后需要显示的标签
    void resetTags(const QList<QString> &tags);
private slots:
    void onKindBtnClicked(QPushButton* clickKindBtn);   // 分类按钮点击
    void onTagBtnClicked(QPushButton* clickLabelBtn);   // 标签按钮点击
    void onRefreshBtnClicked();                         // 刷新按钮点击
    void onTopBtnClicked();                             // 置顶按钮点击

private:
    Ui::homePageWidget *ui;

    QHash<QString, QList<QString>> tags;
};

#endif // HOMEPAGEWIDGET_H

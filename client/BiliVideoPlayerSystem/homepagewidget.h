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

    // 初始化分类和标签
    void initKindsAndTags();
    // 创建按钮函数
    QPushButton* buildSelectBtn(QWidget *parent, const QString &color, const QString &text);
    // 点击分类后需要显示的标签
    void resetTags(const QList<QString> &tags);
private slots:
    void onKindBtnClicked(QPushButton* clickKindBtn);
    void onTagBtnClicked(QPushButton* clickLabelBtn);

private:
    Ui::homePageWidget *ui;

    QHash<QString, QList<QString>> tags;
};

#endif // HOMEPAGEWIDGET_H

#ifndef UPLOADVIDEOPAGE_H
#define UPLOADVIDEOPAGE_H

#include <QWidget>

namespace Ui {
class UploadVideoPage;
}

class UploadVideoPage : public QWidget
{
    Q_OBJECT

public:
    explicit UploadVideoPage(QWidget *parent = nullptr);
    ~UploadVideoPage();

private:
    void addTagsByKind(const QString& kind);	// 将kind下的标签以按钮的形式展示到界面上

private slots:
    void onCommitBtnClicked();                  // 提交按钮

    // 输入字数发生变化
    void onLineEditTextChanged(const QString& text);
    void onPlainEditTextChanged();
    void onChangeBtnClicked();                  // 更改视频封面图
    void onUpDateTags(const QString& kind);		// QComoBox中分类选择改变槽函数

signals:
    void switchMySelfPage(int pageIndex);       // 切换到我的页面
private:
    Ui::UploadVideoPage *ui;
};

#endif // UPLOADVIDEOPAGE_H

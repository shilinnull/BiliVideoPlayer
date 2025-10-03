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

private slots:
    void onCommitBtnClicked();                  // 提交按钮

    // 输入字数发生变化
    void onLineEditTextChanged(const QString& text);
    void onPlainEditTextChanged();

    void onChangeBtnClicked();                  // 更改视频封面图

signals:
    void switchMySelfPage(int pageIndex);       // 切换到我的页面
private:
    Ui::UploadVideoPage *ui;
};

#endif // UPLOADVIDEOPAGE_H

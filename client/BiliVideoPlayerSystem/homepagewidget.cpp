#include "homepagewidget.h"
#include "ui_homepagewidget.h"
#include "util.h"
#include "videobox.h"

#include "./model/datacenter.h"

#include <QVBoxLayout>

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);

    initKindsAndTags();     // 初始化分类和标签按钮
    initRefreshAndTop();    // 初始化刷新和置顶按钮
    initVideos();           // 初始化视频列表
}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::initKindsAndTags()
{
    // 创建分类按钮
    QPushButton* kindBtn = buildSelectBtn(ui->classifys, "#FF6699", "分类");
    ui->classifyHLayout->addWidget(kindBtn);

    // 到数据中心获取所有分类数据
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    auto kinds = kindAndTagPtr->getAllKinds();

    for(auto &kind : kinds)
    {
        QPushButton* kindBtn = buildSelectBtn(ui->classifys, "#666666", kind);
        ui->classifyHLayout->addWidget(kindBtn);
        connect(kindBtn, &QPushButton::clicked, this, [=](){
            onKindBtnClicked(kindBtn);      // 分类按钮点击
        });
    }

    ui->classifyHLayout->setSpacing(8);

    // 获取分类下的标签，默认显示第0个标签
    auto tags = kindAndTagPtr->getTagsByKind(kinds[0]).keys();
    // 默认显示历史标签
    resetTags(tags);
    // 默认选中第一个标签
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    onKindBtnClicked(kindBtns[1]);
}

void HomePageWidget::initRefreshAndTop()
{
    // 先创建一个QWidget
    QWidget* refreshTopWidget = new QWidget(this);
    refreshTopWidget->setFixedSize(42, 94);
    refreshTopWidget->setStyleSheet("QPushButton{"
                                    "border-radius: 21px;"
                                    "border: none;"
                                    "background-color: #61666D;}"
                                    "QPushButton:hover{background-color: #FF6699;}");

    QVBoxLayout* layout = new QVBoxLayout(refreshTopWidget);
    QPushButton* topBtn = new QPushButton(refreshTopWidget);
    topBtn->setFixedSize(42, 42);
    topBtn->setStyleSheet("border-image : url(:/images/homePage/zhiding.png);");
    layout->addWidget(topBtn);

    QPushButton* refreshBtn = new QPushButton(refreshTopWidget);
    refreshBtn->setFixedSize(42, 42);
    refreshBtn->setStyleSheet("border-image : url(:/images/homePage/shuaxin.png);");

    layout->addWidget(refreshBtn);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);

    refreshTopWidget->move(1278, 618);

    connect(topBtn, &QPushButton::clicked, this, &HomePageWidget::onTopBtnClicked);
    connect(refreshBtn, &QPushButton::clicked, this, &HomePageWidget::onRefreshBtnClicked);
}

void HomePageWidget::initVideos()
{
    for(int i = 0; i < 16; i++)
    {
        VideoBox* video = new VideoBox();
        ui->videoGLayout->addWidget(video, i / 4, i % 4);
    }
}

QPushButton *HomePageWidget::buildSelectBtn(QWidget *parent, const QString &color, const QString &text)
{
    QPushButton* btn = new QPushButton(text, parent);
    btn->setMinimumHeight(26);
    btn->setFixedWidth(text.size() * 16 + 18 + 18);
    btn->setStyleSheet("color: " + color);
    return btn;
}

void HomePageWidget::resetTags(const QList<QString> &tags)
{
    // 创建标签
    QPushButton* tag = buildSelectBtn(ui->labels, "#FF6699", "标签");
    ui->labelHLayout->addWidget(tag);

    // 创建具体标签的按钮
    for(auto &tagText : tags)
    {
        QPushButton* labelBtn = buildSelectBtn(ui->labels, "#666666", tagText);
        ui->labelHLayout->addWidget(labelBtn);
        connect(labelBtn, &QPushButton::clicked, this, [=](){
            onTagBtnClicked(labelBtn);      // 标签按钮点击
        });
    }
    ui->labelHLayout->setSpacing(4);
}

void HomePageWidget::onKindBtnClicked(QPushButton *clickKindBtn)
{

    clickKindBtn->setStyleSheet("background-color: #FFECF1;"
                                "color: #FF6699;");

    // 清除旧颜色
    QList<QPushButton*> kindBtns = ui->classifys->findChildren<QPushButton*>();
    for(auto &kindBtn : kindBtns)
    {
        if(kindBtn != clickKindBtn && kindBtn->text() != "分类")
            kindBtn->setStyleSheet("color: #666666");
    }

    // 销毁之前的标签
    QList<QPushButton*> labels = ui->labels->findChildren<QPushButton*>();
    for(auto &label : labels)
    {
        ui->labelHLayout->removeWidget(label);
        delete label;
    }

    // 选中了该分类重新添加标签
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    resetTags(kindAndTagPtr->getTagsByKind(clickKindBtn->text()).keys());
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickLabelBtn)
{

    clickLabelBtn->setStyleSheet("background-color: #FFECF1;"
                                "color: #FF6699;");
    // 清除旧颜色
    QList<QPushButton*> tagBtns = ui->labels->findChildren<QPushButton*>();
    for(auto &tagBtn : tagBtns)
    {
        if(tagBtn != clickLabelBtn && tagBtn->text() != "标签")
            tagBtn->setStyleSheet("color: #666666;");
    }
}

void HomePageWidget::onRefreshBtnClicked()
{
    LOG() << "刷新按钮点击了";
}

void HomePageWidget::onTopBtnClicked()
{
    LOG() << "置顶按钮点击了";
}

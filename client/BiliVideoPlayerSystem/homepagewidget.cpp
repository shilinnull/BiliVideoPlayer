#include "homepagewidget.h"
#include "ui_homepagewidget.h"
#include "util.h"
#include "videobox.h"

#include "./model/datacenter.h"

#include <QVBoxLayout>
#include <QScrollBar>

HomePageWidget::HomePageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePageWidget)
{
    ui->setupUi(this);

    connectSignalAndSlot();
    initKindsAndTags();     // 初始化分类和标签按钮
    initRefreshAndTop();    // 初始化刷新和置顶按钮
    initVideos();           // 初始化视频列表

}

HomePageWidget::~HomePageWidget()
{
    delete ui;
}

void HomePageWidget::connectSignalAndSlot()
{
    auto dataCenter = model::DataCenter::getInstance();
    connect(dataCenter, &model::DataCenter::getAllVideoListDone, this,[=]{
        this->updateVideoList();
    });
    connect(dataCenter, &model::DataCenter::getAllVideoInKindDone, this,[=]{
        this->updateVideoList();
    });
    connect(dataCenter, &model::DataCenter::getAllVideoInTagDone, this,[=]{
        this->updateVideoList();
    });

    // 点击搜索按钮或者在输入框回车
    connect(ui->search, &SearchLineEdit::searchVideos, this, &HomePageWidget::onSearchVideos);
    connect(dataCenter, &model::DataCenter::getAllVideoListSearchTextDone, this, [=]{
        this->updateVideoList();
    });
    connect(ui->videoScroll->verticalScrollBar(), &QScrollBar::valueChanged, this, &HomePageWidget::onScrollAreaValueChanged);
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

    // 启动时默认显示第一个分类的标签
    if(!kinds.isEmpty()) {
        auto tags = kindAndTagPtr->getTagsByKind(kinds[0]).keys();
        resetTags(tags);
    }
}

void HomePageWidget::initRefreshAndTop()
{
    // 先创建一个QWidget
    QWidget* refreshTopWidget = new QWidget(this);
    refreshTopWidget->setFixedSize(42, 94);
    refreshTopWidget->setStyleSheet("QPushButton{"
                                    "border-radius: 21px;"
                                    "border: none;"
                                    "background-color: rgba(97, 102, 109, 0.3);}"
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
    // 左上角对齐
    ui->videoGLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    videoListStyle = AllStyle;

    // 从服务器上获取所有视频，默认20个
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getAllVideoListAsync();
}

void HomePageWidget::clearLayoutVideos()
{
    LOG() << "清空首页中旧视频个数: " << ui->videoGLayout->count();
    QLayoutItem* videoItem = nullptr;
    while((videoItem = ui->videoGLayout->takeAt(0)) != nullptr) {
        if (videoItem->widget()) {
            delete videoItem->widget();
        }
        delete videoItem;
    }
    repaint();
    // 清空dataCenter中视频列表
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getVideoListPtr()->clearVideoList();
    ui->videoScroll->verticalScrollBar()->setValue(0);  // 视频清空之后，将滚动条恢复到最上面
}

void HomePageWidget::onSearchVideos(const QString &searchText)
{
    videoListStyle = SearchStyle;

    clearLayoutVideos();
    // 在服务器上进行模糊搜索进行返回
    auto dataCenter = model::DataCenter::getInstance();
    dataCenter->getVideosBySearchTextAsync(searchText);
}

void HomePageWidget::onScrollAreaValueChanged(int value)
{
    auto dataCenter = model::DataCenter::getInstance();
    auto videoList = dataCenter->getVideoListPtr();
    // 滚动条为0或者视频个数小于20个就不加载
    if(value == 0 || videoList->getVideoCount() < model::VideoList::PAGE_COUNT)
        return ;
    if(videoList->getVideoCount() == videoList->getVideoTotalCount()) {
        return ;
    }
    if(value == ui->videoScroll->verticalScrollBar()->maximum()) {
        auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
        switch(videoListStyle) {
        case AllStyle:
            dataCenter->getAllVideoListAsync();
            break;
        case KindStyle:
            dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(curKind));
            break;
        case TagStyle:
            dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, curTag));
            break;
        case SearchStyle:
            dataCenter->getVideosBySearchTextAsync(ui->search->text());
            break;
        default:
            LOG() << "暂不支持的数据类型";
        }
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
    videoListStyle = KindStyle;
    const QString kindText = clickKindBtn->text();
    if(curKind == kindText)
        return ;
    curKind = kindText;
    curTag = "";

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

    // 从服务器上获取该分类的所有视频
    clearLayoutVideos();    // 先清除旧的数据
    dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(kindText));
}

void HomePageWidget::onTagBtnClicked(QPushButton *clickLabelBtn)
{
    videoListStyle = TagStyle;
    const QString tagText = clickLabelBtn->text();
    if(tagText == curTag)
        return ;
    curTag = tagText;

    clickLabelBtn->setStyleSheet("background-color: #FFECF1;"
                                "color: #FF6699;");
    // 清除旧颜色
    QList<QPushButton*> tagBtns = ui->labels->findChildren<QPushButton*>();
    for(auto &tagBtn : tagBtns)
    {
        if(tagBtn != clickLabelBtn && tagBtn->text() != "标签")
            tagBtn->setStyleSheet("color: #666666;");
    }
    clearLayoutVideos();
    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();
    auto kinds = kindAndTagPtr->getAllKinds();
    if(kinds.isEmpty()) {
        curKind = kinds[0];
    }
    dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, tagText));
}

void HomePageWidget::onRefreshBtnClicked()
{
    clearLayoutVideos();

    auto dataCenter = model::DataCenter::getInstance();
    auto kindAndTagPtr = dataCenter->getKindAndTagsClassPtr();

    switch(videoListStyle) {
    case AllStyle:
        dataCenter->getAllVideoListAsync();
        break;
    case KindStyle:
        dataCenter->getAllVideoInKindAsync(kindAndTagPtr->getKindId(curKind));
        break;
    case TagStyle:
        dataCenter->getAllVideoInTagAsync(kindAndTagPtr->getTagId(curKind, curTag));
        break;
    case SearchStyle:
        dataCenter->getVideosBySearchTextAsync(ui->search->text());
        break;
    default:
        LOG() << "暂不支持的数据类型";
    }
}

void HomePageWidget::onTopBtnClicked()
{
    ui->videoScroll->verticalScrollBar()->setValue(0);
}

void HomePageWidget::updateVideoList()
{
    auto dataCenter = model::DataCenter::getInstance();
    auto videoIdList = dataCenter->getVideoListPtr()->getVideoList();
    LOG() << "从服务器上获取了：" << videoIdList.size() << "个视频";
    for(int i = ui->videoGLayout->count(); i < videoIdList.size(); ++i){
        // 构建视频显示框VideoBox
        VideoBox* videoBox = new VideoBox(videoIdList[i], this);
        ui->videoGLayout->addWidget(videoBox, i/4, i%4);
    }
    LOG()<<"添加到layout中视频个数："<<ui->videoGLayout->count();
}

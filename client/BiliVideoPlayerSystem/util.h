#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QString>
#include <QFileInfo>
#include <QIcon>
#include <QPainter>
#include <QPainterPath>

#define TEST_UI

static inline QString getFileName(const QString& path) {
    QFileInfo fileinfo(path);
    return fileinfo.fileName();
}

#define TAG QString("[%1:%2]").arg(getFileName(__FILE__), QString::number(__LINE__))
#define LOG() qDebug().noquote() << TAG

static inline QByteArray loadFileToByteArray(const QString& path) {
    QFile file(path);
    bool ok = file.open(QFile::ReadOnly);
    if(!ok) {
        LOG() << "打开文件失败";
        return QByteArray();
    }
    QByteArray content = file.readAll();
    file.close();
    return content;
}

static inline void writeByteArrayToFile(const QString& path, const QByteArray& content) {
    QFile file(path);
    bool ok = file.open(QFile::WriteOnly);
    if (!ok) {
        LOG() << "文件打开失败!";
        return;
    }
    file.write(content);
    file.flush();
    file.close();
}

static inline QIcon makeIcon(const QByteArray& byteArray) {
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    QIcon icon(pixmap);
    return icon;
}

static inline QIcon makeCircleIcon(const QByteArray& byteArray, int radius) {
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    if (pixmap.isNull()) {
        return QIcon();
    }
    // 把 pixmap 缩放到指定的 2*radius 大小. IgnoreAspectRatio 忽略长宽比;
    // SmoothTransformation 平滑缩放, 获得更高的图片质量, 但是会牺牲一定速度.
    pixmap = pixmap.scaled(2*radius, 2*radius, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    // 构造绘图设置，可以理解成画图的画布
    QPixmap output = QPixmap(pixmap.size());
    output.fill(Qt::transparent); // 设置透明背景
    QPainter painter(&output);
    painter.setRenderHint(QPainter::Antialiasing);
    // 创建圆形路径
    QPainterPath path;
    path.addEllipse(0, 0, 2 * radius, 2 * radius);
    // 设置裁剪路径，裁剪路径的作用是限制绘图操作的范围，只有在裁剪路径内的区域才会被裁剪
    painter.setClipPath(path);
    // 绘制圆形图片
    painter.drawPixmap(0, 0, pixmap);
    // 结束绘制：end()内部会释放与绘图设备相关的资源，确保绘图命令都能够被正确执行
    painter.end();
    QIcon icon(output);
    return icon;
}

#endif // UTIL_H

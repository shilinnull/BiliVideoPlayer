#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QString>
#include <QFileInfo>

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

#endif // UTIL_H

#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QString>
#include <QFileInfo>

inline const QString default_text_color= "#666666";
inline const QString default_background_color= "#FFFFFF";
inline const QString clicked_background_color = "#FFECF1";
inline const QString clicked_text_color = "#FF6699";

static inline QString getFileName(const QString& path) {
    QFileInfo fileinfo(path);
    return fileinfo.fileName();
}

#define TAG QString("[%1:%2]").arg(getFileName(__FILE__), QString::number(__LINE__))
#define LOG() qDebug().noquote() << TAG

#endif // UTIL_H

#ifndef FILEREADER_H
#define FILEREADER_H

#include <QObject>
#include <QQuickItem>

#include "Common/CommonDef.hpp"

namespace Tool {
class FileReader : public QObject
{
    Q_OBJECT
    QML_ELEMENT

    QML_DECL_PROPERTY(QString, file, "")
    QML_DECL_PROPERTY(QString, source, "")
public:
    FileReader(QObject* parent = nullptr);

private slots:
    void onFileChanged();
};

inline void registerModule()
{
    auto packName = "Tool";
    qmlRegisterType<FileReader>(packName, 1, 0, "FileReader");
}
}


#endif

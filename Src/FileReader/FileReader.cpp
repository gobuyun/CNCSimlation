#include "FileReader.h"

#include <QFile>

using namespace Tool;

FileReader::FileReader(QObject *parent) : QObject(parent)
{
    connect(this, &FileReader::fileChanged, this, &FileReader::onFileChanged);
}

void FileReader::onFileChanged()
{
    QUrl url(m_file);
    QFile file(url.toLocalFile());
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "open" << m_file << "failed, " << file.errorString();
        return;
    }
    auto c = file.readAll();
    file.close();
    // this->setProperty("source", c);
    m_source = c;
    emit sourceChanged();
}

#include <QString>
#include <QFileInfo>
#include <QtGlobal>
#include <QDebug>
#include <QDir>

#include <string.h>
#include <QStandardPaths>
#include <QDateTime>

#include "../deleter.h"

static bool TrashInitialized = false;
static QString TrashPath;
static QString TrashPathInfo;
static QString TrashPathFiles;

static void InitializeTrash()
{
    QStringList paths;
    const char* xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home)
    {
        Q_ASSERT(false); // Debug this the first time.
        qDebug() << "XDG_DATA_HOME not yet tested";
        QString xdgTrash( xdg_data_home );
        paths.append(xdgTrash + "/Trash");
    }

    QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    paths.append( home + "/.local/share/Trash" );
    paths.append( home + "/.trash" );
    foreach( QString path, paths )
    {
        if (TrashPath.isEmpty())
        {
            QDir dir(path);
            if(dir.exists())
            {
                TrashPath = path;
            }
        }
    }

    Q_ASSERT(!TrashPath.isEmpty());
    TrashPathInfo = TrashPath + "/info";
    TrashPathFiles = TrashPath + "/files";
    Q_ASSERT(QDir(TrashPathInfo).exists() && QDir(TrashPathFiles).exists());

    TrashInitialized = true;
}

int SendFileToTrash(const char *filenameChar)
{   
#if 0
    QString filename(filenameChar);
    Q_ASSERT(false); // Debug this the first time.
    if(!TrashInitialized){
        InitializeTrash();
    }
    QFileInfo original(filename);
    Q_ASSERT(original.exists());
    QString info;
    info += "[Trash Info]\nPath=";
    info += original.absoluteFilePath();
    info += "\nDeletionDate=";
    info += QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzzZ");
    info += "\n";

    QString trashname = original.fileName();
    QString infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
    QString filepath = TrashPathFiles + "/" + trashname;
    int nr = 1;
    while( QFileInfo( infopath ).exists() || QFileInfo( filepath ).exists() ){
        nr++;
        trashname = original.baseName() + "." + QString::number( nr );
        if( !original.completeSuffix().isEmpty() ){
            trashname += QString( "." ) + original.completeSuffix();
        }
        infopath = TrashPathInfo + "/" + trashname + ".trashinfo";
        filepath = TrashPathFiles + "/" + trashname;
    }
    QDir dir;
    bool ok = dir.rename(original.absoluteFilePath(), filepath);
    Q_ASSERT(ok); // "Make sure it moved to trash

    QFileInfo infofile;
    infofile.createUtf8(infopath, info);
#endif
    return 0;
}

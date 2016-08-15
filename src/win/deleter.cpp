#include <QString>
#include <QFileInfo>
#include <string.h>
#include <QtGlobal>
#include <QDebug>

#include <windows.h>

#include "../deleter.h"

int SendFileToTrash(const char *filenameChar)
{   
    QString filename(filenameChar);
    QFileInfo fileinfo(filename);
    Q_ASSERT(fileinfo.exists());

    WCHAR from[ MAX_PATH ];
    memset( from, 0, sizeof( from ));
    int l = fileinfo.absoluteFilePath().toWCharArray( from );
    Q_ASSERT( 0 <= l && l < MAX_PATH );
    from[ l ] = '\0';
    SHFILEOPSTRUCT fileop;
    memset( &fileop, 0, sizeof( fileop ) );
    fileop.wFunc = FO_DELETE;
    fileop.pFrom = from;
    fileop.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    int rv = SHFileOperation( &fileop );
    if( 0 != rv ){
        qDebug() << rv << QString::number( rv ).toInt( 0, 8 );
        Q_ASSERT(false); // move to trash failed
    }

    return 0;
}

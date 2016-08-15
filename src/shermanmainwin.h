#ifndef SHERMANMAINWIN_H
#define SHERMANMAINWIN_H

#include <QMainWindow>
#include <QStringList>
#include <QPixmap>
#include <QLabel>
#include <QFuture>
#include <QTime>
#include <QSettings>
#include <QCloseEvent>
#include <QImage>
#include <QList>

#include <utility>

#include "piclabel.h"

#define BUILD_VERSION "Version: 0.12"

namespace Ui {
class ShermanMainWin;
}

class ShermanMainWin : public QMainWindow
{
    Q_OBJECT

public:
    static QStringList extensions;

public:
    explicit ShermanMainWin(QWidget *parent = 0);
    ~ShermanMainWin();
    void loadPicture(const QString& path, bool preload = true);
    void loadPicture(const QString& path, const QImage& image, bool preload);
    void findNeighborPics(const QString& path);
    QSettings& getSetting();
    QImage preloadPicIndex(int ix);

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event);
    virtual void closeEvent(QCloseEvent *event);
    void PrevPic(bool preload);
    void NextPic(bool preload);

public slots:
    void DeletePic();
    void OpenFile();
    void ShowHelp();

private:
    Ui::ShermanMainWin *ui;
    PicLabel *picLabel;
    QStringList neighbors;
    int currentPicIndex;
    QSettings *settings;
    QFuture<QImage> preloadedNextPic;
    QFuture<QImage> preloadedPrevPic;
    bool showingMessage;
    QString lastPicOpened;
};

#endif // SHERMANMAINWIN_H

#include "shermanmainwin.h"
#include "ui_shermanmainwin.h"
#include "ui_about.h"

#include <QApplication>
#include <QDebug>
#include <QImage>
#include <QtConcurrent>
#include <QDir>
#include <QFileInfo>
#include <QDirIterator>
#include <QKeyEvent>
#include <QWheelEvent>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>

#include "deleter.h"

#include <iostream>

QStringList ShermanMainWin::extensions = QStringList()
        << "png" << "jpg" << "jpeg";

ShermanMainWin::ShermanMainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ShermanMainWin),
    currentPicIndex(-1),
    showingMessage(false)
{    
    ui->setupUi(this);

    setWindowTitle("Sherman - <>");
    QIcon icon(":/icons/icon.png");
    setWindowIcon(icon);
    centralWidget()->setStyleSheet("QGraphicsView { background-color : #0f1212;}");

    qApp->installEventFilter(this);
    settings = new QSettings("Chen", "Sherman");

    picLabel = new PicLabel(this);
    ui->centralWidget->layout()->addWidget(picLabel);

    restoreGeometry(settings->value("mainwin/geometry").toByteArray());
    restoreState(settings->value("mainwin/windowState").toByteArray());

    connect(ui->actionTurn_Left, SIGNAL(triggered()),
            picLabel, SLOT(rotateLeft()));
    connect(ui->actionTurn_Right, SIGNAL(triggered()),
            picLabel, SLOT(rotateRight()));
    connect(ui->actionDelete, SIGNAL(triggered()),
            this, SLOT(DeletePic()));
    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(OpenFile()));
    connect(ui->actionHelp, SIGNAL(triggered()),
            this, SLOT(ShowHelp()));

    // Parse command line arguments
    QStringList args = qApp->arguments();
    QString picPath = QString::null;
    for (int k = 1; k < args.length(); k++) {
        for (int m = 0; m < ShermanMainWin::extensions.length(); m++)
            if (args[k].endsWith(ShermanMainWin::extensions[m], Qt::CaseInsensitive))
                picPath = args[k];
    }
    if (picPath != QString::null) {
        loadPicture(picPath);
    }

    // Set last pic opened
    lastPicOpened = getSetting().value("lastPicOpened", QString("")).toString();
}

ShermanMainWin::~ShermanMainWin()
{
    delete ui;
    delete settings;
}

void ShermanMainWin::loadPicture(const QString &picPath, const QImage &image, bool preload)
{
    // Initialize GUI with this path.
    setWindowTitle(QString("Sherman - ") + picPath);
    picLabel->showPicture(image, picPath);

    // Store in the settings the last path
    lastPicOpened = picPath;

    // Start searching for other pics in the same folder
    findNeighborPics(picPath);

    if (preload) {
        // Now preload previous and next
        preloadedNextPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex+1);
        preloadedPrevPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex-1);
    }
}

void ShermanMainWin::loadPicture(const QString &picPath, bool preload)
{
    // Initialize GUI with this path.
    setWindowTitle(QString("Sherman - ") + picPath);
    picLabel->showPicture(picPath);

    // Store in the settings the last path
    lastPicOpened = picPath;

    // Start searching for other pics in the same folder
    findNeighborPics(picPath);

    if (preload) {
        // Now preload previous and next
        preloadedNextPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex+1);
        preloadedPrevPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex-1);
    }
}

void ShermanMainWin::findNeighborPics(const QString &path)
{
    QFileInfo info(path);
    QDirIterator it(info.absolutePath(), QDir::Files);

    QStringList ret;
    while (it.hasNext()) {
        QString file = it.next();
        for (int m = 0; m < ShermanMainWin::extensions.length(); m++) {
            if (file.endsWith(ShermanMainWin::extensions[m], Qt::CaseInsensitive)) {
                ret << file;
            }
        }
    }
    ret.sort(Qt::CaseInsensitive);

    // Find current index
    neighbors = ret;
    currentPicIndex = -1;
    for (int k = 0; k < neighbors.size() && currentPicIndex == -1; k++) {
        if (neighbors[k].endsWith(picLabel->getPath())) {
            currentPicIndex = k;
        }
    }
}

QSettings &ShermanMainWin::getSetting()
{
    return *settings;
}

QImage ShermanMainWin::preloadPicIndex(int ix)
{
    int i = ix;
    if (i < 0) {
        i = neighbors.length() - 1;
    }
    if (i >= neighbors.length()) {
        i = 0;
    }
    return QImage(neighbors[i]);
}

bool ShermanMainWin::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::KeyPress && !picLabel->ShowingMessage() &&!showingMessage)
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_Left) {
            PrevPic(true);
        } else if (keyEvent->key() == Qt::Key_Right) {
            NextPic(true);
        } else if (keyEvent->key() == Qt::Key_Delete ||
                   keyEvent->key() == Qt::Key_Backspace) {
            DeletePic();
        } else if (keyEvent->key() == Qt::Key_Comma) {
            picLabel->rotateLeft();
        } else if (keyEvent->key() == Qt::Key_Period) {
            picLabel->rotateRight();
        }
        event->accept();
        return true;
    } else if (event->type() == QEvent::Wheel && (currentPicIndex != -1)) {
        QWheelEvent* wheelEvent = static_cast<QWheelEvent *>(event);
        if (wheelEvent->angleDelta().y() > 0) {
            picLabel->zoomIn();
        } else if (wheelEvent->angleDelta().y() < 0) {
            picLabel->zoomOut();
        }
    } else if (event->type() == QEvent::FileOpen) {
        QFileOpenEvent *e = static_cast<QFileOpenEvent*>(event);
        loadPicture(e->file());
        e->accept();
        return true;
    }
    return QObject::eventFilter(obj, event);
}

void ShermanMainWin::closeEvent(QCloseEvent *event)
{
    getSetting().setValue("mainwin/geometry", saveGeometry());
    getSetting().setValue("mainwin/windowState", saveState());
    getSetting().setValue("lastPicOpened", lastPicOpened);
    QMainWindow::closeEvent(event);
}

void ShermanMainWin::PrevPic(bool preload)
{
    if (currentPicIndex != -1) {       
        currentPicIndex -= 1;
        if (currentPicIndex < 0) {
            currentPicIndex = neighbors.size() - 1;
        }
        loadPicture(neighbors[currentPicIndex], preloadedPrevPic.result(), preload);
    }
}

void ShermanMainWin::NextPic(bool preload)
{
    if (currentPicIndex != -1) {        
        currentPicIndex += 1;
        if (currentPicIndex == neighbors.length()) {
            currentPicIndex = 0;
        }
        loadPicture(neighbors[currentPicIndex], preloadedNextPic.result(), preload);
    }
}

void ShermanMainWin::DeletePic()
{
    if (currentPicIndex != -1) {
        showingMessage = true;
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Delete image"),
                                      tr("The current image will be deleted,\nAre you sure?"),
                                      QMessageBox::Yes | QMessageBox::No,
                                      QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            QString fileToDelete = neighbors[currentPicIndex];
            NextPic(false);

            // Send to recycle bin
            SendFileToTrash(fileToDelete.toUtf8().constData());

            // Reset previous pic preload
            findNeighborPics(neighbors[currentPicIndex]);
            preloadedNextPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex+1);
            preloadedPrevPic = QtConcurrent::run(this, &ShermanMainWin::preloadPicIndex, currentPicIndex-1);
        }
        showingMessage = false;
    }
}

void ShermanMainWin::OpenFile()
{
    QString lastPath = lastPicOpened;
    QDir lastPathDir;
    if (lastPath.length() == 0) {
        lastPathDir = QDir::home();
    } else {
        QFileInfo info(lastPath);
        lastPathDir = QDir(info.absolutePath());
    }

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open file"),
                                                    lastPathDir.absolutePath(),
                                                    tr("Image Files (*.png *.jpg *.jpeg *.JPG *.JPEG)"));
    if (!fileName.isNull())
        loadPicture(fileName);
}

void ShermanMainWin::ShowHelp()
{
    Ui::Dialog *uiDialog = new Ui::Dialog();
    QDialog *dialog = new QDialog(this);
    uiDialog->setupUi(dialog);
    uiDialog->verticalLayout->setSizeConstraint(QLayout::SetFixedSize);
    uiDialog->labelVersion->setText(QString(BUILD_VERSION));
    dialog->exec();
}

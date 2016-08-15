#include "piclabel.h"

#include <QResizeEvent>
#include <QDebug>
#include <QGraphicsPixmapItem>
#include <QMessageBox>
#include <QTransform>
#include <QImageWriter>

PicLabel::PicLabel(QWidget *parent) :
    QGraphicsView(parent),
    path(QString::null),
    zoomLevel(1.0),
    scene(0),
    item(0),
    rotated(false),
    rotation(0),
    modified(false),
    showingMessage(false)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    setAlignment(Qt::AlignCenter);
    setStyleSheet("QGraphicsView { background-color : #0f1212;}");
    setDragMode(QGraphicsView::ScrollHandDrag);
    setFocusPolicy(Qt::NoFocus);
}

void PicLabel::showPicture(const QString &picPath)
{
    showPicture(QImage(picPath), picPath);
}

void PicLabel::showPicture(const QImage &imageParam, const QString &picPath)
{
    if (!picPath.isNull() && (modified && (rotation % 4) != 0)) {
        showingMessage = true;
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Modified image"),
                                      tr("The current image was modified,\nDo you want to save it?"),
                                      QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        showingMessage = false;
        if (reply == QMessageBox::Yes) {
            // Rotate and save
            QTransform trans;
            trans.rotate(90.0 * ((qreal) rotation));
            QImage rotatedImage = image.transformed(trans, Qt::SmoothTransformation);
            QByteArray format("jpg");
            if (path.endsWith(".png", Qt::CaseInsensitive)) {
                format = QByteArray("png");
            }
            QImageWriter w(path, format);
            w.setQuality(98);
            w.write(rotatedImage);
        }
    }
    zoomLevel = 1;
    rotated = false;
    rotation = 0;
    modified = false;
    image = imageParam;
    this->path = picPath;

    // Delete old scene if exists
    if (scene)
    {
        delete item;
        delete scene;
    }
    scene = new QGraphicsScene();
    setScene(scene);
    item = new QGraphicsPixmapItem(QPixmap::fromImage(imageParam));
    scene->addItem(item);
    fitInView(scene->sceneRect(), Qt::KeepAspectRatio);

    applyZoom();
}

const QString &PicLabel::getPath()
{
    return this->path;
}

bool PicLabel::ShowingMessage() const
{
    return showingMessage;
}

void PicLabel::resizeEvent(QResizeEvent *event)
{
    applyZoom();

    // Finish processing
    QGraphicsView::resizeEvent(event);
}

void PicLabel::zoomIn()
{
    if (zoomLevel != 4) {
        zoomLevel += 0.1;
        if (zoomLevel > 4) {
            zoomLevel = 4;
        }

        applyZoom();
        rotate(90.0 * ((qreal)rotation));
    }
}

void PicLabel::zoomOut()
{
    if (zoomLevel != 1) {
        zoomLevel -= 0.1;
        if (zoomLevel < 1) {
            zoomLevel = 1;
        }
        applyZoom();
        rotate(90.0 * ((qreal)rotation));
    }
}

void PicLabel::rotateLeft()
{
    rotated = !rotated;
    rotation -= 1;
    modified = true;
    applyZoom();
    rotate(90.0 * ((qreal)rotation));
}

void PicLabel::rotateRight()
{
    rotated = !rotated;
    rotation += 1;
    modified = true;
    applyZoom();
    rotate(90.0 * ((qreal)rotation));
}

void PicLabel::applyZoom()
{
    QSize sz = size();

    if (zoomLevel > 1) {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    } else {
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    if (item) {
        resetTransform();
        if (rotated) {
            int aux = sz.height();
            sz.setHeight(sz.width());
            sz.setWidth(aux);
        }
        qreal scalex = ((qreal)sz.width())/((qreal)image.size().width());
        qreal scaley = ((qreal)sz.height())/((qreal)image.size().height());

        scalex = qMin(scalex, scaley);
        scaley = qMin(scalex, scaley);
        scale(scalex * zoomLevel, scaley * zoomLevel);
    }
}

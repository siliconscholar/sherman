#ifndef PICLABEL_H
#define PICLABEL_H

#include <QLabel>
#include <QString>
#include <QImage>
#include <QGraphicsView>

class PicLabel : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PicLabel(QWidget *parent = 0);

    void showPicture(const QString& path);
    void showPicture(const QImage& image, const QString& path);
    const QString& getPath();
    bool ShowingMessage() const;

protected:
    virtual void resizeEvent(QResizeEvent *event);

signals:

public slots:
    void zoomIn();
    void zoomOut();
    void rotateLeft();
    void rotateRight();

private:
    void applyZoom();

private:
    QImage image;
    QString path;
    qreal zoomLevel;

    QGraphicsScene *scene;
    QGraphicsPixmapItem *item;
    bool rotated;
    int rotation;
    bool modified;
    bool showingMessage;
};

#endif // PICLABEL_H

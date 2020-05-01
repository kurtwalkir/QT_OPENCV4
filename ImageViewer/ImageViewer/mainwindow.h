#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QGraphicsScene *imageScene;
    QGraphicsView *imageView;
    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QAction *openAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *prevAction;
    QAction *nextAction;

    void initUI(void);
    void createActions(void);
    void showImage(QString);
    void setupShortcuts(void);

    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;

private slots:
    void openImage(void);
    void saveAs(void);
    void zoomIn(void);
    void zoomOut(void);
    void prevImage(void);
    void nextImage(void);

};
#endif // MAINWINDOW_H

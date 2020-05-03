#include "mainwindow.h"
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QDebug>
#include <QPluginLoader>
#include "opencv2/opencv.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initUI();
    loadPlugins();
}

void MainWindow::initUI()
{
   this->resize(800, 600);
   viewToolBar = addToolBar("File");
   viewToolBar = addToolBar("View");

   imageScene = new QGraphicsScene(this);
   imageView = new QGraphicsView(imageScene);
   setCentralWidget(imageView);

   mainStatusBar = statusBar();
   mainStatusLabel = new QLabel(mainStatusBar);
   mainStatusBar->addPermanentWidget(mainStatusLabel);
   mainStatusLabel->setText("Image Information will be here!");

   createActions();
}

void MainWindow::createActions(void)
{
    openAction = new QAction("&Open", this);
    saveAsAction = new QAction("&Save as", this);
    exitAction = new QAction("E&xit", this);
    zoomInAction = new QAction("Zoom in", this);
    zoomOutAction = new QAction("Zoom Out", this);
    prevAction = new QAction("&Previous Image", this);
    nextAction = new QAction("&Next Image", this);

    viewToolBar->addAction(openAction);
    viewToolBar->addAction(saveAsAction);
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);
    viewToolBar->addAction(exitAction);

    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
    setupShortcuts();

    blurAction = new QAction("Blur", this);
    viewToolBar->addAction(blurAction);
    connect(blurAction, SIGNAL(triggered(bool)), this, SLOT(blurImage()));
}

void MainWindow::openImage(void)
{
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        showImage(filePaths.at(0));
    }
}

void MainWindow::showImage(QString path)
{
    imageScene->clear();
    imageView->resetMatrix();
    QPixmap image(path);
    currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
        .arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);
    currentImagePath = path;
}

void MainWindow::saveAs(void)
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}

void MainWindow::zoomIn()
{
    imageView->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
    imageView->scale(1/1.2, 1/1.2);
}

void MainWindow::prevImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx > 0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx - 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the first one.");
    }
}

void MainWindow::nextImage()
{
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx < fileNames.size() - 1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx + 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the last one.");
    }
}

void MainWindow::setupShortcuts()
{
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    zoomOutAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Up << Qt::Key_Left;
    prevAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Down << Qt::Key_Right;
    nextAction->setShortcuts(shortcuts);
}

void MainWindow::blurImage(void)
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit.");
        return;
    }
    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(
        image.height(),
        image.width(),
        CV_8UC3,
        image.bits(),
        image.bytesPerLine());

    cv::Mat tmp;
    cv::blur(mat, tmp, cv::Size(8, 8));
    mat = tmp;

    QImage image_blurred(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(image_blurred);
    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(pixmap.rect());
    QString status = QString("(editted image), %1x%2")
        .arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}

void MainWindow::loadPlugins()
{
    QDir pluginsDir(QApplication::instance()->applicationDirPath() + "/plugins");
    QStringList nameFilters;
    nameFilters << "*.so";
    QFileInfoList pluginsList = pluginsDir.entryInfoList(
        nameFilters, QDir::NoDotAndDotDot | QDir::Files, QDir::Name);
    foreach(QFileInfo plugin, pluginsList) {
        QPluginLoader pluginLoader(plugin.absoluteFilePath(), this);
        PluginInterface *plugin_ptr = dynamic_cast<PluginInterface*>(pluginLoader.instance());
        if(plugin_ptr) {
            QAction *action = new QAction(plugin_ptr->name());
            viewToolBar->addAction(action);
            plugins[plugin_ptr->name()] = plugin_ptr;
            connect(action, SIGNAL(triggered(bool)), this, SLOT(pluginPerform()));
            // pluginLoader.unload();
        } else {
            qDebug() << "bad plugin: " << plugin.absoluteFilePath();
        }
    }
}


void MainWindow::pluginPerform()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "No image to edit.");
        return;
    }

    QAction *active_action = qobject_cast<QAction*>(sender());
    PluginInterface *plugin_ptr = plugins[active_action->text()];
    if(!plugin_ptr) {
        QMessageBox::information(this, "Information", "No plugin is found.");
        return;
    }

    QPixmap pixmap = currentImage->pixmap();
    QImage image = pixmap.toImage();
    image = image.convertToFormat(QImage::Format_RGB888);
    cv::Mat mat = cv::Mat(
        image.height(),
        image.width(),
        CV_8UC3,
        image.bits(),
        image.bytesPerLine());

    plugin_ptr->edit(mat, mat);

    QImage image_edited(
        mat.data,
        mat.cols,
        mat.rows,
        mat.step,
        QImage::Format_RGB888);
    pixmap = QPixmap::fromImage(image_edited);
    imageScene->clear();
    imageView->resetMatrix();
    currentImage = imageScene->addPixmap(pixmap);
    imageScene->update();
    imageView->setSceneRect(pixmap.rect());
    QString status = QString("(editted image), %1x%2")
        .arg(pixmap.width()).arg(pixmap.height());
    mainStatusLabel->setText(status);
}

MainWindow::~MainWindow()
{
}


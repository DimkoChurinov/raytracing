#include <QPainter>

#include "frontend/tracerwrapper.h"

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{

}

MainWindow::~MainWindow()
{

}

void MainWindow::paintEvent(QPaintEvent* e)
{
	QMainWindow::paintEvent(e);

	if (!mSceneFile.isEmpty())
	{
		TracerWrapper wrapper;

		QPainter painter;
		painter.begin(this);
		//wrapper.renderScene(mSceneFile, &painter, mResX, mResY, mResX, mResY);
		painter.end();
	}
}

void MainWindow::loadAndTraceScene(const QString& sceneFile, int resX, int resY)
{
	mSceneFile = sceneFile;
	mResX      = resX;
	mResY			 = resY;
	
}


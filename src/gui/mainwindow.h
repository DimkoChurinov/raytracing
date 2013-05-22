//-------------------------------------------------------------------
// File: mainwindow.h
// 
// Qt based application main window
//
// Copyright (c) Alexey Vinogradov <alexey.vinogradov@d-inter.ru>
//-------------------------------------------------------------------
#ifndef GUI_MAINWINDOW_H
	#define GUI_MAINWINDOW_H

	#include <QMainWindow>

	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(QWidget *parent = 0, Qt::WFlags flags = 0);
		~MainWindow();

	public:
		void loadAndTraceScene(const QString& sceneFile, int resX, int resY);

	protected:
		void paintEvent(QPaintEvent* e);

	private:
		
		QString				mSceneFile;
		int						mResX;
		int						mResY;
	};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QtGui/QMainWindow>

class MainWindow : public QMainWindow {
public:
	MainWindow();

private:
	class Private;
	Private * p_;
};

#endif

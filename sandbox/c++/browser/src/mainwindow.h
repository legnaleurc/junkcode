#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "controller.h"

QT_BEGIN_NAMESPACE
class QNetworkReply;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void _onMainFrameObjectCleared();
    void _onActionStart();
    void _onActionStop();

private:
	Ui::MainWindow *ui;
	Controller * _ctrl;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

class MainWindowPrivate;

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget * parent = nullptr);

private:
    MainWindowPrivate * d;
};

#endif

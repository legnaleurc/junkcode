#ifndef QTFS_MAINWINDOW_HPP
#define QTFS_MAINWINDOW_HPP

#include <QtWidgets/QMainWindow>

namespace qtfs {

class MainWindow : public QMainWindow {
public:
    MainWindow();

private:
    class Private;
    Private * _;
};

}

#endif

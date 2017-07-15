#ifndef QTFS_VIEWER_HPP
#define QTFS_VIEWER_HPP

#include <QtWidgets/QWidget>
#include <QtWidgets/QFileSystemModel>


namespace qtfs {

class Viewer : public QWidget {
public:
    explicit Viewer(QWidget * parent);

    void initialize(QFileSystemModel * model);

private:
    class Private;
    Private * _;
};

}

#endif

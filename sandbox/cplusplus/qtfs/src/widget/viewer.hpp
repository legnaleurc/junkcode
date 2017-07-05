#ifndef QTFS_VIEWER_HPP
#define QTFS_VIEWER_HPP

#include <QtWidgets/QWidget>


namespace qtfs {

class Viewer : public QWidget {
public:
    explicit Viewer(QWidget * parent);

private:
    class Private;
    Private * _;
};

}

#endif

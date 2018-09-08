#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeView>

#include "drivemodel.h"


int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    DriveModel model;
    QTreeView view;
    view.show();
    view.setModel(&model);
    model.setRootPath("localhost", 8000);

    return app.exec();
}

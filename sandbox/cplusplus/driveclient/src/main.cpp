#include <QtWidgets/QApplication>
#include <QtWidgets/QTreeView>


int main(int argc, char * argv[]) {
    QApplication app(argc, argv);

    QTreeView view;
    view.show();

    return app.exec();
}

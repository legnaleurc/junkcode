#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>

#include "qtcoroutine.hpp"

bool find () {
    static int i = 0;
    static bool ok = false;
    if (i >= 5) {
        ok = true;
    }
    qDebug() << i << ok;
    ++i;
    return ok;
}

void click () {
    qDebug() << SIGNAL(finished());
}

int main (int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    QtCoroutine * b = new QtCoroutine([](const QtCoroutine::Yield & yield)->void {
        bool ok = find();
        while (!ok) {
            // yield for 1000 ms
            yield(1000);
            ok = find();
        }
        click();
    });
    b->connect(b, SIGNAL(finished()), SLOT(deleteLater()));
    QCoreApplication::instance()->connect(b, SIGNAL(finished()), SLOT(quit()));
    b->start();

    return a.exec();
}

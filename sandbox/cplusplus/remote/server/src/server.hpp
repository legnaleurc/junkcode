#include <QtCore/QObject>

namespace remote {

class Server : public QObject {
public:
    Server();

    void start();

private:
    class Private;
    Private * _;
};

}

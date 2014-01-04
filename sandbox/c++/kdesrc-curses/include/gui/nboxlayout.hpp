#ifndef NBOXLAYOUT_HPP
#define NBOXLAYOUT_HPP

#include <memory>

class NWidget;

class NBoxLayout {
public:
    explicit NBoxLayout( NWidget * parent = nullptr );
    virtual ~NBoxLayout();

    void addLayout( NBoxLayout * layout );
    void addWidget( NWidget * widget );

private:
    class Private;
    std::shared_ptr< Private > p_;
};

#endif

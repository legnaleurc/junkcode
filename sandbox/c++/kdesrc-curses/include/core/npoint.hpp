#ifndef NPOINT_HPP
#define NPOINT_HPP

#include <memory>

class NPoint {
public:
    NPoint();
    NPoint( int x, int y );
    NPoint( const NPoint & that );

    NPoint & operator =( const NPoint & that );

    int & x();
    int x() const;
    int & y();
    int y() const;

private:
    class Private;
    std::shared_ptr< Private > p_;
};

#endif

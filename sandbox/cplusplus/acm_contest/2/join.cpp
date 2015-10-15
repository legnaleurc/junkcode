#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <climits>

const double PI = acos( -1.0 );
const double LEFT_COS = cos( PI / 4 );
const double LEFT_SIN = sin( PI / 4 );
const double RIGHT_COS = cos( -1 * PI / 4 );
const double RIGHT_SIN = sin( -1 * PI / 4 );

class Robot {
public:
	explicit Robot( std::istream & in );

	bool isInBoundry() const;

	void rotateLeft( std::size_t arm );

	int dfs();

private:
	typedef std::vector< std::pair< double, double > > Vector;

	void dfs_( std::size_t, std::size_t, std::size_t & );
	void debug_() const;

	std::vector< std::pair< double, double > > arms_;
	double x1_, y1_, x2_, y2_;
};

int main() {
	using namespace std;
	int m;
	cin >> m;
	while( m-- ) {
		Robot robot( cin );
		cout << robot.dfs() << endl;
	}
	return 0;
}

Robot::Robot( std::istream & in ) : arms_(), x1_( 0 ), y1_( 0 ), x2_( 0 ), y2_( 0 ) {
	using namespace std;

	size_t n;
	in >> n >> this->x1_ >> this->y1_ >> this->x2_ >> this->y2_;
	this->x1_ += 10;
	this->y1_ += 10;
	this->x2_ -= 10;
	this->y2_ -= 10;

	double tmp = 0.0;
	generate_n( back_inserter( this->arms_ ), n, [&tmp]()->pair< double, double > {
		tmp += 100.0;
		return std::make_pair( 0.0, tmp );
	} );
}

bool Robot::isInBoundry() const {
	using namespace std;
	pair< double, double > coor( this->arms_.back() );
	return ( coor.first >= this->x1_ ) && ( coor.second >= this->y1_ ) && ( coor.first <= this->x2_ ) && ( coor.second <= this->y2_ );
}

void Robot::rotateLeft( std::size_t arm ) {
	using namespace std;

	Vector::iterator it = this->arms_.begin();
	advance( it, arm );

	pair< double, double > base( 0.0, 0.0 );
	if( arm > 0 ) {
		base = this->arms_.at( arm - 1 );
	}

	for( ; it != this->arms_.end(); ++it ) {
		it->first -= base.first;
		it->second -= base.second;

		double x = LEFT_COS * it->first - LEFT_SIN * it->second;
		double y = LEFT_SIN * it->first + LEFT_COS * it->second;

		it->first = x + base.first;
		it->second = y + base.second;
	}
}

int Robot::dfs() {
	using namespace std;
	size_t minimal = UINT_MAX;
	this->dfs_( 0, 0, minimal );

	return ( minimal == UINT_MAX ) ? ( -1 ) : ( static_cast< int >( minimal ) );
}

void Robot::dfs_( std::size_t begin, std::size_t times, std::size_t & minimal ) {
	using namespace std;

	if( begin == this->arms_.size() || times >= minimal ) {
		return;
	}

	Vector backup( this->arms_ );

	for( size_t i = 0; i < 8; ++i ) {
		size_t diff = ( i > 4 ) ? ( 8 - i ) : ( i );
		if( this->isInBoundry() ) {
			minimal = min( times + diff, minimal );
			this->arms_ = backup;
			return;
		} else {
			this->dfs_( begin + 1, times + diff, minimal );
		}
		this->rotateLeft( begin );
	}
}

void Robot::debug_() const {
	for_each( this->arms_.begin(), this->arms_.end(), []( const std::pair< double, double > & e ) {
		std::cout << "(" << e.first << "," << e.second << ")";
	} );
	std::cout << std::endl;
}

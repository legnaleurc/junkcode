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
	void rotateRight( std::size_t arm );

	std::size_t dfs();

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

// 	this->debug_();
}

void Robot::rotateRight( std::size_t arm ) {
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

		double x = RIGHT_COS * it->first - RIGHT_SIN * it->second;
		double y = RIGHT_SIN * it->first + RIGHT_COS * it->second;

		it->first = x + base.first;
		it->second = y + base.second;
	}

// 	this->debug_();
}

std::size_t Robot::dfs() {
	using namespace std;
	size_t minimal = UINT_MAX;
	this->dfs_( 0, 0, minimal );

	return minimal;
}

void Robot::dfs_( std::size_t begin, std::size_t times, std::size_t & minimal ) {
	using namespace std;

	if( begin == this->arms_.size() ) {
		return;
	}

	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}

	this->rotateLeft( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateLeft( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateLeft( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateLeft( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateRight( begin );
	times -= 1;
	this->rotateRight( begin );
	times -= 1;
	this->rotateRight( begin );
	times -= 1;
	this->rotateRight( begin );
	times -= 1;

	this->rotateRight( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateRight( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateRight( begin );
	times += 1;
	if( this->isInBoundry() ) {
		minimal = min( times, minimal );
		return;
	} else {
		this->dfs_( begin + 1, times, minimal );
	}
	this->rotateLeft( begin );
	times -= 1;
	this->rotateLeft( begin );
	times -= 1;
	this->rotateLeft( begin );
	times -= 1;
}

void Robot::debug_() const {
	for_each( this->arms_.begin(), this->arms_.end(), []( const std::pair< double, double > & e ) {
		std::cout << "(" << e.first << "," << e.second << ")" << std::endl;
	} );
}

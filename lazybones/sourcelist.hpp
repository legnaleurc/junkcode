#include <ostream>
#include <set>
#include <vector>

struct Source {
	Source();
	bool source;
	std::string site;
	std::string part;
	std::set< std::string > sections;
};

std::ostream & operator <<( std::ostream & out, const Source & that );

class SourceList {
public:
	SourceList();
	SourceList( const std::string & sources );

	const Source & operator []( size_t i ) const {
		return this->list_[i];
	}
	size_t size() const {
		return this->list_.size();
	}
private:
	std::vector< Source > list_;
};

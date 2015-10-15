#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>


int main (int argc, char * argv[]) {
    using namespace std;
    typedef char Byte;
    typedef vector<Byte> ByteArray;

    ifstream fin(argv[0], ios::binary);

    if (!fin.is_open()) {
        cerr << "Can not open " << argv[0] << "!" << endl;
        return 1;
    }

    ByteArray binary;
    copy(istreambuf_iterator<Byte>(fin), istreambuf_iterator<Byte>(), back_inserter(binary));
    fin.close();

    string magic = "!_METAMAGIC__";
    magic[0] = '_';

    auto result = search(binary.begin(), binary.end(), magic.begin(), magic.end());
    if (result == binary.end()) {
        cerr << "Can not find piece!" << endl;
        return 1;
    }
    advance(result, magic.size());

    copy(result, binary.end(), ostreambuf_iterator<Byte>(cout));

    return 0;
}

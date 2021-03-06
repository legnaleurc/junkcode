#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <cassert>

#include <uchardet.h>


std::string guess_codec(const std::vector<std::string> & text_list);
std::string guess_codec_2(const std::vector<std::string> & text_list);
std::vector<std::string> read_file_names(const std::string & file_name);
std::string read_local_file(std::istream & is);
uint32_t read_u32(std::istream & is);
uint16_t read_u16(std::istream & is);
std::string read_string(std::istream & is, size_t n);


int main (int argc, char * argv[]) {
    if (argc != 2) {
        return 1;
    }

    auto text_list = read_file_names(argv[1]);
    auto text = guess_codec(text_list);
    std::cout << text << std::endl;
    text = guess_codec_2(text_list);
    std::cout << text << std::endl;

    return 0;
}


std::string guess_codec(const std::vector<std::string> & text_list) {
    using CDSP = std::shared_ptr<std::remove_pointer<uchardet_t>::type>;

    CDSP ucd(uchardet_new(), uchardet_delete);
    if (!ucd) {
        return "cannot alloc uchardet";
    }
    for (const auto & text : text_list) {
        int rv = uchardet_handle_data(ucd.get(), text.c_str(), text.size());
        if (rv != 0) {
            return "cannot handle text";
        }
        rv = uchardet_handle_data(ucd.get(), "\n", 1);
        if (rv != 0) {
            return "cannot handle text";
        }
    }
    uchardet_data_end(ucd.get());
    const char * name = uchardet_get_charset(ucd.get());
    if (!name) {
        return "name is null";
    }
    return name;
}


std::string guess_codec_2(const std::vector<std::string> & text_list) {
    using CDSP = std::shared_ptr<std::remove_pointer<uchardet_t>::type>;

    std::ostringstream sout;
    for (const auto & text : text_list) {
        sout << text << '\n';
    }
    std::string text = sout.str();

    CDSP ucd(uchardet_new(), uchardet_delete);
    if (!ucd) {
        return "cannot alloc uchardet";
    }
    int rv = uchardet_handle_data(ucd.get(), text.c_str(), text.size());
    if (rv != 0) {
        return "cannot handle text";
    }
    uchardet_data_end(ucd.get());
    const char * name = uchardet_get_charset(ucd.get());
    if (!name) {
        return "name is null";
    }
    return name;
}


std::vector<std::string> read_file_names(const std::string & file_name) {
    std::vector<std::string> list;
    std::ifstream fin(file_name, std::ios::binary);
    while (true) {
        uint32_t signature = read_u32(fin);
        if (signature == 0x04034b50) {
            auto file_name = read_local_file(fin);
            list.push_back(file_name);
            continue;
        }
        break;
    }
    fin.close();
    return list;
}


std::string read_local_file(std::istream & is) {
    if (!is.seekg(2, std::ios::cur)) {
        assert(!"seek failed");
    }

    uint16_t general_purpose = read_u16(is);
    assert(!(general_purpose & 0x0008) || !"data");

    if (!is.seekg(10, std::ios::cur)) {
        assert(!"seek failed");
    }

    uint32_t compressed_size = read_u32(is);

    if (!is.seekg(4, std::ios::cur)) {
        assert(!"seek failed");
    }

    uint16_t file_name_length = read_u16(is);

    uint16_t extra_field_length = read_u16(is);

    auto file_name = read_string(is, file_name_length);

    if (!is.seekg(extra_field_length, std::ios::cur)) {
        assert(!"seek failed");
    }

    if (!is.seekg(compressed_size, std::ios::cur)) {
        assert(!"seek failed");
    }

    return file_name;
}


uint32_t read_u32(std::istream & is) {
    uint32_t rv = 0;
    if (!is.read(reinterpret_cast<char *>(&rv), sizeof(rv))) {
        assert(!"read failed");
    }
    return rv;
}


uint16_t read_u16(std::istream & is) {
    uint16_t rv = 0;
    if (!is.read(reinterpret_cast<char *>(&rv), sizeof(rv))) {
        assert(!"read failed");
    }
    return rv;
}


std::string read_string(std::istream & is, size_t n) {
    char rv[n + 1];
    if (!is.read(rv, n)) {
        assert(!"read failed");
    }
    rv[n] = '\0';
    return rv;
}

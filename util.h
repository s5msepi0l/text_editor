#include <iostream>

#include <fstream> // file operations
#include <vector> // data navigation

inline void clrscr() { std::cout<< u8"\033[2J\033[1;1H"; }

typedef struct {
	int row;
	int col;
	int index;
}data_index;


unsigned long get_pos(const char *Src, data_index *self) { //translate col | row into a integer to perform file operations
    std::ifstream fd(Src);
    if (!fd.is_open())
        return -1;

    int curr_row = 1;
    int curr_col = 1;
    char ch;

    while (fd.get(ch)) {
        if (curr_row == self->row && curr_col == self->col) {
            fd.close();
            return fd.tellg();
        }

        if (ch == '\n') {
            curr_row++;
            curr_col = 1;
        } else {
            curr_col++;
        }
    }

    fd.close();

    // If the target position is beyond the end of the file, return the end position
    if (curr_row == self->row && curr_col == self->col)
        return fd.tellg();

    return -1; //specified position is out of bounds
}

static unsigned long fetch_n(const char *path) { // not optimal but does significantly simplify control flow
    std::streampos fsize = 0;
    std::ifstream fd(path);

    fsize = fd.tellg();
    fd.seekg( 0, std::ios::end );
    fsize = fd.tellg() - fsize;
    fd.close();
    return static_cast<unsigned long>(fsize);
}

std::vector<std::string> fetch(const char *path) {
	std::ifstream fd(path);
	std::vector<std::string> buffer;
	if (fd.is_open()) {
		std::string tmp;

		while (fd) {
			std::getline(fd, tmp);
			buffer.push_back(tmp);
		}
	
		fd.close();
		return buffer;
	}

	return buffer;
}
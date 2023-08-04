#include <iostream>
#include <fstream>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <algorithm>

#include "util.h"

enum special_keys {
	LEFT = 37,
	UP = 38,
	RIGHT = 39,
	DOWN = 40,
	ENTER = '\n',
	DELETE = 127,
	ESC = 27
};

class keyboard_input {
private:
	unsigned char ch;
	struct termios old, new0;
	fd_set rfds;

public:
	keyboard_input() {
	    tcgetattr(0, &this->old); /* grab old terminal i/o settings */
	    new0 = old; /* make new settings same as old settings */
	    new0.c_lflag &= ~ICANON; /* disable buffered i/o */
	    new0.c_lflag &= 0 ? ECHO : ~ECHO; /* set echo mode */
	    tcsetattr(0, TCSANOW, &new0); /* use these new terminal i/o settings now */

		FD_ZERO(&rfds);
		FD_SET(0, &rfds);		    	    
	}

	unsigned char getch() {
		read(0, &ch, 1);
		if (ch == 27) {
			
			if (select(1, &rfds, NULL, NULL, NULL) == 0)
				return 27;

			read(0, &ch, 1);

	        if (ch == '[') {
	            // Read the third character to determine the specific key
			    read(0, &ch, 1);


	            switch (ch) {
	                case 'A':
	                    return UP; 
	                case 'B':
	                    return DOWN; 
	                case 'C':
	                    return RIGHT; 
	                case 'D':
	                    return LEFT;
	            }
	        }
		        // If the sequence is not recognized, return 27 (Escape)
		} else if (ch == 10 || ch == 13) { 
	        return ENTER; 
	    }

	    return ch; 
	}
};

//times like this i wish i was coding in java...
class file_handler {
private:
	std::vector<std::string> data;
	std::string path;

	unsigned long size;
public:
	file_handler(const char *Src):
		data(fetch(Src)),
		size(fetch_n(Src)),
		path(Src) {}

	~file_handler() {

	}

	inline std::vector<std::string> get_buffer() {
		return data;
	}

	inline void save() {
		// discard old file content, overwrite
		std::ofstream fd; fd.open(path.c_str(), std::ios::trunc);
		assert(fd.is_open());

		for (int i = 0; i<data.size(); i++) {
			fd << data[i] << '\n';
		}
		fd.close();
	}

	void del(data_index pos) {
		std::cout << "del\n";
		if (data.size() >= pos.col && data[pos.col].size() >= pos.row) {
			data[pos.col].erase(pos.row, 1);

			size--;
		}
	}

	void write(const char ch, data_index pos) { //return value states fd/write status
		data[pos.col].insert(pos.row, 1, ch);

		size++;
	}

};

class fd_editor {
private:
	file_handler fd;
	data_index pos;

	keyboard_input usr_input;

public:	fd_editor(const char *path):
		fd(path), 
		usr_input(),
		pos{0, 0, 0} {}

	void render() {
		std::vector<std::string> buffer = fd.get_buffer();
		for (int i = 0; i<buffer.size(); i++){
			for (int j = 0; j<buffer[i].size(); j++)
			{
				if (i == pos.col && j == pos.row)
					std::cout << "\x1B[32m" << buffer[i][j] <<"\x1B[0m";
				else
					std::cout << buffer[i][j];
				
			}
			std::cout << std::endl;
		}
	}

	void update() { // non I/O blocking
		unsigned char buf = usr_input.getch();
		std::vector<std::string> data = fd.get_buffer();
	    switch(buf) {
			case UP:
				if (pos.col > 0)
					pos.col--;
				break;

			case DOWN:
				if (pos.col < data.size())
				pos.col++;
				break;

			case LEFT:
				if (pos.row > 0)
					pos.row--;
				break;

			case RIGHT:
				if (pos.row < data[pos.col].size())
					pos.row++;
				break;

			case DELETE:
				fd.del(pos);
				pos.row--;
				break;

			case ESC:
				fd.save();
				break;

			default:
				fd.write(buf, pos);
				pos.row++;
				break;
			}
	}

};

int main(int argc, const char **argv) {
	assert(argc > 0);

	fd_editor editor(argv[1]);
	bool running = true;
	
	while (running) {
		editor.update();
		editor.render();
		clrscr();
    }
	
	return 0;
}

#include <stdlib.h>
#include <string>
#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <vector>
using namespace std;

struct Fracture {
    winsize getSize() {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size;
    };
    int getWidth() {
        return getSize().ws_col;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns.
     */
    };
    int getHeight() {
        return getSize().ws_row;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns.
     */
    }
};

struct ScreenCell {
    char character = ' ';
};

struct Screen {
    vector<vector<ScreenCell>> content = {};
    int width;
    int height;

    Screen(int width, int height) {
        content = vector<vector<ScreenCell>>(height); // Give size to outer vector
        for (int y = 0; y < height; y++) {
            content[y] = vector<ScreenCell>(width); // Initialise the width of each row
            for (int x = 0; x < width; x++) {
                content[y][x] = ScreenCell{}; // Initialise each character of the row
            };
        };
        this->width = width;
        this->height = height;
    }

    void draw(int x, int y, char c) {
        content[y][x].character = c;
    }

    vector<string> render() {
        vector<string> outvec(height);
        for (int y = 0; y < height; y++) {
            outvec[y] = getRowAsString(y);
        }
        return outvec;
    }

private:
    string getRowAsString(int y) {
        string row_string;
        for (ScreenCell sc : content[y]) {
            row_string += sc.character;
        }
        return row_string;
    }
};
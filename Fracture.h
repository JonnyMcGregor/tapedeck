#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO
#include <stdlib.h>
#include <vector>

struct Fracture
{
    winsize getSize()
    {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size;
    }

    int getWidth()
    {
        return getSize().ws_col;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns. */
    };
    int getHeight()
    {
        return getSize().ws_row;
        /* size.ws_row is the number of rows, size.ws_col is the number of columns. */
    }
};

struct ScreenCell
{
    char character = ' ';
};

struct Screen
{
    std::vector<ScreenCell> content;

    Screen(int width, int height)
    {
        content.resize(width);
}

    void draw(int x, int y, char c)
    {
        content[x].character = c;
    }

    std::string render()
    {
        std::string output_string;
        for (ScreenCell i : content)
        {
            output_string += i.character;
        }
        return output_string;
    }
};
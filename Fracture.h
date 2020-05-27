#include <sys/ioctl.h> //ioctl() and TIOCGWINSZ
#include <unistd.h>    // for STDOUT_FILENO

class Fracture
{
public:
    winsize getSize()
    {
        winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
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

class Screen
{
}
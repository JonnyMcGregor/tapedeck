struct Rect {

    Rect() {
        Rect{0, 0, 0, 0};
    };

    Rect(int origin_x, int origin_y, int width, int height) {
        x = origin_x;
        y = origin_y;
        this->width = width;
        this->height = height;
    }

    int x = 0, y = 0, width = 0, height = 0;
};
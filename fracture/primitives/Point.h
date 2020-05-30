struct Point {
    int x;
    int y;

    Point() : Point(0, 0) {}

    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }

    void translate(Point p) {
        x = x + p.x;
        y = y + p.y;
    }
};

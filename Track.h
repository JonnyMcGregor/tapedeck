#include <stdlib.h>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include "Clip.h"
class Track
{
public:
    Track(std::string name) { track_name = name; }

    void setName(std::string new_name) { track_name = new_name; }

    void createClip(double start_time)
    {
        int clipNum = ((int)clips.size()) + 1;
        clips.push_back(Clip(track_name + "_clip" + to_string(clipNum), start_time));
    }

    std::vector<Clip> clips;
    bool is_record_enabled = false;

private:
    std::string track_name;
};

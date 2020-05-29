#include <stdlib.h>
#include <vector>
#include "Track.h"
class Session
{
public:
    Session(/* args */);
    ~Session();
    void createTrack()
    {
        tracks.push_back(Track("track" + std::to_string(tracks.size() + 1)));
    }

    enum play_state
    {
        Play,
        Stop,
        Pause,
        Record
    };

private:
    std::vector<Track> tracks;
    double current_time;
};

Session::Session(/* args */)
{
}

Session::~Session()
{
}

#include <fstream>
#include <vector>
class Clip
{
public:
    Clip(std::string name, float time)
    {
        clip_name = name;
        start_time = time;
    }
    ~Clip();

    void addSample(double sample)
    {
        audio_stream.push_back(sample);
    }

    void setName(std::string new_name) { clip_name = new_name; }
    void setStartTime(double time)
    {
        if (time > 0)
            start_time = time;
    }
    void setEndTime(double time)
    {
        if (time > start_time)
            end_time = time;
    }
    std::string getName() { return clip_name; }

private:
    float start_time, end_time, length;
    std::string clip_name;
    std::vector<double> audio_stream;
};
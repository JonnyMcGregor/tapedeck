#pragma once

#include "AudioParams.hpp"
#include "Playhead.hpp"
#include "Track.hpp"
#include "Buffer.hpp"
#include "XmlWrapper.hpp"
#include "WaveFileGenerator/WaveFileGenerator.hpp"
#include <assert.h>
#include <filesystem>

class Session {
public:
    Session(std::string sessionName, AudioParams params);
    ~Session();

    void createProjectFileStructure();
    void loadAllAudioClips();
    void clearAllAudioClips();

    void createTrack();
    void deleteTrack(int index);
    void deleteTrack(std::shared_ptr<Track> track);

    void prepareAudio(int sampleRate, int bufferSize);
    void processAudioBlock(juce::AudioBuffer<float> inputBuffer, juce::AudioBuffer<float> outputBuffer);
    void closeAudio();
    bool isSoloEnabled();

    void createFilesFromRecordedClips();

    void setCurrentTimeInSamples(int newTimeInSamples);
    float getCurrentTimeInSeconds();
    int getCurrentTimeInSamples();

    void loadAudioClip(Clip &clip, int startTimeInReference, int clipLength);
    static double bytesToDouble(unsigned char firstByte, unsigned char secondByte, double maxAmplitude);

    void loadSessionFromXml(string xmlFileName);
    void loadTracks(int numberOfTracks);
    void loadClips(int numberOfClips);
    int numRecordArmedTracks();
    
    float getOutputDB(int channel);

    std::vector<std::shared_ptr<Track>> getSelectedTracks(); 

    enum struct Play_State {
        ToPlay,
        Playing,
        Recording,
        Stopping,
        Stopped,
    };

    Play_State playState = Play_State::Stopped;
    std::vector<std::shared_ptr<Track>> tracks;
    WaveFileGenerator wavFileGen;
    std::unique_ptr<XmlWrapper> sessionXml;
    std::unique_ptr<Playhead> playhead;

private:
    int sampleRate = 0, bufferSize = 0,
          numInputChannels = 0, numOutputChannels = 0, bitDepth = 16;
    ofstream wavFileStreamer;

    juce::LinearSmoothedValue<float> outputDBL = -100, outputDBR = -100;

    std::string sessionName;
    bool isSoloEnabledTracks = false;
};

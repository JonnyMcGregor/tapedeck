#include "Session.hpp"
Session::Session(std::string sessionName, AudioParams params) : sessionName(sessionName), sampleRate(params.sampleRate), bufferSize(params.bufferSize),
                                                                numInputChannels(params.numInputChannels), numOutputChannels(params.numOutputChannels)
{
    juce::FileLogger::getCurrentLogger()->writeToLog("Creating Session...");
    juce::FileLogger::getCurrentLogger()->writeToLog("Session name: " + this->sessionName);
    juce::FileLogger::getCurrentLogger()->writeToLog("Sample Rate: " + juce::String(this->sampleRate));
    juce::FileLogger::getCurrentLogger()->writeToLog("Buffer Size: " + juce::String(this->bufferSize));
    juce::FileLogger::getCurrentLogger()->writeToLog("Num Input Channels: " + juce::String(this->numInputChannels));
    juce::FileLogger::getCurrentLogger()->writeToLog("Num Output Channels: " + juce::String(this->numOutputChannels));

    //Init wave file generator for converting new audio clips to wave files
    wavFileGen.initialise(sampleRate, bitDepth, numInputChannels);
    playhead = std::make_unique<Playhead>(sampleRate);
    createProjectFileStructure();
    std::string xmlFileName = this->sessionName + ".xml";
    sessionXml = std::make_unique<XmlWrapper>(this->sessionName, xmlFileName);
}

Session::~Session() {
    //Clears the Audio clips from temp memory
    clearAllAudioClips();
}

void Session::createProjectFileStructure() {
    if (!filesystem::exists(sessionName))
        filesystem::create_directory(sessionName);
    if (!filesystem::exists(sessionName + "/recorded_audio"))
        filesystem::create_directory(sessionName + "/recorded_audio");
    if (!filesystem::exists(sessionName + "/exported_audio"))
        filesystem::create_directory(sessionName + "/exported_audio");
}

void Session::loadSessionFromXml(string xmlFileName) {
    sessionXml = make_unique<XmlWrapper>(this->sessionName, xmlFileName);
    sessionXml->xmlDoc.LoadFile(sessionXml->fileName.c_str());
    sessionXml->setElements();
    sessionXml->loadPlayheadData(*playhead);
    tracks = sessionXml->loadTracks();
    sessionXml->xmlDoc.SaveFile(sessionXml->fileName.c_str());
}

void Session::clearAllAudioClips() {
    for (auto &track : tracks) {
        for (auto &clip : track->clips) {
            clip->audioData.clear();
        }
    }
}

void Session::createTrack() {
    tracks.push_back(std::shared_ptr<Track>());
    tracks.back() = make_shared<Track>();
    tracks.back()->name = ("Track" + std::to_string(tracks.size()));
}

void Session::deleteTrack(int index) {
    std::vector<std::shared_ptr<Track>>::iterator trackIterator = tracks.begin();
    advance(trackIterator, index);
    tracks.erase(trackIterator);
}
void Session::deleteTrack(std::shared_ptr<Track> trackToDelete)
{
    for (auto it = tracks.begin(); it != tracks.end(); it++) {
        if ((*it).get() == trackToDelete.get()) {
            tracks.erase(it);
        }
    }
}

void Session::prepareAudio(int sampleRate, int bufferSize)
{
    if (this->sampleRate != sampleRate) {
        this->sampleRate = sampleRate;
    }
    if (this->bufferSize != bufferSize) {
        this->bufferSize = bufferSize;
    }

    outputDBL.reset(sampleRate, 0.5);
    outputDBR.reset(sampleRate, 0.5);

    outputDBL.setCurrentAndTargetValue(-100);
    outputDBR.setCurrentAndTargetValue(-100);

    // Create new clip if track is record armed...
    for (auto &track : tracks) {
        if (track->recordArmed) {
            track->createClip(playhead->currentTimeInSamples, (sessionName + "/recorded_audio/"));
        }
    }
}

// Main audio processing callback for engine...
void Session::processAudioBlock(juce::AudioBuffer<float> inputBuffer, juce::AudioBuffer<float> outputBuffer) {
    if (!&inputBuffer || !&outputBuffer) {
        
    }
    for (int sample = 0; sample < bufferSize; sample++, playhead->currentTimeInSamples++) {
        for (int channel = 0; channel < 2; channel++) {
            Sample outputSample = 0;
            //when solo enabled is true all non-solo'd tracks are ignored
            for (auto track : tracks) {
                //input
                if (playState == Play_State::Recording && track->recordArmed && channel == 0) {
                    track->clips.back()->appendSample(inputBuffer.getSample(channel, sample));
                }
                //output
                else {
                    if (track->solo || (!isSoloEnabled() && !track->mute)) {
                        outputSample += track->getSample(getCurrentTimeInSamples());
                    }
                    else {
                        // No change to output sample
                    }
                }
            }
            outputSample.clamp();
            outputBuffer.setSample(channel, sample, outputSample.value);
        }
    }
    outputDBL.skip(bufferSize);
    outputDBR.skip(bufferSize);
    auto value = juce::Decibels::gainToDecibels(outputBuffer.getRMSLevel(0, 0, bufferSize));
    if (value < outputDBL.getCurrentValue()) {
        outputDBL.setTargetValue(value);
    }
    else {
        outputDBL.setCurrentAndTargetValue(value);
    }
    value = juce::Decibels::gainToDecibels(outputBuffer.getRMSLevel(1, 0, bufferSize));
    if (value < outputDBR.getCurrentValue()) {
        outputDBR.setTargetValue(value);
    }
    else {
        outputDBR.setCurrentAndTargetValue(value);
    }
}
void Session::closeAudio()
{
    outputDBR.setTargetValue(-100);
    outputDBL.setTargetValue(-100);
}

bool Session::isSoloEnabled() {
    for (auto &track : tracks) {
        if (track->solo) {
            return true;
        }
    }
    return false;
}

void Session::createFilesFromRecordedClips() {
    for (auto &track : tracks) {
        if (track->recordArmed) {
            for (int i = 0; i < track->clips.size(); i++) {
                if (track->clips[i]->size() > 0) {
                    wavFileStreamer.open(track->clipMetadata[i].referenceFilePath, ios::binary);
                    wavFileGen.openWaveFile(wavFileStreamer);
                    for (int j = 0; j < track->clips[i]->size(); j++) {
                        wavFileGen.writeInputToFile(wavFileStreamer, track->clips[i]->getSample(j).value);
                    }
                    wavFileGen.closeWaveFile(wavFileStreamer);
                    wavFileStreamer.close();
                }
            }
        }
    }
}

int Session::numRecordArmedTracks() {
    int numRecordArmedTracks = 0;
    for (int i = 0; i < tracks.size(); i++) {
        if (tracks[i]->recordArmed) {
            numRecordArmedTracks++;
        }
    }
    return numRecordArmedTracks;
}
float Session::getOutputDB(int channel)
{
    if (channel == 0 || channel == 1) {
        if (playState == Play_State::Stopped) {
            outputDBL.skip(sampleRate / 30);
            outputDBR.skip(sampleRate / 30);
        }
        return channel == 0 ? outputDBL.getCurrentValue() : outputDBR.getCurrentValue();
    }
    else {
        // handle surround at some point
        jassertfalse;
    }
}
u_int Session::getCurrentTimeInSamples() {
	if (playhead)
	{
        return playhead->currentTimeInSamples;
	}
    return 0;
}

void Session::setCurrentTimeInSamples(int newTimeInSamples)
{
    playhead->currentTimeInSamples = newTimeInSamples;
}

float Session::getCurrentTimeInSeconds()
{
    return 1.0f * playhead->currentTimeInSamples / sampleRate;
}

std::vector<std::shared_ptr<Track>> Session::getSelectedTracks() {
    std::vector<std::shared_ptr<Track>> selectedTracks;
    for (auto& track : tracks) {
        if (track->isSelected) {
            selectedTracks.push_back(track);
        }
    }
    return selectedTracks;
}
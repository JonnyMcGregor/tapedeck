#include "Session.hpp"
Session::Session(std::string sessionName, AudioParams params) {
    
    this->sessionName = sessionName;
    this->sampleRate = params.sampleRate;
    this->bufferSize = params.bufferSize;
    this->numInputChannels = params.numInputChannels;
    this->numOutputChannels = params.numOutputChannels;
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

void Session::prepareAudio() {
    for (auto &track : tracks) {
        if (track->recordArmed) {
            track->createClip(playhead->currentTimeInSamples, (sessionName + "/recorded_audio/"));
        }
    }
}
void Session::processAudioBlock(Buffer &inputBuffer, Buffer &outputBuffer) {
    for (int sample = 0; sample < bufferSize; sample++, playhead->currentTimeInSamples++) {
        for (int channel = 0; channel < 2; channel++) {
            Sample outputSample;
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
                }
            }
            outputBuffer.setSample(channel, sample, outputSample.value);
        }
    }
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
    assert(playState == Play_State::Stopping);
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
u_int Session::getCurrentTimeInSamples() {
    return playhead->currentTimeInSamples;
}

float Session::getCurrentTimeInSeconds() {
    return 1.0f * playhead->currentTimeInSamples / sampleRate;
}

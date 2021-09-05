#include "WaveFile.hpp"
#include "tinyxml2/tinyxml2.hpp"
#include "Playhead.hpp"
#include "Track.hpp"
#include <filesystem>
using namespace tinyxml2;

struct XmlWrapper {
    XmlWrapper(std::string session_name, std::string file_name) {
        this->sessionName = session_name;
        this->fileName = session_name + "/" + file_name;
    }

    void createXmlDocument() {
        if (!std::filesystem::exists(sessionName)) {
            std::filesystem::create_directory(sessionName);
        }
        sessionNode = xmlDoc.NewElement("Session");
        xmlDoc.InsertFirstChild(sessionNode);
        xmlDoc.SaveFile(fileName.c_str());
    }

    void refreshXmlDocument(Playhead &playhead, std::vector<std::shared_ptr<Track>> tracks) {
        xmlDoc.FirstChild()->DeleteChildren();
        addPlayheadElement(playhead, tracks.size());
        for (auto &track : tracks) {
            addTrackDataToXml(*track);
        }
        xmlDoc.SaveFile(fileName.c_str());
    }
    void setElements() {
        sessionNode = xmlDoc.RootElement();
        playheadElement = sessionNode->FirstChildElement();
        trackElement = playheadElement->NextSiblingElement();
    }

    void addPlayheadElement(Playhead playhead, int number_of_tracks) {
        playheadElement = xmlDoc.NewElement("playhead");
        playheadElement->SetAttribute("tempo", std::to_string(playhead.tempo).c_str());
        playheadElement->SetAttribute("time_sig_numerator", std::to_string(playhead.timeSigNum).c_str());
        playheadElement->SetAttribute("time_sig_denominator", std::to_string(playhead.timSigDenom).c_str());
        playheadElement->SetAttribute("current_time_samples", std::to_string(playhead.currentTimeInSamples).c_str());
        playheadElement->SetAttribute("current_time_seconds", std::to_string(playhead.currentTimeInSamples).c_str());
        playheadElement->SetAttribute("number_of_tracks", number_of_tracks);
        sessionNode->InsertEndChild(playheadElement);
    }
    void addTrackDataToXml(Track &track) {
        trackElement = xmlDoc.NewElement("track");
        trackElement->SetAttribute("name", track.name.c_str());
        trackElement->SetAttribute("is_record", std::to_string(track.recordArmed).c_str());
        trackElement->SetAttribute("is_solo", std::to_string(track.solo).c_str());
        trackElement->SetAttribute("is_mute", std::to_string(track.mute).c_str());
        trackElement->SetAttribute("number_of_clips", track.clips.size());
        sessionNode->InsertEndChild(trackElement);
        for (int i = 0; i < track.clips.size(); i++) {
            addClipToTrackElement(*track.clips[i], track.clipMetadata[i]);
        }
    }

    void addClipToTrackElement(Clip &clip, ClipMetadata &clip_metadata) {
        clipElement = xmlDoc.NewElement("clip");
        clipElement->SetAttribute("reference_file_path", (clip_metadata.referenceFilePath).c_str());
        clipElement->SetAttribute("start_time_in_session", clip_metadata.startTime);
        clipElement->SetAttribute("start_time_in_reference", clip_metadata.startTimeInReference);
        clipElement->SetAttribute("length_in_samples", clip.size());
        trackElement->InsertEndChild(clipElement);
    }

    void loadPlayheadData(Playhead &playhead) {
        playhead.movePlayhead(playheadElement->FindAttribute("current_time_samples")->IntValue());
        playhead.tempo = playheadElement->FindAttribute("tempo")->IntValue();
        playhead.timeSigNum = playheadElement->FindAttribute("time_sig_numerator")->IntValue();
        playhead.timSigDenom = playheadElement->FindAttribute("time_sig_denominator")->IntValue();
    }

    std::vector<std::shared_ptr<Track>> loadTracks() {
        std::vector<std::shared_ptr<Track>> tracks;
        for (int i = 0; i < getNumTracks(); i++) {
            tracks.push_back(std::shared_ptr<Track>());
            tracks.back() = std::make_shared<Track>();
            tracks.back()->name = ("Track" + std::to_string(tracks.size()));
            loadTrackData(*tracks.back());
        }
        return tracks;
    }

    void loadTrackData(Track &track) {
        for (int i = 0; i < trackElement->FindAttribute("number_of_clips")->IntValue(); i++) {
            track.createClip(clipElement->FindAttribute("start_time_in_session")->IntValue(), (sessionName + "/recorded_audio/"));
            loadAudioClip(track.clips.back(), track.clipMetadata.back());
        }
        trackElement = trackElement->NextSiblingElement();
    }

    void loadAudioClip(std::shared_ptr<Clip> clip, ClipMetadata metadata) {
        WaveFile wave_file = {metadata.referenceFilePath};
        clip->clear();
        for (int i = 0; i < clip->size(); i++) {
            int first_sample_index = 44;
            //each audio sample is 2 bytes therefore index is multiplied by 2 to get byte index of sample
            int sample_pos = first_sample_index + metadata.startTimeInReference + (i * 2);
            //as samples are stored as little endian, both bytes within sample must be retrieved individually
            double max_amplitude = wave_file.readBytePair(34);
            double raw_sample = wave_file.readBytePair(sample_pos);
            //sample data is then converted to its original double between 1.0 and -1.0
            clip->appendSample(Sample(raw_sample / max_amplitude));
        }
    }

    int getNumTracks() {
        return playheadElement->FindAttribute("number_of_tracks")->IntValue();
    }

    int getSelectedClipLength() {
        return clipElement->FindAttribute("length_in_samples")->IntValue();
    }

    XMLDocument xmlDoc;
    XMLNode *sessionNode;
    XMLElement *trackElement, *clipElement, *playheadElement;
    std::string sessionName, fileName;
};

#include "Session.h"
#include "tiny_xml_2/tinyxml2.h"
#include <experimental/filesystem>
using namespace tinyxml2;

class XmlWrapper {
public:
    XmlWrapper(std::string project_name, std::string file_name, Session &session) {
        this->project_name = project_name;
        this->file_name = project_name + "/" + file_name;
        this->session = &session;
    }
    void createXMLDocument() {
        if (!std::experimental::filesystem::exists(project_name)) {
            std::experimental::filesystem::create_directory(project_name);
        }
        xml_doc.InsertFirstChild(session_node);
        xml_doc.SaveFile(file_name.c_str());
    }

    void addTrackToXML(Track &track) {
        track_element = xml_doc.NewElement("track");
        track_element->SetAttribute("name", track.getName().c_str());
        session_node->InsertEndChild(track_element);
        xml_doc.SaveFile(file_name.c_str());
    }

    void addClipToXML(Clip &clip) {
        clip_element = xml_doc.NewElement("clip");
        addAttributesToClipElement(clip);
        std::string clip_name = clip_element->FindAttribute("name")->Value();
        addClipToTrackElement(clip_name);
        xml_doc.SaveFile(file_name.c_str());
    }

    void addAttributesToClipElement(Clip &clip) {
        clip_element->SetAttribute("name", clip.getName().c_str());
        clip_element->SetAttribute("reference_file_path", clip.getReferenceFilePath().c_str());
        clip_element->SetAttribute("start_time_in_session", clip.getStartTime());
        clip_element->SetAttribute("start_time_in_reference", clip.getStartTimeInReference());
        clip_element->SetAttribute("length_in_samples", clip.getNumSamples());
    }
    void addClipToTrackElement(std::string clip_name) {
        track_element = session_node->FirstChildElement();

        int return_type;
        //Use name attribute to identify which track the clip belongs to..
        for (int i = 0; i < session->tracks.size(); i++) {
            int name_length = session->tracks[i].getName().length();
            //if name of track element matches the clip name, add clip element to track element
            if (track_element->FindAttribute("name")->Value() == clip_name.substr(0, name_length)) {
                if (track_element->QueryAttribute(clip_element->FindAttribute("name")->Value(), &return_type) != tinyxml2::XMLError::XML_NO_ATTRIBUTE) {
                    track_element->DeleteChild(track_element->FirstChildElement());
                    track_element->InsertEndChild(clip_element);
                } else {
                    track_element->InsertEndChild(clip_element);
                }

            }
            //else move on to next track element
            else {
                track_element = track_element->NextSiblingElement();
            }
        }
    }

private:
    XMLDocument xml_doc;
    XMLNode *session_node = xml_doc.NewElement("Session");
    XMLElement *track_element, *clip_element;
    std::string project_name, file_name;
    Session *session;
};

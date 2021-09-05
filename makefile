FRACTURE_INCLUDES = Fracture/Widgets/Tapedeck.cpp
SEISMIC_INCLUDES = Seismic/Components/tinyxml2/tinyxml2.cpp Seismic/Components/Session.cpp 
JUCE_INCLUDES = Seismic/juce_modules/juce_core/juce_core.cpp Seismic/juce_modules/juce_audio_basics/juce_audio_basics.cpp Seismic/juce_modules/juce_audio_devices/juce_audio_devices.cpp Seismic/juce_modules/juce_events/juce_events.cpp
LINKS = -lasound -ldl -lcurl -lstdc++fs

asciidaw: main.cpp fracture/* fracture/components/* seismic/components/*
	g++ -g main.cpp seismic/components/Session.cpp seismic/components/tiny_xml_2/tinyxml2.cpp -l rtaudio -l stdc++fs -Wno-return-type -o asciidaw

tapedeck: main.cpp Fracture/* Fracture/Components/* Seismic/Components/*
	g++ -g main.cpp -pthread  -std=c++17 $(FRACTURE_INCLUDES) $(SEISMIC_INCLUDES) $(JUCE_INCLUDES) $(LINKS) -o tapedeck
asciidaw: main.cpp fracture/* fracture/components/* seismic/components/*
	g++ -g main.cpp seismic/components/Session.cpp -l rtaudio -l stdc++fs -Wno-return-type -o asciidaw

CXX = clang++ 
CFLAGS = `pkg-config --cflags --libs /usr/local/Cellar/opencv/4.0.1/lib/pkgconfig/opencv4.pc` -O3 -std=c++14

all: detect video

detect: detect.cpp
	$(CXX) $(CFLAGS) detect.cpp -o bin/detect

video: video_test.cpp
	$(CXX) $(CFLAGS) video_test.cpp -o bin/video

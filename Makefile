CXX = clang++ 
CFLAGS = `pkg-config --cflags --libs /usr/local/Cellar/opencv/4.0.1/lib/pkgconfig/opencv4.pc` -O3 -std=c++14

all: smartfan

detect: detect.cpp
	$(CXX) $(CFLAGS) detect.cpp -o bin/detect

video: video_test.cpp
	$(CXX) $(CFLAGS) video_test.cpp -o bin/video

webcam: webcam.cpp
	$(CXX) $(CFLAGS) webcam.cpp -o bin/webcam

smartfan: smart_fan.cpp dectector.cpp
	$(CXX) $(CFLAGS) -shared -fPIC -o libsmartfan.so smart_fan.cpp dectector.cpp

test: smart_fan.cpp smart_fan_test.cpp dectector.cpp
	$(CXX) $(CFLAGS) smart_fan_test.cpp smart_fan.cpp dectector.cpp -o bin/test

all: testRTSPClient

testRTSPClient: testRTSPClient.cpp
	$(CXX) -o $@ $< $$(pkg-config --cflags --libs live555) -lboost_regex

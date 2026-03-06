test_hls:test_hls.cc ../../source/log.cc ../../source/hls.cc ../../source/util.cc
	g++ -g -std=c++17 $^ -o $@ -lavcodec -lavformat -lavutil -lfmt -lspdlog -ljsoncpp
clean:
	rm -rf test_hls
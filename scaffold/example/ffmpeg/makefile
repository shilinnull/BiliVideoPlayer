all: hls
hls:hls.cc
	g++ -g -std=c++17 $^ -o $@ -lavcodec -lavformat -lavutil
clean:
	rm -rf hls
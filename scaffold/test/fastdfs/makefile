all: test
test:test.cc ../../source/fdfs.cc ../../source/util.cc ../../source/log.cc ../../source/util.cc
	g++ -std=c++17 $^ -o $@ -lfdfsclient -lfastcommon -pthread -lfmt -lspdlog -ljsoncpp -lpthread
clean:
	rm -rf test

all: test
test: test.cc ../../source/log.cc ../../source/elastic.cc ../../source/util.cc
	g++ -std=c++17 $^ -o $@ -lcpr -ljsoncpp -lelasticlient -lprotobuf -lpthread -ldl -lssl -lgflags -lfmt -lspdlog
clean:
	rm -rf test
json_test: json_test.cc ../../source/util.cc  ../../source/log.cc
	g++ -std=c++17 $^ -o $@ -ljsoncpp -lfmt -lspdlog -lpthread
clean:
	rm -rf json_test
CFLAGS=-I../../source/ -std=c++17
log_test:log_test.cc ../../source/log.cc
	g++ $(CFLAGS) $^ -o $@ -lspdlog -lgflags -lfmt -pthread
clean:
	rm -rf log_test
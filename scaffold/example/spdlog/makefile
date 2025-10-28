CFLAGS=-I. -std=c++17
main:main.cc log.cc
	g++ $(CFLAGS) $^ -o $@ -lspdlog -lgflags -lfmt -pthread
clean:
	rm -rf main
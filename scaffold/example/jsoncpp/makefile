all: test1 test2 test3 serialize unserialize
serialize: serialize.cc
	g++ -std=c++11 $^ -o $@ -ljsoncpp
unserialize: unserialize.cc
	g++ -std=c++11 $^ -o $@ -ljsoncpp
test1:test1.cc
	g++ -std=c++17 $^ -o $@ -ljsoncpp
test2:test2.cc
	g++ -std=c++17 $^ -o $@ -ljsoncpp
test3:test3.cc
	g++ -std=c++17 $^ -o $@ -ljsoncpp
clean:
	rm -rf test1 test2 test3 serialize unserialize
all: discovery registry
discovery:discovery.cc ../../source/etcd.cc ../../source/log.cc ../../source/util.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api -lspdlog -lfmt -ljsoncpp
registry:registry.cc ../../source/etcd.cc ../../source/log.cc ../../source/util.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api -lspdlog -lfmt -ljsoncpp
clean:
	rm -rf discovery registry
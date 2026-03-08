all: client server
client: client.cc cal.pb.cc ../../source/log.cc ../../source/rpc.cc
	g++ -std=c++17 $^ -o $@ -lbrpc -lleveldb -lprotobuf -lpthread -ldl -lssl -lcrypto -lgflags -lfmt -lspdlog
server: server.cc cal.pb.cc ../../source/log.cc ../../source/rpc.cc
	g++ -std=c++17 $^ -o $@ -lbrpc -lleveldb -lprotobuf -lpthread -ldl -lssl -lcrypto -lgflags -lfmt -lspdlog
%.pb.cc: %.proto
	protoc --cpp_out=./ $^
clean:
	rm -rf *.pb.* client server
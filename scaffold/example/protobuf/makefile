.all: serialize %.pb.cc
serialize: serialize.cc person.pb.cc
	g++ -o $@ $^ -std=c++17 -lpthread -lprotobuf
%.pb.cc: %.proto
	protoc -I ./ --cpp_out=. $^
.PHONY: clean
clean:
	rm -f serialize *.pb.*

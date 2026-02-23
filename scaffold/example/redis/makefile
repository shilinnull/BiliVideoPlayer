all: string list hash set zset complement pipeline transaction watch
string:string.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
list:list.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
hash:hash.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
set:set.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
zset:zset.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
complement:complement.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
pipeline:pipeline.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
transaction:transaction.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
watch:watch.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
.PHONY:clean
clean:
	rm -f string list hash set zset complement pipeline transaction watch
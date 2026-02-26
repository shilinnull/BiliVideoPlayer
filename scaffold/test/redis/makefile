test: test.cc ../../source/redis.cc
	g++ -std=c++17 $^ -o $@ -lhiredis -lredis++ -lpthread
.PHONY:clean
clean:
	rm -f test
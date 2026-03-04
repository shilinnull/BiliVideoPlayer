all: simple_publish simple_subscribe delayed_publish delayed_subscribe
simple_publish:simple_publish.cc  ../../source/mq.cc ../../source/log.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread -lfmt
simple_subscribe:simple_subscribe.cc  ../../source/mq.cc ../../source/log.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread -lfmt
delayed_publish:delayed_publish.cc ../../source/mq.cc ../../source/log.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread -lfmt
delayed_subscribe:delayed_subscribe.cc ../../source/mq.cc ../../source/log.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread -lfmt
clean:
	rm -rf simple_publish simple_subscribe delayed_subscribe
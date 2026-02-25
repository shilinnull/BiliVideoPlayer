all: simple_publish simple_subscribe dlx_publish dlx_subscribe
simple_publish:simple_publish.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread
simple_subscribe:simple_subscribe.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread

dlx_publish:dlx_publish.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread
dlx_subscribe:dlx_subscribe.cc
	g++ -std=c++17 $^ -o $@ -lamqpcpp -lev -lpthread
clean:
	rm -rf simple_publish simple_subscribe dlx_publish dlx_subscribe
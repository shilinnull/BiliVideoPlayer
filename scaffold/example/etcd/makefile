all: simple_get simple_put dir_get dir_put
simple_get:simple_get.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api
simple_put:simple_put.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api
dir_get:dir_get.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api
dir_put:dir_put.cc
	g++ -std=c++17 $^ -o $@ -lcpprest -letcd-cpp-api
clean:
	rm -rf simple_get simple_put dir_get dir_put
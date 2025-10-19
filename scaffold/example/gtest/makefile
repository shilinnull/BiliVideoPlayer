environment: environment.cc
	g++ -o environment environment.cc -lgtest -pthread

global: global.cc
	g++ -o global global.cc -lgtest -pthread

simple: simple.cc
	g++ -o simple simple.cc -lgtest -pthread

.PHONY: clean
clean:
	rm -f global simple environment
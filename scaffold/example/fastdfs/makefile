all: upload download remove
upload:upload.cc
	g++ -std=c++11 $^ -o $@ -lfdfsclient -lfastcommon -pthread
download:download.cc
	g++ -std=c++11 $^ -o $@ -lfdfsclient -lfastcommon -pthread
remove:remove.cc
	g++ -std=c++11 $^ -o $@ -lfdfsclient -lfastcommon -pthread
clean:
	rm -rf upload download remove

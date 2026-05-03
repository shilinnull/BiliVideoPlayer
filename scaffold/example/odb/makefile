test:test.cpp person-odb.cxx
	g++ -std=c++17 $^ -o $@ -lodb-mysql -lodb -lodb-boost -lpthread
person-odb.cxx: person.hxx
	odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time $^
clean:
	rm -rf *-odb.*  *.sql test

odb_test:odb_test.cc student-odb.cxx ../../source/odb.cc
	g++ -std=c++17 $^ -o $@ -lodb-mysql -lodb -lodb-boost -lpthread
student-odb.cxx: student.h
	odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time $^
clean:
	rm -rf odb_test *-odb.*  student.sql
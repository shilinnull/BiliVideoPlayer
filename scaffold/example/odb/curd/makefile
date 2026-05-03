curd:curd.cc student-odb.cxx
	g++ -std=c++17 $^ -o $@ -lodb-mysql -lodb -lodb-boost -lpthread
student-odb.cxx: student.h
	odb -d mysql --std c++11 --generate-query --generate-schema --profile boost/date-time $^
clean:
	rm -rf *-odb.*  *.sql curd

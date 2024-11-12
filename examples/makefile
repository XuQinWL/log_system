test:test.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp -L../lib

test_:test.cpp	
	g++ -g -o $@ $^ -std=c++11 -lpthread -ljsoncpp -L../lib

.PHONY:clean
clean:
	rm -f test test_ logfile.log

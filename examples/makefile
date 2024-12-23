test:test.cpp
	g++ -o $@ $^ -std=c++11 -lpthread -ljsoncpp -L../lib

gdb:test.cpp	
	g++ -g -o $@ $^ -std=c++11 -lpthread -ljsoncpp -L../lib

.PHONY:clean
clean:
	rm -rf test gdb logfile

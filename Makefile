input_files_test: 
	g++ -O3 -Wall -o input_files_test input_files_test.cc src/GEvent.cc `root-config --cflags --glibs` -Iinclude/

clean:
	rm -f input_files_test

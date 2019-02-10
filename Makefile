all:
	rm -rf build
	mkdir build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make

debug:
	rm -rf build
	mkdir build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

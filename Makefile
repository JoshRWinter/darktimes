.PHONY: debug clean

all:
	rm -rf build
	mkdir build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && make
	rm -rf release
	mkdir release
	mv build/darktimes release
	mv build/assets release
	strip release/darktimes

debug:
	rm -rf build
	mkdir build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

clean:
	rm -rf build

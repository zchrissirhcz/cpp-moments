BUILD_DIR=build

all:
	cmake -S . -B ${BUILD_DIR}
	cmake --build ${BUILD_DIR}
	./testbed

.PHONY : testbed
testbed:
	cmake --build ${BUILD_DIR} --target testbed
	./testbed

.PHONY : clean
clean:
	cmake --build ${BUILD_DIR} --target clean%   

.PHONY : compare
compare:
	./testbed > gtest.log 2>&1 ; ./qtest > qtest.log 2>&1
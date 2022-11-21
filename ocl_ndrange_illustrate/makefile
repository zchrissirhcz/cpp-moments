UNAME := $(shell uname)

ifeq ($(UNAME), Linux)
	#OPENCL_CONFIGS = -I/usr/local/cuda-11.8/targets/x86_64-linux/include -L/usr/local/cuda-11.8/targets/x86_64-linux/lib -libOpenCL
	OPENCL_CONFIGS = -I/usr/local/cuda-11.8/targets/x86_64-linux/include -L/usr/lib/x86_64-linux-gnu/ -lOpenCL
	COMPILER = nvcc
endif
ifeq ($(UNAME), Darwin) # macOS
	OPENCL_CONFIGS = -framework OpenCL
	COMPILER = g++ -std=c++11
endif

OPENCV_CONFIGS = `pkg-config opencv4 --libs --cflags`

ASAN = "-fno-omit-frame-pointer -fsanitize=address"
TSAN = "-fno-omit-frame-pointer -fsanitize=thread"
SANITIZER = $(ASAN) # nvcc does not recognize sanitizer options

all:
	${COMPILER} --std=c++11 main.cpp ${OPENCL_CONFIGS} ${OPENCV_CONFIGS}

.PHONY: clean

clean:
	rm a.out
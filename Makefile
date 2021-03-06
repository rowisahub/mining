CXX = clang++-14
override CXXFLAGS += -g -Wno-everything
override CXXPOSTF += -pthread bLibs/builds/RandomX/librandomx.a

SRCS = $(shell find . -type d \( -path ./bLibs/RandomX -o -name '.ccls-cache' \) -prune -o -type f -name '*.cpp' -print)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#echo "Building..."

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o main $(CXXPOSTF)

clean:
	rm -f $(OBJS) main


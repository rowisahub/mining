CXX = clang++-14
override CXXFLAGS += -g -Wno-everything
override CXXPOSTF += -pthread bLibs/RandomX/build/librandomx.a

SRCS = $(shell find . -type d \( -path ./bLibs -o -name '.ccls-cache' \) -prune -o -type f -name '*.cpp' -print)
OBJS = $(patsubst %.cpp, %.o, $(SRCS))

#echo "Building..."

main: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o main $(CXXPOSTF)

clean:
	rm -f $(OBJS) main


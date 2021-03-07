src = $(wildcard src/*.cpp)
obj = $(src:.cpp=.o)

CXXFLAGS = -std=c++1z -Wall -g -Ofast

exe: $(obj)
	$(CXX) -g -o $@ $^ $(CXXFLAGS)
clean:
	rm -rf *o exe
	rm -rf src/*.o

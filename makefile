CXX = g++
LDFLAGS = `pkg-config --libs sdl2`
CXXFLAGS = `pkg-config --cflags sdl2` -std=c++11
TARGET = glib
SRCS = main.cpp display.cpp container.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS   = $(SRCS:.cpp=.depends)

.PHONY: clean all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.depends: %.cpp
	$(CXX) -M $(CXXFLAGS) $< > $@

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

-include $(DEPS)

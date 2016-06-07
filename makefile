CXX = g++
LDFLAGS = `pkg-config --libs sdl2`
CXXFLAGS = `pkg-config --cflags sdl2` -std=c++11 -Ofast
TARGET = bin/glib
LIBSRCS = $(wildcard lib/src/*.cpp)
LIBOBJS = $(addprefix lib/obj/,$(notdir $(LIBSRCS:.cpp=.o)))
LIBDEPS = $(addprefix lib/obj/,$(notdir $(LIBSRCS:.cpp=.d)))

PRGSRCS = $(wildcard src/*.cpp)
PRGOBJS = $(addprefix obj/,$(notdir $(PRGSRCS:.cpp=.o)))
PRGDEPS = $(addprefix obj/,$(notdir $(PRGSRCS:.cpp=.d)))

#$(info ${OBJS})

.PHONY: clean all lib

all: $(TARGET)

lib: $(LIBOBJS)

$(TARGET): lib $(PRGOBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LIBOBJS) $(PRGOBJS) -o $(TARGET) 

obj/%.o : src/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

lib/obj/%.o : lib/src/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

-include $(LIBDEPS)
-include $(PRGDEPS)

clean:
	rm -f $(LIBOBJS) $(PRGOBJS) $(LIBDEPS) $(PRGDEPS) $(TARGET)

COMP = g++
LIBS = `pkg-config --libs sdl2`
CFLAGS = `pkg-config --cflags sdl2` -std=c++11
SRCS = main.cpp display.cpp container.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = glib

out: $(OBJS)
	$(COMP) -o $(TARGET) $(OBJS) $(LIBS)

$($(COMP) -M $(SRCS))

clean:
	rm $(OBJS) $(TARGET)

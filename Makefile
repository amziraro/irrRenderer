TARGET := irrRenderer

SOURCES := $(wildcard source/*.cpp)
OBJS := $(patsubst source/%.cpp,obj/%.o,$(SOURCES))
DEBUGOBJS := $(patsubst source/%.cpp,obj/debug/%.o,$(SOURCES))

IRRLICHT := "../irrlicht-1.9"

CPPFLAGS := -Iinclude/ -Wall -O2 -ffast-math -MMD -fPIC -I$(IRRLICHT)/include 
DEBUGFLAGS = -g

## Horrible quick hack to allow crpss-compilation from  Linux to Windows using mingw64
# use `make all_linux` to get linux libs
# use `make all_win32` to get windows libs
# when switching between use `make clean` otherwise things might go awry

all: all_linux

all_linux: CXX = g++
all_linux: CPPFLAGS += -I/usr/X11R6/include
all_linux: PLATFORM = linux
all_linux: release debug

all_win32: CXX = x86_64-w64-mingw32-g++
all_win32: PLATFORM = win32-gcc
all_win32: release debug

debug: $(DEBUGOBJS)
	mkdir -p lib/$(PLATFORM)/debug
	ar -rs lib/$(PLATFORM)/debug/lib$(TARGET).a $^

release: $(OBJS)
	mkdir -p lib/$(PLATFORM)/release
	ar -rs lib/$(PLATFORM)/release/lib$(TARGET).a $^

obj/%.o: source/%.cpp
	mkdir -p $$(dirname $@)
	g++ $(CPPFLAGS) -c -o $@ $<

obj/debug/%.o: source/%.cpp
	mkdir -p $$(dirname $@)
	g++ $(CPPFLAGS) $(DEBUGFLAGS) -c -o $@ $<

-include $(OBJS:.o=.d)

clean:
	rm -rf obj


.PHONY: all debug release clean

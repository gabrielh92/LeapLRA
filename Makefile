EXENAME = leapLRA

COMPILER = g++
WARNINGS = -Wall -g -std=c++98 
LINKER = g++

MAIN_OBJS = main.o serialArd.o leaplistener.o
MAIN_DEPS = serialArd.o leaplistener.o serialArd.h serialArd.cpp leaplistener.h leaplistener.cpp

LEAP_DEPS = leaplistener.cpp leaplistener.h serialArd.cpp serialArd.h
SERIAL_DEPS = serialArd.cpp serialArd.h
CODE_CLEAN = *.o $(EXENAME)

all: $(EXENAME)

$(EXENAME) : main.o
	$(LINKER) $(MAIN_OBJS) -I include -o $(EXENAME) libLeap.dylib

main.o : $(MAIN_DEPS)
	$(COMPILER) $(WARNINGS) -c main.cpp

leaplistener.o : $(LEAP_DEPS)
	$(COMPILER) $(WARNINGS) -c leaplistener.cpp

serialArd.o : $(SERIAL_DEPS)
	$(COMPILER) $(WARNINGS) -c serialArd.cpp


clean: 
	-rm -f -r $(CODE_CLEAN)
	clear
	ls

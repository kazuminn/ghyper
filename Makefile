TAR	= x86 
BIN	= test03.bin
BINSRC = test03.c

OBJS	= test/test.cpp main.cpp Emulator.cpp ModRM.o interrupt.cpp pc.o hinstruction16.cpp ghyper.cpp
OBJS	+= GUI.cpp eflags.cpp
OBJS	+= kazuminlib/kazuminlib.a
OBJS	+= device/Device.a
#OBJS	+= GL/lib/libglut.a

CFLAGS	= -std=c++11 -g
LDFLAGS	= -lglut -lGLU -lGL
LDFLAGS += -pthread
RUNFLAGS= $(BIN)

NASK	= ~/tolset/z_tools/nask
CC	= gcc
CXX	= g++

%.o:%.cpp
	$(CXX) -DPRINT -o $@ -c $< $(CFLAGS)

%.bin:%.nask
	$(NASK) $< $@ $*.lst

default:
	make $(BIN)
	make -C kazuminlib 
	make -C device
	make $(TAR)

run:$(TAR) $(BIN)
	make
	./$(TAR) $(RUNFLAGS)

clean:
	make -C kazuminlib clean
	make -C device clean
	rm *.o
	rm x86 

full:
	make clean
	make

$(TAR):$(OBJS)
	$(CXX)  -o $@ $^ $(LDFLAGS) -lgtest

device/Device.a:
	make -C device

$(BIN):$(BINSRC)
	gcc -m32 -nostdlib -fno-asynchronous-unwind-tables -I~/tolset_p86/z_tools/i386-elf-gcc/include\
	 -g -fno-stack-protector -c $(BINSRC)	
	ld --entry=start --oformat=binary -Ttext 0x7c00 -o $@ $(BINSRC:.c=.o)

crt0.o:crt0.asm
	nasm -f elf crt0.asm

test:$(OBJS)
	$(CXX) -g  -DDEBUG $(OBJS) $(LDFLAGS)   -lgtest -lpthread -o t 

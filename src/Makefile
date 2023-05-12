
VER=0.2

GCC=g++
RC=windres
EXE=watch.v${VER}.scr
LDFLAGS=-s -O2 -mwindows -lgdiplus  -lscrnsave -lopengl32 -lglu32 -static-libgcc -static-libstdc++
CXXFLAGS=-O2 -Wl,-rpath
OBJS=main_scr.o resource.res

.PHONY:all clean resource.res run

all:${EXE}

clean:
	rm -rf ${EXE}
	rm -rf ${OBJS}

%.o: %.cpp
	${GCC} ${CXXFLAGS} -c "$<" -o "$@"

resource.res: resources.rc main.def icon.ico resource.h
	${RC} -i "$<" -O coff -o "$@"

${EXE}: ${OBJS}
	${GCC} ${OBJS} -o "$@" ${LDFLAGS} 
	${EXE} -c


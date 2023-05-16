
VER=0.4

GCC=g++
RC=windres
EXE=watch.v${VER}.scr
LDFLAGS=-s -O2 -mwindows -lgdiplus -lopengl32 -lglu32 -static-libgcc -static-libstdc++
CXXFLAGS=-O2 -Wl,rpath,--subsystem,windows -municode,-rpath
OBJS=main_win.o resource.res

.PHONY:all clean resource.res run

all:${EXE}

clean:
	rm -rf ${EXE}
	rm -rf ${OBJS}

%.o: %.cpp
	${GCC} ${CXXFLAGS} -c "$<" -o "$@"

resource.res: resources.rc icon.w.ico resource.h
	${RC} -i "$<" -O coff -o "$@"

${EXE}: ${OBJS}
	${GCC} ${OBJS} -o "$@" ${LDFLAGS} 
	${EXE} -c


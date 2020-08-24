CXX = g++
CXXFLAGS = -std=c++14 -MMD -lfftw3 -Wl,-rpath=/usr/local/lib
EXEC = WaveformPlayer
OBJECTS = WaveformPlayer.o WAVFile.o WAVPlayer.o Display.o WaveformDisplay.o FFTWwrap.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC} : ${OBJECTS}
	${CXX} ${OBJECTS} -lportaudio -lrt -lm -lasound -pthread -lncurses -o ${EXEC} ${CXXFLAGS}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}

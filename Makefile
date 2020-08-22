CXX = g++
CXXFLAGS = -std=c++14 -Wall -MMD -lfftw3
EXEC = WaveformPlayer
OBJECTS = WaveformPlayer.o WAVFile.o WAVPlayer.o Display.o WaveformDisplay.o DFT.o FFTWwrap.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC} : ${OBJECTS}
	${CXX} ${OBJECTS} libportaudio.a -lrt -lm -lasound -pthread -lncurses -o ${EXEC} ${CXXFLAGS}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}

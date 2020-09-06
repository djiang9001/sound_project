#include <iostream>
#include <fstream>
#include <string>
#include <emscripten.h>
#include <emscripten/html5.h>
#include "fftw3.h"
#include "curses.h"

EM_BOOL play(double time, void* userData) {
  double x = EM_ASM_DOUBLE({
			if (sound) {
				return sound.seek();
			} else {
				return 0.0;
			}
		});
  //std::cout << std::to_string(x) << std::endl;
  double y = EM_ASM_DOUBLE({
			if (Howler && Howler.ctx) {
				if (Howler.ctx.outputLatency) {
					return Howler.ctx.outputLatency;
				} else if (Howler.ctx.baseLatency) {
					return Howler.ctx.baseLatency;
				}
			}
			return 0.0;
		});
  //std::cout << std::to_string(y) << std::endl;
  //puts("one iteration");
  // Return true to keep the loop running.
  return EM_TRUE;
}

int main(int argc, char* argv[]) {
	std::cout << "Hello World" << std::endl;
	freopen("logError.txt", "w", stderr);
	std::cerr << "error" << std::endl;
	int N = 999;
	fftw_complex in[N], out[N];
	fftw_plan p;
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	initscr();
	cbreak();
	noecho();
	nodelay(stdscr, TRUE);
	keypad(stdscr, TRUE);
	curs_set(1);
	move(10, 10);
	addstr("test");
	refresh();
	getch();
	int wav_loaded_c = 0;
	while (!wav_loaded_c) {
		wav_loaded_c = EM_ASM_INT({
			return wav_loaded;
		});
		getch();
		emscripten_sleep(100);
	}
	EM_ASM(
		sound.play();
	);
	std::ifstream f("your_wav_file");
	char chunkID[4];
	for (int i = 0; i < 50; ++i) {
		f.read((char*)&chunkID, 4);
		std::cout.write(chunkID, 4) << std::endl;
	}
	emscripten_request_animation_frame_loop(play, 0);
	return 0;
}



#include <iostream>
#include <fstream>
#include <string>
#include <emscripten.h>
#include <emscripten/html5.h>

EM_BOOL play(double time, void* userData) {
  double x = EM_ASM_DOUBLE({
			if (sound) {
				return sound.seek();
			} else {
				return 0.0;
			}
		});
  std::cout << std::to_string(x) << std::endl;
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
  std::cout << std::to_string(y) << std::endl;
  std::ifstream f("your_wav_file");
  if (f.is_open()) {
	  std::cout << "open" << std::endl;
  } else {
	  std::cout << "not open" << std::endl;
  }
  puts("one iteration");
  // Return true to keep the loop running.
  return EM_TRUE;
}

int main() {
	std::cout << "Hello World" << std::endl;
	emscripten_request_animation_frame_loop(play, 0);
	return 0;
}



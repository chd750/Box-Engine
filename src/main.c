#include <stdio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_events.h>
#include <include/rendering.h>
#include <include/audio.h>

void fps(float rate, Uint64 *last_f){
	
	while(SDL_GetTicks() - 1000/rate < *last_f){}
	*last_f = SDL_GetTicks();

}

int main(){

	Bx_Mixer *m = Bx_InitializeMixer(NULL, 1);
	Bx_InitializeChannel(m, 0, 0, 0);
	int exit;
	SDL_Event e;

	while(exit){

		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					exit = 0;
					break;
			}
		}

	}

	SDL_Quit();
	return 0;
}

#include <stdio.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

void fps(float rate, Uint64 *last_f){
	
	while(SDL_GetTicks() - 1000/rate < *last_f){}
	*last_f = SDL_GetTicks();

}

int main(){
	
	if(!SDL_Init(SDL_INIT_VIDEO)){
		printf("SDL_Error: Could not initialize SDL\n%s", SDL_GetError());
	}

	// Alright so now I have to remember how to open a window
	SDL_Window *window = SDL_CreateWindow("New window :)", 600, 400, 0);
	SDL_Surface *ws = SDL_GetWindowSurface(window);
	if(window == NULL){
		printf("SDL_Error: Could not create window\n%s", SDL_GetError());
	}

	Uint64 lt = SDL_GetTicks(); 
	Uint64 st = lt;
	Uint64 fcount = 0;
	int exit = 1;
	SDL_Event e;

	while(exit){

		fps(60.f, &lt);
		fcount++;

		while(SDL_PollEvent(&e)){
			switch(e.type){
				case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
					exit = 0;
					break;
			}
		}

		SDL_ClearSurface(ws, 0.f, 0.f, 0.f, 0.f);
		if((fcount/30)%2){
			SDL_ClearSurface(ws, 1.f, 0.f, 0.f, 1.f);
		}	

		SDL_UpdateWindowSurface(window);
	}

	SDL_Quit();

	printf("End game.\n");
	return 0;
}

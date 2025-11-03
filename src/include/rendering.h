#include <stdlib.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

#define Bx_QUEUESIZE_DEFAULT 256

typedef enum{

	Bx_SPRITE = 0,
	Bx_RECT = 1

}Bx_RenderObjectType;


typedef struct{
	Bx_RenderObjectType type;
	SDL_Texture *target;
	SDL_FRect *region;
	SDL_FRect *pos;
	SDL_Texture *sprite_tex;
}Bx_SpriteObject;

typedef struct{
	Bx_RenderObjectType type;
	SDL_Texture *target;
	SDL_FRect *pos;
	SDL_Color *color;
	Uint32 is_outline;
}Bx_RectObject;

typedef struct{
	Bx_RenderObjectType len;
	SDL_Texture *target;
	SDL_FPoint *points;
	SDL_Color *color;
	void* buf;
}Bx_LineObject;

typedef union{
	Bx_SpriteObject sprite;
	Bx_RectObject rect;
	Bx_LineObject line;
	struct{
		Bx_RenderObjectType type;
		SDL_Texture *target;
		void* data1;
		void* data2;
		void* data3;
	};
}Bx_RenderObject;

typedef struct{

	int window_size[2];
	size_t queue_size;
	char *window_name;

}Bx_RenderInfo;

typedef struct{
	
	SDL_Window *w;
	SDL_Renderer *rend;
	Bx_RenderObject **pipeline;
	size_t out;
	size_t in;
	size_t size;

}Bx_Renderer;

int Bx_ReallocRenderer(size_t amt, Bx_Renderer *rend, int copy){
	
	if(amt == 0){
		free(rend->pipeline);
		rend->pipeline = NULL;
		rend->size = 0;
		return 0;
	}

	Bx_RenderObject **q = malloc(sizeof(Bx_RenderObject) * amt);
	
	if(q == NULL){
		printf("Memory_Error: could not initialize queue of (%d) elements.\n");
		printf("No modifications have been made; call Bx_ReallocRender function with "); 
		printf("smaller parameter if more memory is needed.\n");
		return -1;
	}

	size_t lb = rend->size < amt ? rend->size : amt;	

	if(copy) memcpy(q, rend->pipeline, sizeof(Bx_RenderObject) * lb);
	else{
		rend->in = rend->out = 0;
	}

	rend->size = amt;
	free(rend->pipeline);
	rend->pipeline = q;

	return 0;
}

Bx_Renderer* Bx_InitRenderer(Bx_RenderInfo *inf){

	if(!SDL_Init(SDL_INIT_VIDEO)){
		fprintf(stderr, "SDL_Error: Could not initialize SDL.\n%s", SDL_GetError());
		return NULL;
	}

	SDL_Window *wd = SDL_CreateWindow(inf->window_name, inf->window_size[0], inf->window_size[1], 0);
	if(wd == NULL){
		fprintf(stderr, "SDL_Error: Could not create window.\n%s", SDL_GetError());
		return NULL;
	}

	SDL_Renderer *rend = SDL_CreateRenderer(wd, NULL);
	if(rend == NULL){
		fprintf(stderr, "SDL_Error: Could not get rendering context\n%s", SDL_GetError());
		SDL_DestroyWindow(wd);
		return NULL;
	}

	Bx_Renderer *res = (Bx_Renderer*) malloc(sizeof(Bx_Renderer));
	if(rend == NULL){
		fprintf(stderr, "Memory_Error: Could not create renderer object\n");
		SDL_DestroyRenderer(rend);
		SDL_DestroyWindow(wd);
		return NULL;
	}
	
	res->pipeline = NULL;
	Bx_ReallocRenderer(inf->queue_size, res, 0);
	res->w = wd;
	res->rend = rend;

	return res;
}

int Bx_PushToRender(Bx_RenderObject* this, Bx_Renderer* rend){
	
	if(rend->size == 0){
		if(Bx_ReallocRenderer(1, rend, 0) == -1){
			return -1;
		}
	}

	if(rend->in == rend->size){
		if(Bx_ReallocRenderer(rend->size * 2, rend, 1) == -1){
			return -1;
		}
	}
	
	rend->pipeline[rend->in] = this;
	rend->in++;
	return 0;
}

void Bx_RenderObj(Bx_RenderObject* this, Bx_Renderer* rend){

	SDL_SetRenderTarget(rend->rend, this->target);

	if(this->type == Bx_SPRITE) SDL_RenderTexture(rend->rend, this->sprite.sprite_tex, this->sprite.region, this->sprite.pos);
	else{
		SDL_SetRenderDrawColor(rend->rend, this->rect.color->r, this->rect.color->g, this->rect.color->b, this->rect.color->a);
		
		if(this->type == Bx_RECT){
			
			if(this->rect.is_outline) SDL_RenderRect(rend->rend, this->rect.pos);
			else SDL_RenderFillRect(rend->rend, this->rect.pos);
		}
		else SDL_RenderLines(rend->rend, this->line.points, this->line.len);	
	}
}


void Bx_Render(Bx_Renderer* rend){

	SDL_SetRenderTarget(rend->rend, NULL);
	SDL_SetRenderDrawColor(rend->rend, 0, 0, 0, 255);
	SDL_RenderFillRect(rend->rend, NULL);

	while(rend->out < rend->in){
		
		Bx_RenderObj(rend->pipeline[rend->out], rend);
		rend->out++;
	}

	rend->out = rend->in = 0;
	SDL_RenderPresent(rend->rend);

}

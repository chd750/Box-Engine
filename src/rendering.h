#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_rect.h>

typedef Bx_SpriteObject struct{
	Uint32 type = 0;
	SDL_Texture *target;
	SDL_FRect *region;
	SDL_FRect *pos;
	SDL_Texture *sprite_tex;
}so;

typedef Bx_RectObject struct{
	Uint32 type = 1;
	SDL_Texture *target;
	SDL_FRect *pos;
	SDL_Color *color;
}ro;

typedef Bx_LineObject struct{
	Uint32 len;
	SDL_Texture *target;
	SDL_Fpoint *points;
	SDL_Color *color;
}lo;

typedef Bx_RenderObject union{
	Bx_SpriteObject sprite;
	Bx_RectObject rect;
	Bx_LineObject line;
	struct{
		Uint32 type;
		SDL_Texture *target;
		void* data1;
		void* data2;
		void* data3;
	};
}ruo;

typedef Bx_Renderer struct{
	
	SDL_Renderer *rend;
	Bx_Object **pipeline;
	Uint32 out;
	Uint32 in;
	Uint32 size;

}

enum{
	Bx_SPRITE = 0,
	Bx_RECT
}

void render(Bx_RenderObject* this, Bx_Renderer* rend){
	
	SDL_SetRenderTarget(this->target);

	if(this->type == Bx_SPRITE) SDL_RenderTexture(rend->rend, this->sprite.sprite_tex, this->sprite.region, this->sprite.pos);
	else{
		SDL_RenderSetDrawColor(this->data2.r, this->data2.g, this->data2.b, this->data2.a);

		if(this->type == Bx_RECT) SDL_RenderRect(rend->rend, this->rect.pos);
		else SDL_RenderLines(rend->rend, this->line.points);	
	}
}

int realloc_renderer(Bx_Renderer* this, Uint32 amt){
	
	return 0;

}

int push_render(Bx_RenderObject* this, Bx_Renderer* rend){
	
	if(rend->in == rend->size){
		if(!realloc_renderer(rend, rend->size*2)){
			fprintf(stderr, "MemError: Could not expand queue to add new element\n");
			return -1;
		}
	}
	
	rend->pipeline[rend->in] = this;
	rend->in++;
	return 0;
}

void render(Bx_Renderer* rend){

	while(rend->out < rend->in){
		
		render(rend->pipeline[rend->out], rend);
		rend->out++;
	}

	rend->out = 0;
	rend->in = 0;

}

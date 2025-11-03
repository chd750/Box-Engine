# Rendering

This is the rendering component of the BoxEngine kernel. All functions and type declarations relating to rendering 
objects are here. 

## How to use

To create a window, make a ``Bx_RenderInfo`` object containing all attributes and informations of your renderer.
Note that once this object is created and passed to ``Bx_CreateRenderer``, there is no way to change the attributes
(as of BoxEngine version 0). You might have to destroy and recreate your renderer if any changes are requested.
Functions to change attributes will be implemented in future releases of BxEngine v0.

Once this function is called, you will be given a pointer (which is NULL if the renderer couldn't be initialized),
and you must now pass this pointer to all rendering functions. To render any object, you must initialize a 
``Bx_RenderObject`` and specify the attributes as seen in the type declaration below.

To render an object, you push it to queue in the order you want it to be rendered (queue follows first-in first-out 
or FIFO principle, so elements pushed first are rendered first), and once you have queued all elements you wish to
render, you call ``Bx_Render`` and all objects will be displayed on your screen.

## Reference

### Bx_Renderer
    
The main renderer object for BoxEngine. Defines an instance of one window, and designed for internal use only. 

```
typedef struct{
	
	SDL_Window *w; // SDL Window instance
	SDL_Renderer *rend; // SDL renderer instance
	Bx_RenderObject **pipeline; // Pointer to queue elements
	size_t out; // amount of elements processed
	size_t in; // amount of elements in queue
	size_t size; // size of rendering queue

}Bx_Renderer;
```

### Bx_RenderInfo

Renderer initialization info. For now only has three parameters:

```
typedef struct{

	int window_size[2]; // dimensions of window
	size_t queue_size; // requested size of queue. 
	char *window_name; // name of window

}Bx_RenderInfo;
```

### Bx_RenderObject

General superclass of objects handled by renderer when Bx_Render is called.

```
typedef union{
	Bx_SpriteObject sprite; // signature of sprite data
	Bx_RectObject rect; // signature of rectangle data
	Bx_LineObject line; // signature of sequence of lines data
	
    struct{ // general signature
		Bx_RenderObjectType type; // type of object (Bx_SPRITE for sprite, Bx_RECT for rect, 2 or more for lines)
		SDL_Texture *target; // texture to be affected by changes. Set NULL for window
		void* data1; // filler categories
		void* data2;
		void* data3;
	};
}Bx_RenderObject;
```

#### Bx_SpriteObject

Subclass of Bx_RenderObject that has the specific attributes of a sprite.

```
typedef struct{
	Bx_RenderObjectType type; // this is Bx_SPRITE
	SDL_Texture *target; // texture to be affected by changes (NULL if window)
	SDL_FRect *region; // region of the sprite that will be drawn (NULL if whole sprite, useful for spritesheets)
	SDL_FRect *pos; // position where the sprite will be drawn (NULL if whole rendering target)
	SDL_Texture *sprite_tex; // Source texture of the sprite
}Bx_SpriteObject;

```

#### Bx_RectObject

Subclass of Bx_RenderObject that has the specific attributes of a rectangle.

```
typedef struct{
	Bx_RenderObjectType type; // this is Bx_RECT
	SDL_Texture *target; // texture to be affected by changes (NULL if window) 
	SDL_FRect *pos; // Dimensions of rectangle to be drawn (NULL if whole target)
	SDL_Color *color; // color of the rectangle to be drawn
	Uint32 is_outline; // 0 or false if whole rectangle to be filled, 1 if only the frame of the rect is desired.
}Bx_RectObject;
```

#### Bx_LineObject

Subclass of Bx_RenderObject that has the specific attributes of a sequence of lines.

```
typedef struct{
	Bx_RenderObjectType len; // Amount of points to be connected by lines 
	SDL_Texture *target; // texture to be affected by changes (NULL if window)
	SDL_FPoint *points; // sequence of points that will be connected by lines
	SDL_Color *color; // color of the lines connecting the provided points
	void* buf; // buffer value (not really supposed to contain anything)
}Bx_LineObject;
```

### Bx_CreateRenderer

Function that initializes the renderer instance for a given window. Returns NULL if renderer could not be created.
Check console for errors.

``Bx_Renderer* Bx_CreateRenderer(Bx_RenderInfo *info)``

| parameter | type | purpose |
| --- | ---- | ------- |
| info | Bx_RenderInfo* | Information passed to the kernel in order to create a renderer object. |

### Bx_DestroyRenderer

Function that closes and de-initializes the renderer.

``void Bx_DestroyRenderer(Bx_Renderer* this)``

| parameter | type | purpose |
| --- | ---- | ------- |
| this | Bx_Renderer* | Renderer to be cleared and closed by kernel. |


### Bx_PushToRender

Function that pushes an object into a renderer's rendering queue.

``void Bx_PushToRender(Bx_RenderObject* this, Bx_Renderer* rend)``

| parameter | type | purpose |
| --- | ---- | ------- |
| this | Bx_RenderObject* | Object to be rendered by renderer rend. |
| rend | Bx_Renderer* | Renderer that renders the object. |

### Bx_Render

Function that renders all objects in the passed renderer's rendering queue.

``void Bx_Render(Bx_Renderer* this)``

| parameter | type | purpose |
| --- | ---- | ------- |
| this | Bx_Renderer* | The renderer that renders the objects queued to it. |


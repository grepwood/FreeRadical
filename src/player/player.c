#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <WAAVE.h>

/* input file */
char* filename;


/* screen proprieties */
SDL_DisplayMode desktopMode;
int screenWidth;
int screenHeight;
int bpp;


/* window surface */
int flags;
SDL_Window* screen;
SDL_Renderer* screenRenderer;
int winWidth;
int winHeight;
int fullscreenFlag = 0;
SDL_Rect destRect;


/* playing stream */
WVStream* stream = NULL;
WVStreamingObject* streamObj = NULL;


/* close sequence */
void closePlayer(void)
{
  /* close the opened stream. not mandatory  */
  if(stream)
    WV_closeStream(stream);

  /* close the streaming object */
  if(streamObj)
    WV_freeStreamRendererObj(streamObj);
  
  /* close the waave engine */
  WV_waaveClose();

  /* close sdl */
  SDL_Quit();
  exit(0);
}


/* check key */
void keyPressed( SDL_Keysym* keysym  ) 
{
 
  switch ( keysym->sym ) {

  case SDLK_ESCAPE:
    closePlayer();  //close
    break;
    
  case SDLK_SPACE:
    WV_playStream(stream); //play
    break;

  case SDLK_BACKSPACE:
    WV_pauseStream(stream);  //pause
    break;

  case SDLK_KP_PLUS:
    WV_shiftDBVolume(stream, +1); //encrease volume
    break;

  case SDLK_KP_MINUS:
    WV_shiftDBVolume(stream, -1); //decrease volume
    break;

  case SDLK_f:
    if(!fullscreenFlag){   //toggle fullscreen
      fullscreenFlag = 1;
      SDL_ShowCursor(SDL_DISABLE);
      SDL_SetWindowFullscreen(screen, SDL_TRUE);
      SDL_RenderSetViewport(screenRenderer, NULL);
      if(streamObj)
	WV_resetStreamRendererOutput(streamObj, screenRenderer, NULL); 
    }
    else{
      fullscreenFlag = 0;
      SDL_ShowCursor(SDL_ENABLE);
      SDL_SetWindowFullscreen(screen, SDL_FALSE);
      SDL_RenderSetViewport(screenRenderer, NULL);
      if(streamObj)
	WV_resetStreamRendererOutput(streamObj, screenRenderer, NULL);
      
    }
    break;

    
    /********/
    /* seek */
    /********/
    uint32_t seekShift; //will be converted in milliseconds
    int seekDirection;

  case SDLK_RIGHT:
    seekShift = 10;
    seekDirection = WV_SEEK_FORWARD; 
    goto do_seek;

  case SDLK_LEFT:
    seekShift = 10;
    seekDirection = WV_SEEK_BACKWARD;
    goto do_seek;

  case SDLK_UP:
    seekShift = 60;
    seekDirection = WV_SEEK_FORWARD; 
    goto do_seek;

  case SDLK_DOWN:
    seekShift = 60;
    seekDirection = WV_SEEK_BACKWARD; 
    goto do_seek;

  do_seek:
    seekShift *= 1000;
    WV_rseekStream(stream, seekShift, seekDirection);
    break;
     
  default:
    break;
  }
}


/* sdl events */
void processEvents(void)
{
  SDL_Event event;
  while( SDL_WaitEvent( &event ) ) {
    switch( event.type ) {
    
    case SDL_KEYDOWN:
      keyPressed( &event.key.keysym );
      break;
      
    case SDL_WINDOWEVENT:
      if(event.window.event == SDL_WINDOWEVENT_RESIZED){
	if(!fullscreenFlag){
	  winWidth = event.window.data1;
	  winHeight = event.window.data2;
	  SDL_RenderSetViewport(screenRenderer, NULL); 
	  SDL_RenderClear(screenRenderer);
	  if(streamObj)
	    WV_resetStreamRendererOutput(streamObj, screenRenderer, NULL); // update the object
	}  
      }
      break;
      
    case SDL_QUIT:
      closePlayer();
      break;
    }

    /* there events are dynamics */
    if(event.type == WV_REFRESH_EVENT)
      WV_refreshVideoFrame(&event);
    else if(event.type == WV_EOF_EVENT)
      closePlayer();
    
  }
}



int main(int argc, char** argv)
{
  if(argc != 2){
    printf("usage : %s vidfilename\n", argv[0]);
    return 0;
  }

  /* parse arg */
  filename = argv[1];
  

  /* the window will be resized */
  /* after reading video size */
  winWidth = 640;
  winHeight = 480;

  /************/
  /* init sdl */
  /************/
  if( SDL_Init( SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_TIMER ) < 0 ) {
    fprintf( stderr, "Video initialization failed: %s\n", SDL_GetError( ) );
    SDL_Quit( );
    exit(0);
  }

  /* set windows attributes */
  SDL_GetDesktopDisplayMode(0, &desktopMode);
  SDL_SetWindowDisplayMode(screen, &desktopMode); //seems not working ?

  flags = SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL;

  /* create display */
  screen = SDL_CreateWindow("Waave player",\
			    SDL_WINDOWPOS_UNDEFINED,\
			    SDL_WINDOWPOS_UNDEFINED,\
			    winWidth,\
			    winHeight,\
			    flags);
  
  if( !screen ) {
    fprintf( stderr, "Video mode set failed: %s\n", SDL_GetError( ) );
    SDL_Quit( );
    exit(0);
  }
  
  /**************/
  /* init waave */
  /**************/

  /* start waave and load file */
  WV_waaveInit(WAAVE_INIT_AUDIO|WAAVE_INIT_VIDEO);
  stream = WV_getStream(filename);
  
  /* build a streaming object for the screen */
  int streamType = WV_getStreamType(stream);
  if(streamType == WV_STREAM_TYPE_VIDEO || streamType == WV_STREAM_TYPE_AUDIOVIDEO){
    screenRenderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(screenRenderer);
    destRect.x = SDL_WINDOWPOS_UNDEFINED;
    destRect.y = SDL_WINDOWPOS_UNDEFINED;
    destRect.w = winWidth;
    destRect.h = winHeight;
    streamObj = WV_getStreamRendererObj(screenRenderer,&destRect,50);
    WV_setStreamingMethod(stream, streamObj);
  }

  /* load the stream */
  WV_loadStream(stream);
  
  /* reset window size for video */
  if(streamObj){
    winWidth = streamObj->srcWidth;
    winHeight = streamObj->srcHeight;
    SDL_SetWindowSize(screen, winWidth, winHeight);
    WV_resetStreamRendererOutput(streamObj, screenRenderer, NULL); //!! update the object !!
  }

  /* at start the stream is paused */
  /* so we launch playback */
  WV_playStream(stream);
  
  /*****************/
  /* display loop  */
  /*****************/
  while( 1 ) {
    processEvents( );
  }
  return(0);
}

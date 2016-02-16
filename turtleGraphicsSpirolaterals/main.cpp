


#include <iostream>
#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

bool leftMouseButtonDown = false;
bool quit = false;
SDL_Event event;


SDL_Window * window;
int drawingWindowId;
int ToolsWindowId;

SDL_Renderer * renderer;
//SDL_Renderer * renderer2;
SDL_Texture * texture;

Uint32 * pixels = new Uint32[640 * 480];
//void * pixels2;

Uint32 * backbufferPixels=NULL;
int sizeofbackbufferPixels;

Uint8 tmpColor;


//ToolBox Booleans
typedef enum {FALSE,TRUE} boolean;
boolean drawRectangle =FALSE;
boolean drawPointLine =FALSE;
boolean drawPointCircle =FALSE;
boolean drawPointPencil =FALSE;
boolean drawPointBrush =FALSE;
boolean drawPointBucketFill =FALSE;
boolean drawPointRedColor =FALSE;
boolean drawPointBlackColor =FALSE;
boolean drawPointEraser =FALSE;

boolean NOWDRAWING =FALSE;

boolean executedOnce =FALSE;

SDL_Color colorpicked;

///////////////////////////////////////////////////////////////////
/// \brief loadedSurface
const char * path="toolbox.png";
SDL_Surface* loadedSurface;
SDL_Surface * surf;

SDL_Surface* formattedSurface;
SDL_Surface* formattedSurface2;

SDL_Texture * texture2;


typedef struct Point
{
    Point(){;}
    Point(int _x,int _y){x=_x;y=_y;}
    ~Point(){}
    Point(const Point &_p){x=_p.x;y=_p.y;}
    void operator=(const Point &_p){x=_p.x;y=_p.y;}

    int x,y;
}p;


Point lineStart;
Point lineEnd;

void drawBresenhamLine(int x,int y,int x2,int y2)
{
     if (x<0) {x=0;};
     if (x>640) {x=639;}
     if (y<0) {y=0;}
     if (y>480) {y=479;}

     if (x2<0) {x2=0;};
     if (x2>640) {x2=639;}
     if (y2<0) {y2=0;}
     if (y2>480) {y2=479;}

      Point * lineArray=NULL;
      lineArray=(Point*)malloc(1*sizeof(Point));


      //http://tech-algorithm.com/articles/drawing-line-using-bresenham-algorithm/
      int w = x2 - x ;
      int h = y2 - y ;
      int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0 ;
      if (w<0) dx1 = -1 ; else if (w>0) dx1 = 1 ;
      if (h<0) dy1 = -1 ; else if (h>0) dy1 = 1 ;
      if (w<0) dx2 = -1 ; else if (w>0) dx2 = 1 ;

      int longest = abs(w) ;
      int shortest = abs(h) ;

      if (!(longest>shortest))
      {
          longest = abs(h) ;
          shortest = abs(w) ;
          if (h<0) dy2 = -1 ;
          else if (h>0) dy2 = 1 ;
          dx2 = 0 ;
      }
      int numerator = longest >> 1 ;
      for (int i=0;i<=longest;i++)
      {
          SDL_RenderDrawPoint(renderer,x,y);

          lineArray=(Point*)realloc(lineArray,(i+1)*sizeof(Point));
          lineArray[i]=Point(x,y);


    //          std::cout<<"lineArray="<<lineArray[i].x<<","<<lineArray[i].y<<std::endl;

          numerator += shortest ;
          if (!(numerator<longest))
          {
              numerator -= longest ;
              x += dx1 ;
              y += dy1 ;
          } else {
              x += dx2 ;
              y += dy2 ;
          }
      }

    //clear our buffer constantly so as to end up with only 1 line

    //Now create the backbuffercopy of the original pixel array (our board)
    memcpy(backbufferPixels, pixels,  640 * 480 * sizeof(Uint32));

    for(int i=0;i<=longest;i++)
    {

        Uint32 tmpPixel=SDL_MapRGB(formattedSurface2->format, colorpicked.r,colorpicked.g,colorpicked.b);
        backbufferPixels[lineArray[i].y * 640 + lineArray[i].x]=tmpPixel;
    }

    free(lineArray);
}


int main(int argc, char ** argv)
{

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2 Turtle Graphics / Spirolateral Drawing",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    renderer = SDL_CreateRenderer(window, -1, 0);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);

    surf = SDL_CreateRGBSurface(0,640,480,32,0,0,0,0);
    formattedSurface2 = SDL_ConvertSurfaceFormat( surf, SDL_PIXELFORMAT_ARGB8888, NULL );
    if( formattedSurface2 == NULL )
    {
        printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
    }
    else
    {
            int pitch;
            void *pxs=(void*)pixels;
            //Lock texture for manipulation
            SDL_LockTexture( texture, NULL, &pxs, &pitch );

            //Copy loaded/formatted surface pixels
            memcpy( pixels, formattedSurface2->pixels, 640 *480 );

            //Unlock texture to update
            SDL_UnlockTexture( texture );

    }
    //Get rid of old loaded surface
    SDL_FreeSurface( surf );


    memset(pixels, 255, 640 * 480 * sizeof(Uint32));


    while (!quit)
    {
        SDL_UpdateTexture(texture, NULL, pixels, 640 * sizeof(Uint32));
        SDL_PollEvent(&event);

        SDL_Rect DestR;
        DestR.x = 100;
        DestR.y = 100;
        DestR.w = 100;
        DestR.h = 100;

        switch (event.type)
        {
        case SDL_KEYDOWN:

            if (event.key.keysym.sym==SDLK_SPACE)
            {
                memset(pixels,255,640*480*sizeof(Uint32));
                executedOnce=FALSE;
            }
            break;


        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                leftMouseButtonDown = false;

                //Decision made- Now mouse is up, so copy to original 'pixel' data the buffered data.
                std::cout<<"drawPointLine="<<drawPointLine<<std::endl;

                 if (drawPointLine && backbufferPixels!=NULL)
                {
                    memcpy(pixels,backbufferPixels,640*480*sizeof(Uint32));

                    delete backbufferPixels;
                    backbufferPixels=NULL;
                    drawPointLine=FALSE;


                }

                if (drawPointCircle && backbufferPixels!=NULL)
                {
                    memcpy(pixels,backbufferPixels,640*480*sizeof(Uint32));

                    delete backbufferPixels;
                    backbufferPixels=NULL;
                }

                NOWDRAWING=FALSE;

            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    leftMouseButtonDown = true;
                    drawPointLine=TRUE;
                }

            }
            break;

        case SDL_MOUSEMOTION:
            if (leftMouseButtonDown)
            {



            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
        }


int mouseX = event.motion.x;
int mouseY = event.motion.y;

if (drawPointLine==TRUE && !executedOnce)
{
        //avoid multiple executions
        executedOnce=TRUE;

        //Line Drawing
        //Make sure we draw the texture and THEN..we draw on the top of it for the backbuffer lines,points (for circle,rectangle)
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        Point currentmousePos(mouseX,mouseY);
        //start line point initialized just once
        if (!NOWDRAWING)
        {
            NOWDRAWING=TRUE;
            lineStart=currentmousePos;
        }

        std::cout<<"lineStart="<<lineStart.x<<","<<lineStart.y<<std::endl;

        //end line point
        lineEnd=currentmousePos;

        //Allocate some memory for the backbufferPixels
        if (backbufferPixels==NULL)
        {
            backbufferPixels=(Uint32*)malloc(640*480* sizeof(Uint32));
        }
        else
        {
            memset(backbufferPixels,0,640*480* sizeof(Uint32));
        }

        //Now create the backbuffercopy of the original pixel array (our main drawing board)
        memcpy(backbufferPixels, pixels,  640 * 480 * sizeof(Uint32));

        //Draw Simple Spirolateral (angle 90. initial segment 1, order 2, repetitions/recursion 2)

        int initialangle=0;
        int initialSegment=20;
        int order=6;
        int repetitionRecursion=2;

        printf ("Enter angle (ex. 60):  ");
        scanf("%d",&initialangle);

        printf ("Enter initialSegment length (ex. 5): ");
        scanf("%d",&initialSegment);

        printf ("Enter fractal order (ex. 7): ");
        scanf("%d",&order);

        printf ("Enter repetitionRecursion (ex. 80): ");
        scanf("%d",&repetitionRecursion);


        Point end=lineStart;

        int segment=0;
        int angle=initialangle;
        for (int i=0; i<repetitionRecursion;i++)
        {
            segment=initialSegment;
//            angle=initialangle;

            //draw lines sequentially based on the order of the spirolateral (in. example 2= times= 2 segments need to be drawn for the spirolateral to be completed)
            for (int j=0; j<order;j++)
            {
                SDL_RenderPresent(renderer);

                end.x=lineStart.x+segment*sin(angle*M_PI/180.0f);
                end.y=lineStart.y+segment*cos(angle*M_PI/180.0f);

                drawBresenhamLine(lineStart.x,lineStart.y, end.x,end.y);
                memcpy(pixels,backbufferPixels,640*480*sizeof(Uint32));

                lineStart=end;
                segment+=initialSegment;
                angle-=initialangle;
                angle=fmod(angle,360.0);

                printf ("Draw with angle %d\n:", angle);

            }
            SDL_RenderPresent(renderer);
        }

        drawPointLine=FALSE;
}





        // Creat a rect at pos ( 50, 50 ) that's 50 pixels wide and 50 pixels high.
        if(!leftMouseButtonDown)
        {
            SDL_RenderCopy(renderer, texture, NULL, NULL);
        }

        //update the rendered image
        SDL_RenderPresent(renderer);

    }

    SDL_FreeSurface( formattedSurface2 );

    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



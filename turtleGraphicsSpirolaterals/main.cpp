


#include <iostream>
#include <SDL.h>
#include <math.h>
#include <stdio.h>
#include <string.h> // pulls in declaration for strlen.

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


// Main Mid-Point Circle Algorithm Helper Functions
void drawpixel(int x, int y, Uint32 * all_pixels, Uint32 ** tmpPixels/*, SDL_Surface * gPNGSurface, SDL_Window * gWindow, SDL_Surface * gScreenSurface */)
{


    if(x>0 && x<640 && y>0 && y<480) //keep safe image boundaries
    {
        //next pixel position
        int index = y* 640   + x;

        //Get specific pixel
        Uint32 pixel = (*tmpPixels)[index];

        //Change the current pixel to the values specified by newRed,newGreen,newBlue

        SDL_Color c;c.r=colorpicked.r;c.g=colorpicked.g;c.b=colorpicked.b;
        Uint32 tmpPixel=SDL_MapRGB(formattedSurface2->format,c.r,c.g,c.b );

        //Update the buffer surface's pixels
        (*tmpPixels)[y * 640 + x] = tmpPixel;

        SDL_SetRenderDrawColor( renderer, colorpicked.a, colorpicked.r, colorpicked.g, colorpicked.b);
        SDL_RenderDrawPoint(renderer,x,y);

    }
}


void callDrawPixel(int xc,int yc,int x,int y, Uint32 * all_pixels, Uint32 ** tmpPixels)
{

   // Draw 45 arcs from bottom counterclockwise
   drawpixel(xc+x, yc+y, all_pixels, tmpPixels);
   drawpixel(xc+x, yc-y, all_pixels, tmpPixels);
   drawpixel(xc+y, yc+x, all_pixels, tmpPixels);
   drawpixel(xc+y, yc-x, all_pixels, tmpPixels);

   drawpixel(xc-x, yc-y, all_pixels, tmpPixels);
   drawpixel(xc-x, yc+y, all_pixels, tmpPixels);
   drawpixel(xc-y, yc-x, all_pixels, tmpPixels);
   drawpixel(xc-y, yc+x, all_pixels, tmpPixels);
}




void drawMultipleInnerCircles(int r, int x, int y, int yc, int xc, float Pk, Uint32 * all_pixels, Uint32 ** tmpPixels, boolean &changeLocation, boolean &enterWhile/*, SDL_Surface *gPNGSurface, SDL_Window *gWindow, SDL_Surface * gScreenSurface */)
{
//    while (r>=0 && enterWhile)
    {
        while(x<y && enterWhile)
        {
            if (Pk<0)
            {
                x=x+1;
                Pk=Pk+(2*x)+1;
            }
            else
            {
                x=x+1;
                y=y-1;
                Pk=Pk+(2*x)-(2*y)+1;
            }
            callDrawPixel(xc,yc,x,y, all_pixels, tmpPixels);

        }


        changeLocation=TRUE;
        x=1;
        y=--r;
        Pk=1-r;
    }


}


typedef struct Point
{
    Point(){;}
    Point(int _x,int _y){x=_x;y=_y;}
    ~Point(){}
    Point(const Point &_p){x=_p.x;y=_p.y;}
    void operator=(const Point &_p){x=_p.x;y=_p.y;}

    int x,y;
}p;


#include <stack>
typedef struct Element
{
    Element() {  ; }
    Element(int _x,int _y)
    {
        x=_x;
        y=_y;
    }
    ~Element(){}

 int x;
 int y;
}Element;



static int counter=0;

void floodfill (int x,int  y, Uint32* all_pixels, Uint8 targetR,Uint8 targetG,Uint8 targetB, Uint8 replacementR ,Uint8 replacementG,Uint8 replacementB)
{
    std::stack<Element> stack;


    Uint32 * tmpPixels=(Uint32*)malloc(640*480* sizeof(Uint32));
    memcpy(tmpPixels,all_pixels, 640 * 480 * sizeof(Uint32));


//    int pitch;
//    void *pxs=(void *) all_pixels;
//    SDL_LockTexture(texture,NULL,&pxs,&pitch);

    Element e(x,y);
    stack.push(e);

        while( (stack.size()) > 0 )
        {

            Element e = stack.top(); stack.pop();
            x= e.x;
            y= e.y;

//            std::cout<<"x="<<x<<", y="<<y<<std::endl;

            //make sure pixel position is within the surface's boundaries
            if  (x > 0 && x < 640 && y > 0 && y < 480)
            {

                //next pixel position
                int index = y* 640   + x;

                //Get specific pixel
                Uint32 pixel = tmpPixels[index];

                Uint8 curPixR, curPixG, curPixB;
                //Get current r,g,b

//                Uint32 *pixelformat;
//                int w,h;
//                SDL_QueryTexture(texture, pixelformat, NULL, &w, &h);


                SDL_GetRGB(pixel, formattedSurface2->format , &curPixR, &curPixG, &curPixB);


                //We only change the color picked up by the mouse at pixel location we clicked
                // If the node's color we visit doesn't match the color we target to change we simply continue
                if( (curPixR!=targetR || curPixG!=targetG || curPixB!=targetB) )
                {
                        continue;
                }
                if( (curPixR==replacementR && curPixG==replacementG && curPixB==replacementB) )
                {
                        break;
                }

                //if we have just hit the pixel that we intend to change the colour of
                {//if we have just hit the pixel that we intend to change the colour of
//                {
                    //Change the current pixel to the values specified by newRed,newGreen,newBlue
//                    pixel = (0xFF << 24) | (replacementR << 16) | (replacementG << 8) | replacementB;
                    //Update the surface's pixels
//                    all_pixels[y * 640 + x] = pixel;


                    SDL_Color c;c.r=replacementR;c.g=replacementG;c.b=replacementB;
                    Uint32 tmpPixel=SDL_MapRGB(formattedSurface2->format,c.r,c.g,c.b );

//                    for(int i =0;i<640;i++)
//                        for(int j =0;j<480;j++)
                            tmpPixels[y * 640 + x] = tmpPixel;


                    stack.push( Element(x + 1, y) );  // right
                    stack.push( Element(x - 1, y) );  // left
                    stack.push( Element(x, y + 1) );  // down
                    stack.push( Element(x, y - 1) );  // up

                }
            }
    }

        memcpy(all_pixels,tmpPixels,640*480* sizeof(Uint32));
        delete tmpPixels;
}


Point lineStart;
Point lineEnd;

void drawBresenhamLine(int x,int y,int x2,int y2)
{
     if(x<0) {x=0;};
     if(x>640) {x=639;}
     if(y<0) {y=0;}
     if(y>480) {y=479;}

     if(x2<0) {x2=0;};
     if(x2>640) {x2=639;}
     if(y2<0) {y2=0;}
     if(y2>480) {y2=479;}

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



////    Point lineArray[100]={Point(0,0)};

//      int lineArrayElements=0;
//      Point * lineArray=NULL;
//      lineArray=(Point*)malloc(1*sizeof(Point));
//      lineArrayElements++;
//      int dx=x1-x0;
//      int dy=y1-y0;

//      int D = 2*dy - dx;
//      lineArray[0]=Point(x0,y0);;
//      int y=y0;

////      if (D > 0)
////      {
////          y = y+1;
////          D = D - (2*dx);
////      }

//      for(int x=x0+1;x<=x1;x++)
//      {
//          lineArray=(Point*)realloc(lineArray,(x+1)*sizeof(Point));
//          lineArray[x]=Point(x,y);
//          D = D + (2*dy);
//          if (D > 0)
//          {
//            y = y+1;
//            D = D - (2*dx);
//          }
//          lineArrayElements++;
//      }


//      for(int i=0;i<lineArrayElements;i++)
//      {
//          Point tmpPlotingPoint=lineArray[i];

//          SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );

//          //back buffer draw points
//          SDL_RenderDrawPoint(renderer,tmpPlotingPoint.x,tmpPlotingPoint.y);
////          SDL_Color pointcolor;
////          pointcolor.r=0;pointcolor.g=255;pointcolor.b=0;
////          Uint32 tmpPixel=SDL_MapRGB(formattedSurface->format, pointcolor.r,pointcolor.g,pointcolor.b);
////          backbufferPixels[tmpPlotingPoint.y * 640 + tmpPlotingPoint.x]=tmpPixel;

//      }


//      free(lineArray);

}



int main(int argc, char ** argv)
{

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = SDL_CreateWindow("SDL2 Turtle Graphics / Spirolateral Drawing",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    int drawingWindowId=SDL_GetWindowID(window);
//    SDL_Window * window2 = SDL_CreateWindow("Tools",
//        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 100, 100, 0);


    renderer = SDL_CreateRenderer(window, -1, 0);
//    renderer2 = SDL_CreateRenderer(window2, -1, 0);
    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, 640, 480);



//    loadedSurface=IMG_Load(path);


//        if( loadedSurface == NULL )
//        {
//            printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
//        }
//        else
//        {
//            // Convert surface to display format

//            // formattedSurface = SDL_ConvertSurface( loadedSurface,  SDL_GetWindowSurface(gWindow)->format, NULL );
//            formattedSurface = SDL_ConvertSurfaceFormat( loadedSurface, SDL_PIXELFORMAT_ARGB8888, NULL );

//            if( formattedSurface == NULL )
//            {
//                printf( "Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError() );
//            }
//            else
//            {
//                //Create blank streamable texture
//                texture2 = SDL_CreateTexture( renderer2,SDL_GetWindowPixelFormat( window2 ), SDL_TEXTUREACCESS_STREAMING, 100, 100 );
//                if( texture2 == NULL )
//                {
//                    printf( "Unable to create blank texture! SDL Error: %s\n", SDL_GetError() );
//                }
//                else
//                {
//                    int pitch;
//                    //Lock texture for manipulation
//                    SDL_LockTexture( texture2, NULL, &pixels2, &pitch );

//                    //Copy loaded/formatted surface pixels
//                    memcpy( pixels2, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h );


//                    //Unlock texture to update
//                    SDL_UnlockTexture( texture2 );
////                    pixels2 = NULL;
//                }

//            }
//            //Get rid of old loaded surface
//            SDL_FreeSurface( loadedSurface );
//        }
    //////////////////////////////////////////////////////////////////


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
//        SDL_UpdateTexture(texture2, NULL, pixels2, 100 * sizeof(Uint32));
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


//                    else if (drawRectangle==TRUE)
//                    {

//                    }
//                    else if (drawPointCircle==TRUE)
//                    {

//                        //Line Drawing
//                        //Make sure we draw the texture and THEN..we draw on the top of it for the backbuffer lines,points (for circle,rectangle)
//                        SDL_RenderCopy(renderer, texture, NULL, NULL);

//                        Point currentmousePos(mouseX,mouseY);
//                        //start line point initialized just once
//                        if (!NOWDRAWING)
//                        {
//                            NOWDRAWING=TRUE;
//                            lineStart=currentmousePos;
//                        }
//                        //end line point
//                        lineEnd=currentmousePos;

//                        //Now we need to calculate r based on the lineStart,lineEnd
////                        if (!(lineEnd.x-lineStart.x==0 && lineEnd.y-lineStart.y==0))
//                        {
//                            //find length of r based on 2 points
//                            int r=sqrt( pow((lineEnd.x-lineStart.x),2) + pow((lineEnd.y-lineStart.y),2) );

//                            boolean enterWhile=TRUE;
//                            boolean changeLocation=FALSE;
////                            int r=50;
//                            //mid circle point found
//                            int xc=(lineEnd.x+lineStart.x)/2;//initial x,y values
//                            int yc=(lineEnd.y+lineStart.y)/2;//initial x,y values
//                            int x=1;
//                            int y=r;
//                            float Pk=1-r;

//                            if (backbufferPixels==NULL)
//                            {
//                                backbufferPixels=(Uint32*)malloc(640*480* sizeof(Uint32));
//                            }
//                            else
//                            {
//                                memset(backbufferPixels,0,640*480* sizeof(Uint32));
//                            }

//                            memcpy(backbufferPixels,pixels, 640 * 480 * sizeof(Uint32));

//                            drawMultipleInnerCircles(r, x,  y, yc ,  xc, Pk, pixels, &backbufferPixels, changeLocation, enterWhile/*, gPNGSurface, gWindow, gScreenSurface*/);



//                        }
//                    }
//                    else if (drawPointBucketFill==TRUE)
//                    {
//                        if (!NOWDRAWING)
//                        {
//                            NOWDRAWING=TRUE;

//                            if(mouseX!=-1 && mouseY!=-1)
//                            {
//                                Uint8 red,green,blue;
//                                Uint8 newRed,newGreen,newBlue;

//                                //Grab the color of the pixel on the spot and change the pixel's that match this color
//                                int index =(mouseY) * 640 + mouseX;
//                                Uint32 pixel = pixels[index];

//                                SDL_GetRGB(pixel, formattedSurface2->format, &red, &green, &blue);

//                                SDL_Color c;c.r=red;c.g=green;c.b=blue;


//                                //Flood-Fill with Yellow Colour
//                                newRed=colorpicked.r;
//                                newGreen=colorpicked.g;
//                                newBlue=colorpicked.b;
//                                SDL_Color nc;nc.r=newRed;nc.g=newGreen;nc.b=newBlue;


//    //                            floodFill4(mouseX,mouseY,pixels,red,green,blue, newRed,newGreen,newBlue);
//                                floodfill(mouseX,mouseY,pixels, c.r,c.g,c.b, nc.r,nc.g,nc.b);

//                            }
//                        }

//                    }
//                    else if (drawPointPencil==TRUE)
//                    {
//                        //Line Drawing
//                        //Make sure we draw the texture and THEN..we draw on the top of it for the backbuffer lines,points (for circle,rectangle)
//                        SDL_RenderCopy(renderer, texture, NULL, NULL);

//                        Uint32 pix=SDL_MapRGB(formattedSurface2->format,colorpicked.r,colorpicked.g,colorpicked.b);
//                        pixels[(mouseY) * 640 + mouseX] = pix;//(0xFF << 24) | (tmpColor << 16) | (tmpColor << 8) | tmpColor;;

//                    }
//                    else if (drawPointBrush==TRUE)
//                    {
//                        //Line Drawing
//                        //Make sure we draw the texture and THEN..we draw on the top of it for the backbuffer lines,points (for circle,rectangle)
//                        SDL_RenderCopy(renderer, texture, NULL, NULL);

//                        //Create a brush (draws on surrounding pixels as well depending on the brushsize)
//                        int brushsize=8;
//                        for(int i=0;i<brushsize/2;i++)
//                        {
//                            for(int j=0;j<brushsize/2;j++)
//                            {
//                                Uint32 pix=SDL_MapRGB(formattedSurface2->format,colorpicked.r,colorpicked.g,colorpicked.b);

//                                pixels[(mouseY+j) * 640 + mouseX+i] = pix;
//                            }
//                        }
//                    }
//                    else if (drawPointEraser==TRUE)
//                    {
//                        //Line Drawing
//                        //Make sure we draw the texture and THEN..we draw on the top of it for the backbuffer lines,points (for circle,rectangle)
//                        SDL_RenderCopy(renderer, texture, NULL, NULL);


//                        //Create a brush (draws on surrounding pixels as well depending on the brushsize)
//                        int brushsize=8;
//                        for(int i=0;i<brushsize/2;i++)
//                        {
//                            for(int j=0;j<brushsize/2;j++)
//                            {
//                                Uint32 pix=SDL_MapRGB(formattedSurface2->format,colorpicked.r,colorpicked.g,colorpicked.b);

//                                pixels[(mouseY+j) * 640 + mouseX+i] = pix;//(0xFF << 24) | (tmpColor << 16) | (tmpColor << 8) | tmpColor;;
//                            }
//                        }
//                    }





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
//                            memcpy(pixels,backbufferPixels,640*480*sizeof(Uint32));
        }



//        drawBresenhamLine(lineStart.x,lineStart.y, lineEnd.x,lineEnd.y);

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

            //draw lines sequentially based on the order of the spirolateral (in. example 2= times= 2 segments need to be drawn for the spirolateral to be completed)
            for (int j=0; j<order;j++)
            {
                SDL_RenderPresent(renderer);

                end.x=lineStart.x+segment*sin(angle*M_PI/180.0f);
                end.y=lineStart.y+segment*cos(angle*M_PI/180.0f);

                drawBresenhamLine(lineStart.x,lineStart.y, end.x,end.y);
                memcpy(pixels,backbufferPixels,640*480*sizeof(Uint32));

                lineStart=end;
                segment*=1.5;
                angle+=initialangle;
                angle=fmod(angle,360.0);

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


//        SDL_RenderClear(renderer2);
//        SDL_RenderCopy(renderer2, texture2, NULL, NULL);
//        SDL_RenderPresent(renderer2);
    }


//    SDL_FreeSurface( formattedSurface );
    SDL_FreeSurface( formattedSurface2 );

    delete[] pixels;
//    delete[] pixels2;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



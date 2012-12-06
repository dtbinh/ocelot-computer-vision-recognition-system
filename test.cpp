//OCVRS UI
//Written by Aaron Myers


//The headers
#include "SDL.h"
#include "SDL_image.h"
#include <string>
#include <iostream>
#include <sstream>
#include <Windows.h>
using namespace std;
//Screen attributes
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

//The button states in the sprite sheet
const int CLIP_MOUSEOVER = 0;
const int CLIP_MOUSEOUT = 1;
const int CLIP_MOUSEDOWN = 2;
const int CLIP_MOUSEUP = 3;
const int CLIP_RANDMOUSEUP = 4;
const int CLIP_RANDMOUSEDOWN =5 ;
const int CLIP_TERMINATEUP = 6;
const int CLIP_TERMINATEDOWN = 7;
const int CLIP_TAKEPICTUREUP = 8;
const int CLIP_TAKEPICTUREDOWN = 9;
const int CLIP_END =10 ;

//The surfaces
SDL_Surface *background = NULL;
SDL_Surface *buttonSheet = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *picture = NULL;
SDL_Surface *stream = NULL;

//The event structure
SDL_Event event;

//The clip regions of the sprite sheet
SDL_Rect clips[CLIP_END];

//The button
class Button
{
    private:
    //The attributes of the button
    SDL_Rect box;
	

    //The part of the button sprite sheet that will be shown
    SDL_Rect* clip;
	SDL_Rect* randClip;

	int myup_clip;
	int mydown_clip;

    public:
    //Initialize the variables
    Button( int x, int y, int w, int h, int down_clip, int up_clip  );

    //Handles events and set the button's sprite region
    void handle_events();

    //Shows the button on the screen
    void show();
};

SDL_Surface *load_image( std::string filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0, 0xFF, 0xFF ) );
        }
    }

    //Return the optimized surface
    return optimizedImage;
}

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL)
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "OCVRS User Interface", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the button sprite sheet
    
	background = load_image("ocelot.bmp");
	apply_surface( 0, 0, background, screen );
	
	buttonSheet = load_image( "button.png" );

	
    //If there was a problem in loading the button sprite sheet
    if( buttonSheet == NULL )
    {
        return false;
    }

    //If everything loaded fine
    return true;
}

void clean_up()
{
    //Free the surface
    SDL_FreeSurface( buttonSheet );
	SDL_FreeSurface(background);
	SDL_FreeSurface(picture);
    //Quit SDL
    SDL_Quit();
}

void set_clips()
{
    //Clip the sprite sheet
   // clips[ CLIP_MOUSEOVER ].x = 0;
   // clips[ CLIP_MOUSEOVER ].y = 0;
   // clips[ CLIP_MOUSEOVER ].w = 320;
   // clips[ CLIP_MOUSEOVER ].h = 240;

  //  clips[ CLIP_MOUSEOUT ].x = 320;
   // clips[ CLIP_MOUSEOUT ].y = 0;
  //  clips[ CLIP_MOUSEOUT ].w = 320;
   // clips[ CLIP_MOUSEOUT ].h = 240;

	// Zig-zag
    clips[ CLIP_MOUSEDOWN ].x = 0;
    clips[ CLIP_MOUSEDOWN ].y = 240;
    clips[ CLIP_MOUSEDOWN ].w = 100;
    clips[ CLIP_MOUSEDOWN ].h = 100;

    clips[ CLIP_MOUSEUP ].x = 320;
    clips[ CLIP_MOUSEUP ].y = 240;
    clips[ CLIP_MOUSEUP ].w = 100;
    clips[ CLIP_MOUSEUP ].h = 100;

	// Random clips
	clips[CLIP_RANDMOUSEDOWN].x = 541;
	clips[CLIP_RANDMOUSEDOWN].y = 240;
	clips[CLIP_RANDMOUSEDOWN].h = 105;
	clips[CLIP_RANDMOUSEDOWN].w = 100;

	clips[CLIP_RANDMOUSEUP].x = 221;
	clips[CLIP_RANDMOUSEUP].y = 240;
	clips[CLIP_RANDMOUSEUP].h = 105;
	clips[CLIP_RANDMOUSEUP].w = 100;

	//Terminate Clips

	clips[CLIP_TERMINATEDOWN].x = 544;
	clips[CLIP_TERMINATEDOWN].y = 371;
	clips[CLIP_TERMINATEDOWN].h = 100;
	clips[CLIP_TERMINATEDOWN].w = 100;

	clips[CLIP_TERMINATEUP].x = 216;
	clips[CLIP_TERMINATEUP].y = 371;
	clips[CLIP_TERMINATEUP].h = 100;
	clips[CLIP_TERMINATEUP].w = 100;




	//Take Picture Clips
	clips[CLIP_TAKEPICTUREDOWN].x = 326;
	clips[CLIP_TAKEPICTUREDOWN].y = 371;
	clips[CLIP_TAKEPICTUREDOWN].h = 100;
	clips[CLIP_TAKEPICTUREDOWN].w = 100;

	clips[CLIP_TAKEPICTUREUP].x = 5;
	clips[CLIP_TAKEPICTUREUP].y = 371;
	clips[CLIP_TAKEPICTUREUP].h = 100;
	clips[CLIP_TAKEPICTUREUP].w = 100;

}

Button::Button( int x, int y, int w, int h, int down_clip, int up_clip )
{
    //Set the button's attributes
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;

	mydown_clip = down_clip;
	myup_clip = up_clip;
	
	clip = &clips[up_clip];

}

void Button::handle_events()
{
    //The mouse offsets
    int x = 0, y = 0;

    //If the mouse moved

    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN )
    {
        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;



            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                //Set the button sprite
                clip = &clips[ mydown_clip ];
				//Take Picture
				if (( x > 30) && (x < 30 + box.w)  && (y > 120) && (y < 120 + box.h))
				{
					system("start /MIN control.exe");
					picture = load_image("control.jpeg");
					apply_surface( 200, 120, picture, screen );
				}
				
				//Zig zag
				if((x > 30) && ( x< 30 + box.w) && (y > 220) && (y < 220 + box.h))
				{
					system("start /MIN zigzag.exe");
				}
				//Rand
				if((x > 30) && ( x< 30 + box.w) && (y > 320) && (y < 320 + box.h))
				{
					system("start /MIN random.exe");
				}
				//Terminate
				if((x > 30) && ( x< 30 + box.w) && (y > 20) && (y < 20 + box.h))
				{
					system("taskkill /f /im zigzag.exe");
					system("taskkill /f /im random.exe");
				}

            }
        }
    }
    //If a mouse button was released
    if( event.type == SDL_MOUSEBUTTONUP )
    {
        //If the left mouse button was released
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                //Set the button sprite
                clip = &clips[ myup_clip ];
			
            }
        }
    }
}

void Button::show()
{
    //Show the button

		apply_surface( box.x, box.y, buttonSheet, screen, clip );

		//apply_surface(box.x, box.y, buttonSheet, screen, randClip);
}

int main( int argc, char* args[] )
{
    //Quit flag
	

    bool quit = false;

    //Initialize
    if( init() == false )
    {
        return 1;
    }

    //Load the files
    if( load_files() == false )
    {
        return 1;
    }

    //Clip the sprite sheet
    set_clips();


    //Make the buttons
    Button myButton( 30, 220, 100, 100, CLIP_MOUSEDOWN, CLIP_MOUSEUP);	
	Button randomSearch(30,320,100,100,CLIP_RANDMOUSEUP,CLIP_RANDMOUSEDOWN);
	Button takePicture(30,120,100,100,CLIP_TAKEPICTUREUP,CLIP_TAKEPICTUREDOWN);
	Button terminate(30,20,100,100,CLIP_TERMINATEUP,CLIP_TERMINATEDOWN);


    //While the user hasn't quit
    while( quit == false )
    {
        //If there's events to handle
        if( SDL_PollEvent( &event ) )
        {
            //Handle button events
            myButton.handle_events();
			randomSearch.handle_events();
			takePicture.handle_events();
			terminate.handle_events();

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }
        }

        //Fill the screen white
        //SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );

        //Show the buttons
		myButton.show();
		randomSearch.show();
		takePicture.show();
		terminate.show();

		

        //Update the screen
        if( SDL_Flip( screen ) == -1 )
        {
            return 1;
        }


    }

    //Clean up
    clean_up();

    return 0;
}

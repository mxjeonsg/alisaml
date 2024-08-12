#include <stdlib.h>
#include <stdio.h>

#include "./internal/types.h"

#include <raylib.h>


#include "../internal/alisaml.c"


// Colour >> Color
typedef Color Colour;

#define SCREEN_W 800
#define SCREEN_H 600


const u8* proper_frame =
    "<Alisaml>" "\n"
    "  <Test>Content-Test</Test>" "\n"
    "  <AlisaContext>" "\n"
    "    <ScreenResolution>800x600</ScreenResolution>" "\n"
    "    <ScreenTitle>UwU</ScreenTitle>" "\n"
    "    <ScreenTargetFPS>60</ScreenTargetFPS>" "\n"
    "  </AlisaContext>" "\n"
    "  <head>" "\n"
    "    <title>Sample test</title>" "\n"
    "  </head>" "\n"
    "  <body>" "\n"
    "    <Text>Example text</Text>" "\n"
    "    <Test>Esto esta siendo demasiado demasiado estresante wbn</Test>" "\n"
    "  </body>" "\n"
    "</Alisaml>"
;

const i32 main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Sexo");
    SetTargetFPS(60);
    while(!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        BeginDrawing(); 
        {
            DrawText("Yeah, I think this is working.", 20, 20, 20, (Color){0x9, 0x7f, 0x9, 0xff});
        }
        EndDrawing();
    }
    CloseWindow();
}
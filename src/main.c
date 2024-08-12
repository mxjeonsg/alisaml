// #pragma GCC diagnostic ignored "-fpermissive"
#include <stdlib.h>
#include <stdio.h>
#include "utils/types.h"
#include <raylib.h>
#include "utils/alisaml.c"

typedef Color Colour;


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
    // XML parser context prepare
    AlisamlCtx actx = initAlisaml(proper_frame);
    alisamlctx_parsesource(&actx);

    InitWindow(800, 600, "Sexo");
    while(!WindowShouldClose()) {
        ClearBackground(RAYWHITE);
        BeginDrawing(); {
            DrawText("Sexito", 20, 20, 20, BLACK);
        } EndDrawing();
    }
    CloseWindow();

    destroyAlisaml(&actx);
}
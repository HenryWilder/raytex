#include <raylib.h>
#include <raymath.h>
#include <raytex.h>

Color Rainbow(float t);

int main()
{
    int windowWidth  = 640;
    int windowHeight = 480;
    InitWindow(windowWidth, windowHeight, "RayTeX Example");
    SetTargetFPS(240);

    RayTeX tex = GenRayTeXFrac(
        GenRayTeXFrac(
            GenRayTeXText("apple",  20, BLACK),
            GenRayTeXText("orange", 20, BLACK), 5, BLACK),
        GenRayTeXFrac(
            GenRayTeXText("banana", 20, BLACK),
            GenRayTeXText("mango",  20, BLACK), 5, BLACK), 5, BLACK);

    RayTeXPath path1 = GenRayTeXPath(
        GetRayTeXSubLocation(tex, )
    );

    while (!WindowShouldClose())
    {


        int width  = MeasureRayTeXWidth (tex);
        int height = MeasureRayTeXHeight(tex);

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRayTeX(tex, (windowWidth - width) / 2, (windowHeight - height) / 2);
        DrawFPS(0,0);
        EndDrawing();
    }

    UnloadRayTeX(tex);
    CloseWindow();
}

Color Rainbow(float t)
{
    return ColorFromHSV(t * 180.0f, 1.0f, 1.0f);
}

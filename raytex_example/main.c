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

    RayTeX tex = GenRayTeXFraction(
        GenRayTeXFraction(
            GenRayTeXText("apple",  20, BLACK),
            GenRayTeXText("orange", 20, BLACK), 5, BLACK),
        GenRayTeXFraction(
            GenRayTeXText("banana", 20, BLACK),
            GenRayTeXText("mango",  20, BLACK), 5, BLACK), 5, BLACK);

    while (!WindowShouldClose())
    {
        tex.frac[TEX_FRAC_NUMERATOR]  .frac[TEX_FRAC_NUMERATOR]  .fontSize = (int)Lerp(10.0f, 80.0f, (float)(0.5 * (1.0 - sin(GetTime()))));
        tex.frac[TEX_FRAC_DENOMINATOR].frac[TEX_FRAC_NUMERATOR]  .fontSize = (int)Lerp(10.0f, 80.0f, (float)(0.5 * (1.0 + sin(GetTime()))));
        tex.frac[TEX_FRAC_NUMERATOR]  .frac[TEX_FRAC_DENOMINATOR].fontSize = (int)Lerp(10.0f, 80.0f, (float)(0.5 * (1.0 + cos(GetTime()))));
        tex.frac[TEX_FRAC_DENOMINATOR].frac[TEX_FRAC_DENOMINATOR].fontSize = (int)Lerp(10.0f, 80.0f, (float)(0.5 * (1.0 - cos(GetTime()))));
        tex.frac[TEX_FRAC_DENOMINATOR].frac[TEX_FRAC_DENOMINATOR].color = Rainbow(GetTime());

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

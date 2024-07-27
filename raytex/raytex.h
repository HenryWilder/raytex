#ifndef RAYTEX_H
#define RAYTEX_H

#ifndef RL_COLOR_TYPE
// Color, 4 components, R8G8B8A8 (32bit)
typedef struct Color {
    unsigned char r;        // Color red value
    unsigned char g;        // Color green value
    unsigned char b;        // Color blue value
    unsigned char a;        // Color alpha value
} Color;
#endif // !RL_COLOR_TYPE

typedef enum {
    TEXSYMBOL_NEQ,
} RayTexSymbol;

#define TEX_BACKSLASH "\\\\"
#define TEX_NEQ       "\\neq"
#define TEX_HRULE     "\\hrule"

RayTexSymbol RayTeXSymbolFromName(const char *name);
int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize);
int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize);
void DrawRayTeXSymbol(RayTexSymbol symbol, int x, int y, int fontSize, Color color);

typedef enum {
    TEXMODE_MODE_TEXT,
    TEXMODE_MODE_SYMBOL,
    TEXMODE_MODE_FRAC,
} TeXMode;

enum {
    TEX_FRAC_NUMERATOR   = 0,
    TEX_FRAC_DENOMINATOR = 1,
};

typedef struct RayTeX {
    TeXMode mode;
    Color color;
    union {
        int fontSize;
        int spacing;
    };
    union {
        RayTexSymbol symbol;
        const char *text;   // Space between bottom of numerator and top of denominator
        struct RayTeX *frac; // Exactly 2 elements
    };
} RayTeX;

int MeasureRayTeXWidth(RayTeX tex);
int MeasureRayTeXHeight(RayTeX tex);

RayTeX GenRayTeXText(const char *content, int fontSize, Color color);
RayTeX GenRayTeXSymbol(RayTexSymbol symbol, int fontSize, Color color);
RayTeX GenRayTeXFraction(RayTeX numerator, RayTeX denominator, int spacing, Color color);

void UnloadRayTeX(RayTeX tex);                                                     // All children will also be unloaded

void DrawRayTeX(RayTeX tex, int x, int y);

#endif
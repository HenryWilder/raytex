#ifndef RAYTEX_H
#define RAYTEX_H

#ifdef SUPPORT_TRACELOG
    #define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

    #ifdef SUPPORT_TRACELOG_DEBUG
        #define TRACELOGD(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
    #else
        #define TRACELOGD(...) (void)0
    #endif
#else
    #define TRACELOG(level, ...) (void)0
    #define TRACELOGD(...) (void)0
#endif

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

RayTexSymbol RayTeXSymbolFromName(const char *name);
int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize);
int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize);
void DrawRayTeXSymbol(RayTexSymbol symbol, int x, int y, int fontSize, Color color);

typedef enum {
    TEXMODE_MODE_TEXT,
    TEXMODE_MODE_MATH,
} TeXMode;

typedef struct RayTeX {
    int mode;
    int fontSize;
    Color color;
    int childCount;
    struct RayTeX *children;
    const char *content;
} RayTeX;

int MeasureRayTeXWidth(RayTeX tex);
int MeasureRayTeXHeight(RayTeX tex);

RayTeX GenRayTeXText(const char *content, int mode, int fontSize, Color color);
void UnloadRayTeX(RayTeX tex);                                                     // All children will also be unloaded

void AttachRayTeXChild(RayTeX *parent, RayTeX child, int index);
RayTeX DetachRayTeXChild(RayTeX *parent, int index);

RayTeX *GetRayTeXChild(RayTeX parent, int index);                                  // The child is still attached, but you can access it to attach/detach children
void SetRayTeXChildMode(RayTeX parent, int index, int mode);
void SetRayTeXChildFontSize(RayTeX parent, int index, int fontSize);
void SetRayTeXChildColor(RayTeX parent, int index, Color color);
void SetRayTeXChildContent(RayTeX parent, int index, const char *content);

void DrawRayTeX(RayTeX tex, int x, int y);

#endif
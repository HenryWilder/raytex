#ifndef RAYTEX_H
#define RAYTEX_H
#include <stdarg.h>

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
    TEXMODE_TEXT,
    TEXMODE_SYMBOL,
    TEXMODE_FRAC,
    TEXMODE_HORIZONTAL,
    TEXMODE_VERTICAL,
    TEXMODE_MATRIX,
} TeXMode;

enum {
    TEX_FRAC_NUMERATOR   = 0,
    TEX_FRAC_DENOMINATOR = 1,
};

typedef struct RayTeX {
    TeXMode mode;
    union {
        struct {
            int fontSize;
            Color color;
            RayTexSymbol content;
        } symbol;

        struct {
            int fontSize;
            Color color;
            const char *content;
        } text;

        struct {
            int spacing;            // Space between bottom of numerator and top of denominator
            Color color;
            struct RayTeX *content; // Exactly 2 elements
        } frac;

        struct {
            int elementCount;
            struct RayTeX *content; // elementCount elements
        } horizontal;

        struct {
            int elementCount;
            struct RayTeX *content; // elementCount elements
        } vertical;

        struct {
            int rowCount;
            int columnCount;
            struct RayTeX *content; // rowCount * columnCount elements
        } matrix;
    };
} RayTeX;

typedef enum {
    TEXFIELD_TEXT_FONTSIZE,   // int
    TEXFIELD_TEXT_COLOR,      // Color
    TEXFIELD_TEXT_CONTENT,    // RayTexSymbol

    TEXFIELD_SYMBOL_FONTSIZE, // int
    TEXFIELD_SYMBOL_COLOR,    // Color
    TEXFIELD_SYMBOL_CONTENT,  // const char*

    TEXFIELD_FRAC_SPACING,    // int
    TEXFIELD_FRAC_COLOR,      // Color

    // All other members should be pathed instead of set.
    // Modifying TeX structure invalidates all saved paths.
} TeXField;

typedef enum {
    TEXPATHPARTKIND_FIELD, // field
    TEXPATHPARTKIND_CHILD, // index
    TEXPATHPARTKIND_CELL,  // row, column
} TeXPathPartKind;

struct RayTeXPathBuilder;
typedef struct RayTeXPathBuilder RayTeXPathBuilder;

typedef struct RayTeXPathPart {
    TeXPathPartKind kind;
    union {
        // End of path
        TeXField field;

        // Horizontal/Vertical
        int index;

        // Matrix
        struct {
            int rowIndex;
            int columnIndex;
        };
    };
} RayTeXPathPart;

typedef struct RayTeXPath {
    RayTeXPathPart *parts; // terminated by TEXPATHPARTKIND_FIELD
} RayTeXPath;

int MeasureRayTeXWidth(RayTeX tex);
int MeasureRayTeXHeight(RayTeX tex);

RayTeX GenRayTeXBlank();
RayTeX GenRayTeXText(const char *content, int fontSize, Color color);
RayTeX GenRayTeXSymbol(RayTexSymbol symbol, int fontSize, Color color);
RayTeX GenRayTeXFrac(RayTeX numerator, RayTeX denominator, int spacing, Color color);
RayTeX GenRayTeXHorizontal(int count, ...);
RayTeX GenRayTeXVertical(int count, ...);
RayTeX GenRayTeXMatrix(int rowCount, int columnCount, ...);

RayTeXPathBuilder *GetRayTeXElementFieldLocation(RayTeX tex, TeXField field);
RayTeXPathBuilder *GetRayTeXFieldLocation(RayTeXPathBuilder *path, TeXField field);
RayTeXPathBuilder *GetRayTeXElementSubLocation(RayTeX tex, int index); // Frac, Horizontal, Vertical
RayTeXPathBuilder *GetRayTeXSubLocation(RayTeXPathBuilder *path, int index); // Frac, Horizontal, Vertical
RayTeXPathBuilder *GetRayTeXMatrixElementSubLocation(RayTeX tex, int rowIndex, int columnIndex); // Matrix
RayTeXPathBuilder *GetRayTeXMatrixSubLocation(RayTeXPathBuilder *tex, int rowIndex, int columnIndex); // Matrix
void UnloadRayTeXPathBuilder(RayTeXPathBuilder *builder, RayTeXPathBuilder *untilBuilder);
void UnloadRayTeXPathBuilderCompletely(RayTeXPathBuilder *builder);

RayTeXPath GenRayTeXPath(RayTeXPathBuilder* builder);
void UnloadRayTeXPath(RayTeXPath path);
void SetRayTeXValue(RayTeX tex, RayTeXPath path, void* value);

void UnloadRayTeX(RayTeX tex); // All children will also be unloaded

void DrawRayTeX(RayTeX tex, int x, int y);

#endif
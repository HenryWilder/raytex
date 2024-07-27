#ifndef RAYTEX_H
#define RAYTEX_H
#include <stdarg.h>
#include <raylib.h>

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
    TEXMODE_SPACE,
    TEXMODE_VSPACE,
    TEXMODE_TEXT,
    TEXMODE_SYMBOL,
    TEXMODE_FRAC,
    TEXMODE_HORIZONTAL,
    TEXMODE_VERTICAL,
    TEXMODE_MATRIX,
} TeXMode;

enum {
    QUAD_SIZE      =          18, // 18 mu
    THINSPACE_SIZE =           3, //  3 mu
    BINSPACE_SIZE  =           4, //  4 mu
    RELSPACE_SIZE  =           5, //  5 mu
    EXSPACE_SIZE   =          -3, // -3 mu
    QQUAD_SIZE     = 2*QUAD_SIZE, // 36 mu
};

#define MU_TO_PIXELS(mu, fontSize) (((mu)*(fontSize))/18)

#define BLANK_TEX CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size = 0 } }                // empty cell (for matrix)

#define QUAD      CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size =      QUAD_SIZE } }   // 18 mu \quad
#define THINSPACE CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size = THINSPACE_SIZE } }   //  3 mu \,
#define BINSPACE  CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size =  BINSPACE_SIZE } }   //  4 mu \:
#define RELSPACE  CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size =  RELSPACE_SIZE } }   //  5 mu \;
#define EXSPACE   CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size =   EXSPACE_SIZE } }   // -3 mu \!
#define QQUAD     CLITERAL(RayTeX){ .mode = TEXMODE_SPACE, .space = { .size =     QQUAD_SIZE } }   // 36 mu \qquad

#define VQUAD      CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size =      QUAD_SIZE } } // 18 mu vertical \quad
#define VTHINSPACE CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size = THINSPACE_SIZE } } //  3 mu vertical \,
#define VBINSPACE  CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size =  BINSPACE_SIZE } } //  4 mu vertical \:
#define VRELSPACE  CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size =  RELSPACE_SIZE } } //  5 mu vertical \;
#define VEXSPACE   CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size =   EXSPACE_SIZE } } // -3 mu vertical \!
#define VQQUAD     CLITERAL(RayTeX){ .mode = TEXMODE_VSPACE, .space = { .size =     QQUAD_SIZE } } // 36 mu vertical \qquad

typedef enum {
    HORIZONTAL_TEXALIGN_TOP,
    HORIZONTAL_TEXALIGN_CENTER,
    HORIZONTAL_TEXALIGN_BOTTOM,

    VERTICAL_TEXALIGN_LEFT   = HORIZONTAL_TEXALIGN_TOP,
    VERTICAL_TEXALIGN_CENTER = HORIZONTAL_TEXALIGN_CENTER,
    VERTICAL_TEXALIGN_RIGHT  = HORIZONTAL_TEXALIGN_BOTTOM,
} TeXAlign;

enum {
    TEX_FRAC_NUMERATOR   = 0,
    TEX_FRAC_DENOMINATOR = 1,
};

typedef struct RayTeX {
    Color overrideColor;
    int overrideFontSize;
    Font *overrideFont;               // NULL if not overriding
    int isOverridingColor    : 1;     // bool
    int isOverridingFontSize : 1;     // bool
    int fillsParentCrossAxis : 1;     // bool
    int mode : (sizeof(int) * 8 - 3); // TeXMode
    union {
        struct {
            int size;               // Measured in mu (18 mu = current font size)
        } space;

        struct {
            RayTexSymbol content;
        } symbol;

        struct {
            const char *content;
        } text;

        struct {
            int spacing;            // Space between bottom of numerator and top of denominator
            struct RayTeX *content; // Exactly 2 elements
        } frac;

        struct {
            TeXAlign alignContent;
            int elementCount;
            struct RayTeX *content; // elementCount elements
        } horizontal;

        struct {
            TeXAlign alignContent;
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

int MeasureRayTeXWidthEx(Font font, RayTeX tex, int fontSize);
int MeasureRayTeXHeightEx(Font font, RayTeX tex, int fontSize);

int MeasureRayTeXWidth(RayTeX tex, int fontSize);
int MeasureRayTeXHeight(RayTeX tex, int fontSize);

RayTeX SetRayTeXColor(RayTeX tex, Color color);     // Sets the TeX color
RayTeX SetRayTeXFontSize(RayTeX tex, int fontSize); // Sets the TeX font size
RayTeX SetRayTeXFont(RayTeX tex, Font font);        // Sets the TeX font

RayTeX GenRayTeXSpace(int mu);
RayTeX GenRayTeXVSpace(int mu);
RayTeX GenRayTeXText(const char *content);
RayTeX GenRayTeXSymbol(RayTexSymbol symbol);
RayTeX GenRayTeXFrac(RayTeX numerator, RayTeX denominator, int spacing);
RayTeX GenRayTeXHorizontal(TeXAlign alignContent, int count, ...);
RayTeX GenRayTeXVertical(TeXAlign alignContent, int count, ...);
RayTeX GenRayTeXMatrix(int rowCount, int columnCount, ...);
void UnloadRayTeX(RayTeX tex); // All children will also be unloaded

void DrawRayTeXEx(Font font, RayTeX tex, int x, int y, int fontSize, Color color);
void DrawRayTeXCenteredEx(Font font, RayTeX tex, int x, int y, int width, int height, int fontSize, Color color);
void DrawRayTeXCenteredRecEx(Font font, RayTeX tex, Rectangle rec, int fontSize, Color color);

void DrawRayTeX(RayTeX tex, int x, int y, int fontSize, Color color);
void DrawRayTeXCentered(RayTeX tex, int x, int y, int width, int height, int fontSize, Color color);
void DrawRayTeXCenteredRec(RayTeX tex, Rectangle rec, int fontSize, Color color);

#endif
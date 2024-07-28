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
Vector2 MeasureRayTeXSymbolEx(Font font, RayTexSymbol symbol, float fontSize);
int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize);
int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize);
void DrawRayTeXSymbolEx(Font font, RayTexSymbol symbol, Vector2 position, float fontSize, Color color);
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

struct RayTeX;

typedef struct RayTeXRef {
    bool isOwned;
    struct RayTeX *ptr;
} RayTeXRef;

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
            int size; // Measured in mu (18 mu = current font size)
        } space;

        struct {
            RayTexSymbol content;
        } symbol;

        struct {
            const char *content;
        } text;

        struct {
            RayTeXRef content[2];
        } frac;

        struct {
            int elementCount;
            RayTeXRef *content; // elementCount elements
        } horizontal;

        struct {
            int elementCount;
            RayTeXRef *content; // elementCount elements
        } vertical;

        struct {
            int rowCount;
            int columnCount;
            RayTeXRef *content; // rowCount*columnCount elements
        } matrix;
    };
} RayTeX;

Vector2 MeasureRayTeXEx(Font font, RayTeX tex, int fontSize);
int MeasureRayTeXWidth(RayTeX tex, int fontSize);
int MeasureRayTeXHeight(RayTeX tex, int fontSize);

void UpdateRayTeXColor(RayTeX *tex, Color color);
void UpdateRayTeXFontSize(RayTeX *tex, int fontSize);
void UpdateRayTeXFont(RayTeX *tex, Font font);
void ClearRayTeXColor(RayTeX *tex);              // Clears the element's override so that it inherits from its parent again
void ClearRayTeXFontSize(RayTeX *tex);           // Clears the element's override so that it inherits from its parent again
void ClearRayTeXFont(RayTeX *tex);               // Clears the element's override so that it inherits from its parent again

RayTeX RayTeXColor(RayTeX tex, Color color);     // Sets the TeX color of the element and returns the modified element - useful for initialization
RayTeX RayTeXFontSize(RayTeX tex, int fontSize); // Sets the TeX font size of the element and returns the modified element - useful for initialization
RayTeX RayTeXFont(RayTeX tex, Font font);        // Sets the TeX font of the element and returns the modified element - useful for initialization

// Remember that you can also use the `&` operator if you want to update the element itself and not one of its children

RayTeX *RayTeXFracNumerator(RayTeX *fracTex);                     // Returns a pointer to the element for updating after initialization
RayTeX *RayTeXFracDenominator(RayTeX *fracTex);                   // Returns a pointer to the element for updating after initialization
RayTeX *RayTeXHorizontalChild(RayTeX *horizontalTex, int index);  // Returns a pointer to the element for updating after initialization
RayTeX *RayTeXVerticalChild(RayTeX *verticalTex, int index);      // Returns a pointer to the element for updating after initialization
RayTeX *RayTeXMatrixCell(RayTeX *matrixTex, int row, int column); // Returns a pointer to the element for updating after initialization

RayTeX GenRayTeXSpace(int mu);
RayTeX GenRayTeXVSpace(int mu);
RayTeX GenRayTeXText(const char *content);
RayTeX GenRayTeXSymbol(RayTexSymbol symbol);
RayTeX GenRayTeXFrac(RayTeX numerator, RayTeX denominator);
RayTeX GenRayTeXFracVP(RayTeX numerator, RayTeX *denominator);
RayTeX GenRayTeXFracPV(RayTeX *numerator, RayTeX denominator);
RayTeX GenRayTeXFracPP(RayTeX *numerator, RayTeX *denominator);
RayTeX GenRayTeXHorizontal(const char *fmt, ...); // fmt: 'p' for pointer, 'v' for value
RayTeX GenRayTeXVertical(const char *fmt, ...);   // fmt: 'p' for pointer, 'v' for value
RayTeX GenRayTeXMatrix(const char *fmt, ...);     // fmt: 'p' for pointer, 'v' for value, '&' for column skip, '\\' for end of row

// Unloads the tex and all owned children.
// Any child that was added by value is owned. Any child that was added by pointer is unowned.
// Unowned children will not be unloaded. They may be shared, and need to be unloaded separately.
void UnloadRayTeX(RayTeX tex);

void DrawRayTeX(RayTeX tex, int x, int y, int fontSize, Color color);
void DrawRayTeXEx(Font font, RayTeX tex, int x, int y, int fontSize, Color color);

void DrawRayTeXCentered(RayTeX tex, int x, int y, int width, int height, int fontSize, Color color);
void DrawRayTeXCenteredRec(RayTeX tex, Rectangle rec, int fontSize, Color color);
void DrawRayTeXCenteredPro(Font font, RayTeX tex, Rectangle rec, float fontSize, Color color);

#endif
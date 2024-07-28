#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "raytex.h"

#define MAX_TEXT_BUFFER_LENGTH 1024
#define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

enum {
    TEXFRAC_OVERHANG  = 4, // Measured in mu
    TEXFRAC_SPACING   = 2, // Measured in mu
    TEXFRAC_THICKNESS = 1, // Measured in mu
};

RayTexSymbol RayTeXSymbolFromName(const char *name)
{
    // todo: Implement this as a lookup table once there are a lot of symbols
    if (TextIsEqual(name, "neq")) return TEXSYMBOL_NEQ;

    TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \"%s\"", name);
}

Vector2 MeasureRayTeXSymbolEx(Font font, RayTexSymbol symbol, float fontSize)
{
    Vector2 size = { 0 };
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
    {
        Vector2 baseSize = MeasureTextEx(font, "=", fontSize, fontSize / 10);
        size.x = baseSize.x + MU_TO_PIXELS(RELSPACE_SIZE*2.0f, fontSize);
        size.y = baseSize.y;
    }
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
    return size;
}

int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize)
{
    return (int)MeasureRayTeXSymbolEx(GetFontDefault(), symbol, (float)fontSize).x;
}

int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize)
{
    return (int)MeasureRayTeXSymbolEx(GetFontDefault(), symbol, (float)fontSize).y;
}

void DrawRayTeXSymbolEx(Font font, RayTexSymbol symbol, Vector2 position, float fontSize, Color color)
{
    // Todo: Implement these with textures or a custom font at some point
    Vector2 size = MeasureRayTeXSymbolEx(font, symbol, fontSize);
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
    {
        float space = MU_TO_PIXELS((float)RELSPACE_SIZE, (float)fontSize);

        Vector2 positionWithSpace = { 0 };
        positionWithSpace.x = position.x + space;
        positionWithSpace.y = position.y;
        DrawTextEx(font, "=", positionWithSpace, fontSize, fontSize / 10, color);

        Vector2 crossBottomLeft = { position.x + space, position.y + size.y };
        Vector2 crossTopRight = { position.x + size.x - space, position.y };
        DrawLineV(crossBottomLeft, crossTopRight, color);
    }
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
}

void DrawRayTeXSymbol(RayTexSymbol symbol, int x, int y, int fontSize, Color color)
{
    Vector2 position = { 0 };
    position.x = (float)x;
    position.y = (float)y;
    DrawRayTeXSymbolEx(GetFontDefault(), symbol, position, (float)fontSize, color);
}

static Vector2 rMeasureRayTeX(const Font *font, const RayTeX *tex, float fontSize)
{
    Vector2 size = { 0 };
    if (tex->isOverridingFontSize) fontSize = (float)tex->overrideFontSize;
    if (tex->overrideFont != NULL) font = tex->overrideFont;
    switch (tex->mode)
    {
    case TEXMODE_SPACE:
        size.x = MU_TO_PIXELS((float)tex->space.size, fontSize);
        size.y = 0.0f;
        break;

    case TEXMODE_VSPACE:
        size.x = 0.0f;
        size.y = MU_TO_PIXELS((float)tex->space.size, fontSize);
        break;

    case TEXMODE_TEXT:
        size = MeasureTextEx(*font, tex->text.content, fontSize, fontSize / 10);
        break;

    case TEXMODE_SYMBOL:
        size = MeasureRayTeXSymbolEx(*font, tex->symbol.content, fontSize);
        break;

    case TEXMODE_FRAC:
    {
        Vector2 numeratorSize = rMeasureRayTeX(font, tex->frac.content[TEX_FRAC_NUMERATOR].ptr, fontSize);
        Vector2 denominatorSize = rMeasureRayTeX(font, tex->frac.content[TEX_FRAC_DENOMINATOR].ptr, fontSize);
        size.x = (numeratorSize.x > denominatorSize.x ? numeratorSize.x : denominatorSize.x) + MU_TO_PIXELS(TEXFRAC_OVERHANG*2.0f, fontSize);
        size.y = numeratorSize.y + denominatorSize.y + MU_TO_PIXELS(TEXFRAC_SPACING*2.0f+TEXFRAC_THICKNESS, fontSize);
    }
        break;

    case TEXMODE_HORIZONTAL:
    {
        float totalWidth = 0.0f;
        float maxHeight = 0.0f;
        for (int i = 0; i < tex->horizontal.elementCount; ++i)
        {
            RayTeX *element = tex->horizontal.content[i].ptr;
            Vector2 elementSize = rMeasureRayTeX(font, element, fontSize);
            totalWidth += elementSize.x;

            if (element->mode == TEXMODE_FRAC)
            {
                float spacing = MU_TO_PIXELS((float)TEXFRAC_SPACING, fontSize);
                const Vector2 numeratorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_NUMERATOR].ptr, fontSize);
                const Vector2 denominatorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_DENOMINATOR].ptr, fontSize);
                elementSize.y += (numeratorSize.y - denominatorSize.y + spacing) / 2;
            }
            if (elementSize.y > maxHeight) maxHeight = elementSize.y;
        }
        size.x = totalWidth;
        size.y = maxHeight;
    }
        break;

    case TEXMODE_VERTICAL:
    {
        float maxWidth = 0.0f;
        float totalHeight = 0.0f;
        for (int i = 0; i < tex->horizontal.elementCount; ++i)
        {
            Vector2 elementSize = rMeasureRayTeX(font, tex->horizontal.content[i].ptr, fontSize);
            if (elementSize.x > maxWidth) maxWidth = elementSize.x;
            totalHeight += elementSize.y;
        }
        size.x = maxWidth;
        size.y = totalHeight;
    }
        break;

    case TEXMODE_MATRIX:
    {
        // todo: measure matrix
        TRACELOG(LOG_WARNING, "RAYTEX: Matrix measuring not implemented");
    }
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex->mode);
    }
    return size;
}

Vector2 MeasureRayTeXEx(Font font, RayTeX tex, int fontSize)
{
    return rMeasureRayTeX(&font, &tex, (float)fontSize);
}
int MeasureRayTeXWidth(RayTeX tex, int fontSize)
{
    return (int)MeasureRayTeXEx(GetFontDefault(), tex, fontSize).x;
}
int MeasureRayTeXHeight(RayTeX tex, int fontSize)
{
    return (int)MeasureRayTeXEx(GetFontDefault(), tex, fontSize).y;
}

void UpdateRayTeXColor(RayTeX *tex, Color color)
{
    tex->overrideColor = color;
    tex->isOverridingColor = true;
}

void UpdateRayTeXFontSize(RayTeX *tex, int fontSize)
{
    tex->overrideFontSize = fontSize;
    tex->isOverridingFontSize = true;
}

void UpdateRayTeXFont(RayTeX *tex, Font font)
{
    if (tex->overrideFont == NULL) tex->overrideFont = RL_MALLOC(sizeof(Font));

    if (tex->overrideFont != NULL) *tex->overrideFont = font;
    else TRACELOG(LOG_ERROR, "RAYTEX: UpdateRayTeXFont() failed to allocate");
}

void ClearRayTeXColor(RayTeX *tex)
{
    tex->isOverridingColor = false;
}

void ClearRayTeXFontSize(RayTeX *tex)
{
    tex->isOverridingFontSize = false;
}

void ClearRayTeXFont(RayTeX *tex)
{
    RL_FREE(tex->overrideFont);
    tex->overrideFont = NULL;
}

RayTeX RayTeXColor(RayTeX tex, Color color)
{
    UpdateRayTeXColor(&tex, color);
    return tex;
}

RayTeX RayTeXFontSize(RayTeX tex, int fontSize)
{
    UpdateRayTeXFontSize(&tex, fontSize);
    return tex;
}

RayTeX RayTeXFont(RayTeX tex, Font font)
{
    UpdateRayTeXFont(&tex, font);
    return tex;
}

RayTeX *RayTeXFracNumerator(RayTeX *fracTex)
{
    if (fracTex->mode != TEXMODE_FRAC) TRACELOG(LOG_WARNING, "RAYTEX: RayTeXFracNumerator() only valid for TEXMODE_FRAC");
    return fracTex->frac.content[TEX_FRAC_NUMERATOR].ptr;
}

RayTeX *RayTeXFracDenominator(RayTeX *fracTex)
{
    if (fracTex->mode != TEXMODE_FRAC) TRACELOG(LOG_WARNING, "RAYTEX: RayTeXFracDenominator() only valid for TEXMODE_FRAC");
    return fracTex->frac.content[TEX_FRAC_DENOMINATOR].ptr;
}

RayTeX *RayTeXHorizontalChild(RayTeX *horizontalTex, int index)
{
    if (horizontalTex->mode != TEXMODE_HORIZONTAL) TRACELOG(LOG_WARNING, "RAYTEX: RayTeXHorizontalChild() only valid for TEXMODE_HORIZONTAL");
    if (index < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXHorizontalChild() index (%i) cannot be negative", index);
        //index = 0;
    }
    int lastChildIndex = horizontalTex->horizontal.elementCount - 1;
    if (index > lastChildIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXHorizontalChild() index (%i) cannot be higher than the index of horizontalTex's last child (%i)",
                 index, lastChildIndex);
        //index = lastChildIndex;
    }
    return horizontalTex->horizontal.content[index].ptr;
}

RayTeX *RayTeXVerticalChild(RayTeX *verticalTex, int index)
{
    if (verticalTex->mode != TEXMODE_VERTICAL) TRACELOG(LOG_WARNING, "RAYTEX: RayTeXVerticalChild() only valid for TEXMODE_VERTICAL");
    if (index < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXVerticalChild() index (%i) cannot be negative", index);
        //index = 0;
    }
    int lastChildIndex = verticalTex->horizontal.elementCount - 1;
    if (index > lastChildIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXVerticalChild() index (%i) cannot be higher than the index of verticalTex's last child (%i)",
                 index, lastChildIndex);
        //index = lastChildIndex;
    }
    return verticalTex->vertical.content[index].ptr;
}

RayTeX *RayTeXMatrixCell(RayTeX *matrixTex, int rowIndex, int columnIndex)
{
    if (matrixTex->mode != TEXMODE_MATRIX) TRACELOG(LOG_WARNING, "RAYTEX: RayTeXMatrixCell() only valid for TEXMODE_MATRIX");

    if (rowIndex < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXMatrixCell() rowIndex (%i) cannot be negative", rowIndex);
        //rowIndex = 0;
    }
    int lastRowIndex = matrixTex->matrix.rowCount - 1;
    if (rowIndex > lastRowIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXMatrixCell() rowIndex (%i) cannot be higher than the index of matrixTex's last row (%i)",
                 rowIndex, lastRowIndex);
        //rowIndex = lastRowIndex;
    }

    if (columnIndex < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXMatrixCell() columnIndex (%i) cannot be negative", columnIndex);
        //columnIndex = 0;
    }
    int lastColumnIndex = matrixTex->matrix.columnCount - 1;
    if (columnIndex > lastColumnIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: RayTeXMatrixCell() columnIndex (%i) cannot be higher than the index of matrixTex's last column (%i)",
                 columnIndex, lastColumnIndex);
        //columnIndex = lastColumnIndex;
    }

    return matrixTex->matrix.content[rowIndex * matrixTex->matrix.columnCount + columnIndex].ptr;
}

RayTeX GenRayTeXSpace(int mu)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_SPACE;
    element.space.size = mu;
    TRACELOG(LOG_INFO, "RAYTEX: TeX %i mu horizontal space element generated successfully", mu);
    return element;
}

RayTeX GenRayTeXVSpace(int mu)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_VSPACE;
    element.space.size = mu;
    TRACELOG(LOG_INFO, "RAYTEX: TeX %i mu vertical space element generated successfully", mu);
    return element;
}

RayTeX GenRayTeXText(const char *content)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_TEXT;
    element.text.content = content;
    TRACELOG(LOG_INFO, "RAYTEX: TeX text element \"%s\" generated successfully", content);
    return element;
}

RayTeX GenRayTeXTextf(const char *fmt, ...)
{
    char buffer[MAX_TEXT_BUFFER_LENGTH];
    va_list args;
    va_start(args, fmt);
    vsprintf_s(buffer, MAX_TEXT_BUFFER_LENGTH, fmt, args);
    va_end(args);
    size_t sizeUsed = sizeof(char) * (strlen(buffer) + 1);
    char *memory = RL_MALLOC(sizeUsed);
    if (memory != NULL)
    {
        memcpy(memory, buffer, sizeUsed);
        RayTeX element = { 0 };
        element.mode = TEXMODE_TEXT;
        element.text.isOwned = true;
        element.text.content = memory;
        TRACELOG(LOG_INFO, "RAYTEX: TeX text element \"%s\" generated successfully", buffer);
        return element;
    }
    else TRACELOG(LOG_ERROR, "GenRayTeXTextf() failed to allocate");
}

RayTeX GenRayTeXSymbol(RayTexSymbol symbol)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_SYMBOL;
    element.symbol.content = symbol;
    TRACELOG(LOG_INFO, "RAYTEX: TeX symbol element generated successfully");
    return element;
}

static RayTeXRef RayTeXRefFromValue(RayTeX value)
{
    RayTeX *pointer = RL_MALLOC(sizeof(RayTeX));
    if (pointer != NULL)
    {
        *pointer = value;
        RayTeXRef ref = { 0 };
        ref.isOwned = true;
        ref.ptr = pointer;
        return ref;
    }
    else TRACELOG(LOG_ERROR, "RayTeXRefFromValue() failed to allocate");
}

static RayTeXRef RayTeXRefFromPointer(RayTeX *pointer)
{
    RayTeXRef ref = { 0 };
    ref.isOwned = false;
    ref.ptr = pointer;
    return ref;
}

RayTeX GenRayTeXFrac(char fmt0, char fmt1, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_FRAC;
    va_list args;
    va_start(args, fmt1);
    for (int i = 0; i < 2; ++i)
    {
        char ch = i == 0 ? fmt0 : fmt1;
        switch (ch)
        {
            case ' ': element.frac.content[i] = RayTeXRefFromValue(GenRayTeXSpace(va_arg(args, int)));           break;
            case 't': element.frac.content[i] = RayTeXRefFromValue(GenRayTeXText(va_arg(args, const char*)));    break;
            case 's': element.frac.content[i] = RayTeXRefFromValue(GenRayTeXSymbol(va_arg(args, RayTexSymbol))); break;
            case 'i': element.frac.content[i] = RayTeXRefFromValue(GenRayTeXTextf("%i", va_arg(args, int)));     break;
            case 'v': element.frac.content[i] = RayTeXRefFromValue(va_arg(args, RayTeX));                        break;
            case 'p': element.frac.content[i] = RayTeXRefFromPointer(va_arg(args, RayTeX*));                     break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: GenRayTeXFrac() at index %i of fmt '%c','%c': meaning of '%c' is unknown", i, fmt0, fmt1, ch);
        }
    }
    va_end(args);
    TRACELOG(LOG_INFO, "RAYTEX: TeX fraction element generated successfully");
    return element;
}

RayTeX GenRayTeXHorizontal(const char *fmt, ...)
{
    int count = (int)strlen(fmt);
    RayTeX element = { 0 };
    element.mode = TEXMODE_HORIZONTAL;
    element.horizontal.elementCount = count;
    element.horizontal.content = RL_MALLOC(count * sizeof(RayTeXRef));
    if (element.horizontal.content != NULL)
    {
        int argC = 0;
        va_list args;
        va_start(args, fmt);
        for (int i = 0; i < count; ++i)
        {
            switch (fmt[i])
            {
            case ' ': element.horizontal.content[i] = RayTeXRefFromValue(GenRayTeXSpace(va_arg(args, int)));           break;
            case 't': element.horizontal.content[i] = RayTeXRefFromValue(GenRayTeXText(va_arg(args, const char*)));    break;
            case 's': element.horizontal.content[i] = RayTeXRefFromValue(GenRayTeXSymbol(va_arg(args, RayTexSymbol))); break;
            case 'i': element.horizontal.content[i] = RayTeXRefFromValue(GenRayTeXTextf("%i", va_arg(args, int)));     break;
            case 'v': element.horizontal.content[i] = RayTeXRefFromValue(va_arg(args, RayTeX));                        break;
            case 'p': element.horizontal.content[i] = RayTeXRefFromPointer(va_arg(args, RayTeX*));                     break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: GenRayTeXHorizontal() at index %i of fmt \"%s\": meaning of '%c' is unknown", i, fmt, fmt[i]);
            }
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX horizontal with %i elements generated successfully", count);
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXHorizontal() failed to allocate");
    return element;
}

// WARNING: Shallow copies of the elements are created.
// Unloading them outside of the vertical will also unload them for the vertical,
// and unloading the vertical will also unload them outside of the vertical.
RayTeX GenRayTeXVertical(const char *fmt, ...)
{
    int count = (int)strlen(fmt);
    RayTeX element = { 0 };
    element.mode = TEXMODE_VERTICAL;
    element.vertical.elementCount = count;
    element.vertical.content = RL_MALLOC(count * sizeof(RayTeXRef));
    if (element.vertical.content != NULL)
    {
        va_list args;
        va_start(args, fmt);
        for (int i = 0; i < count; ++i)
        {
            switch (fmt[i])
            {
            case ' ': element.vertical.content[i] = RayTeXRefFromValue(GenRayTeXSpace(va_arg(args, int)));           break;
            case 't': element.vertical.content[i] = RayTeXRefFromValue(GenRayTeXText(va_arg(args, const char*)));    break;
            case 's': element.vertical.content[i] = RayTeXRefFromValue(GenRayTeXSymbol(va_arg(args, RayTexSymbol))); break;
            case 'i': element.vertical.content[i] = RayTeXRefFromValue(GenRayTeXTextf("%i", va_arg(args, int)));     break;
            case 'v': element.vertical.content[i] = RayTeXRefFromValue(va_arg(args, RayTeX));                        break;
            case 'p': element.vertical.content[i] = RayTeXRefFromPointer(va_arg(args, RayTeX*));                     break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: GenRayTeXVertical() at index %i of fmt \"%s\": meaning of '%c' is unknown", i, fmt, fmt[i]);
            }
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical with %i elements generated successfully", count);
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXVertical() failed to allocate");
    return element;
}

// WARNING: Shallow copies of the elements are created.
// Unloading them outside of the matrix will also unload them for the matrix,
// and unloading the matrix will also unload them outside of the matrix.
RayTeX GenRayTeXMatrix(const char *fmt, ...)
{
    int rowCount = 0;
    int columnCount = 0;
    {
        int currentRowColumnCount = 0;
        for (const char *c = fmt; *c; ++c)
        {
            if (*c != '\\')
            {
                ++rowCount;
                if (currentRowColumnCount > columnCount) columnCount = currentRowColumnCount;
                currentRowColumnCount = 0;
            }
            else ++currentRowColumnCount;
        }
    }

    RayTeX element = { 0 };
    element.mode = TEXMODE_MATRIX;
    element.matrix.rowCount = rowCount;
    element.matrix.columnCount = columnCount;
    int elementCount = rowCount * columnCount;
    element.matrix.content = RL_MALLOC(elementCount * sizeof(RayTeXRef));
    if (element.matrix.content != NULL)
    {
        va_list args;
        va_start(args, fmt);
        int index = 0;
        const char *c = fmt;
        int row = 0;
        int column = 0;
        for (const char *c = fmt; *c; ++c)
        {
            switch (*c)
            {
            case ' ': element.matrix.content[index] = RayTeXRefFromValue(GenRayTeXSpace(va_arg(args, int)));           break;
            case 't': element.matrix.content[index] = RayTeXRefFromValue(GenRayTeXText(va_arg(args, const char*)));    break;
            case 's': element.matrix.content[index] = RayTeXRefFromValue(GenRayTeXSymbol(va_arg(args, RayTexSymbol))); break;
            case 'i': element.matrix.content[index] = RayTeXRefFromValue(GenRayTeXTextf("%i", va_arg(args, int)));     break;
            case 'v': element.matrix.content[index] = RayTeXRefFromValue(va_arg(args, RayTeX));                        break;
            case 'p': element.matrix.content[index] = RayTeXRefFromPointer(va_arg(args, RayTeX*));                     break;
            case '&': element.matrix.content[index] = RayTeXRefFromValue(BLANK_TEX);
            case '\\':
                for (; column < columnCount; ++column, ++index) { element.matrix.content[index] = RayTeXRefFromValue(BLANK_TEX); }
                break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: GenRayTeXMatrix() at index %i of fmt \"%s\": meaning of '%c' is unknown", index, fmt, *c);
            }
            ++column;

            if (column == columnCount)
            {
                column = 0;
                ++row;
            }
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX matrix with %i elements (%i rows x %i columns) generated successfully", elementCount, rowCount, columnCount);
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXMatrix() failed to allocate");
    return element;
}

static void UnloadAndFreeRayTeXRefIfOwned(RayTeXRef ref)
{
    if (ref.isOwned)
    {
        UnloadRayTeX(*ref.ptr);
        RL_FREE(ref.ptr);
    }
    else TRACELOG(LOG_INFO, "RAYTEX: potentially-shared child visited during unloading process has not been unloaded");
}

void UnloadRayTeX(RayTeX tex)
{
    RL_FREE(tex.overrideFont);
    switch (tex.mode)
    {
    case TEXMODE_SPACE:
        TRACELOG(LOG_INFO, "RAYTEX: TeX horizontal space element unloaded successfully");
        break;

    case TEXMODE_VSPACE:
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical space element unloaded successfully");
        break;

    case TEXMODE_TEXT:
        if (tex.text.isOwned) RL_FREE(tex.text.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX text element unloaded successfully");
        break;

    case TEXMODE_SYMBOL:
        TRACELOG(LOG_INFO, "RAYTEX: TeX symbol element unloaded successfully");
        break;

    case TEXMODE_FRAC:
        UnloadAndFreeRayTeXRefIfOwned(tex.frac.content[TEX_FRAC_NUMERATOR]);
        UnloadAndFreeRayTeXRefIfOwned(tex.frac.content[TEX_FRAC_DENOMINATOR]);
        TRACELOG(LOG_INFO, "RAYTEX: TeX fraction element unloaded successfully");
        break;

    case TEXMODE_HORIZONTAL:
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            UnloadAndFreeRayTeXRefIfOwned(tex.horizontal.content[i]);
        }
        RL_FREE(tex.horizontal.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX horizontal element unloaded successfully");
        break;

    case TEXMODE_VERTICAL:
        for (int i = 0; i < tex.vertical.elementCount; ++i)
        {
            UnloadAndFreeRayTeXRefIfOwned(tex.vertical.content[i]);
        }
        RL_FREE(tex.vertical.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical element unloaded successfully");
        break;

    case TEXMODE_MATRIX:
        for (int i = 0; i < tex.matrix.rowCount * tex.matrix.columnCount; ++i)
        {
            UnloadAndFreeRayTeXRefIfOwned(tex.matrix.content[i]);
        }
        RL_FREE(tex.matrix.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX matrix element unloaded successfully");
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Failed to unload TeX element with unknown mode [%i]", tex.mode);
    }
}

static void rDrawRayTeX(const Font *font, const RayTeX *tex, Vector2 position, float fontSize, Color color)
{
    if (tex->isOverridingColor) color = tex->overrideColor;
    if (tex->isOverridingFontSize) fontSize = (float)tex->overrideFontSize;
    if (tex->overrideFont != NULL) font = tex->overrideFont;

    // boxes around everything
#if 0
    Vector2 texSize = rMeasureRayTeX(font, tex, fontSize);
    DrawRectangleLines(position.x, position.y, texSize.x, texSize.y, MAGENTA);
#endif

    switch (tex->mode)
    {
    case TEXMODE_SPACE:
    case TEXMODE_VSPACE:
        break;

    case TEXMODE_TEXT:
        DrawTextEx(*font, tex->text.content, position, fontSize, fontSize / 10, color);
        break;

    case TEXMODE_SYMBOL:
        DrawRayTeXSymbolEx(*font, tex->symbol.content, position, fontSize, color);
        break;

    case TEXMODE_FRAC:
    {
        RayTeX *numerator = tex->frac.content[TEX_FRAC_NUMERATOR].ptr;
        RayTeX *denominator = tex->frac.content[TEX_FRAC_DENOMINATOR].ptr;
        
        Vector2 size = rMeasureRayTeX(font, tex, fontSize);
        Vector2 numeratorSize = rMeasureRayTeX(font, numerator, fontSize);
        Vector2 denominatorSize = rMeasureRayTeX(font, denominator, fontSize);

        float spacing = MU_TO_PIXELS((float)TEXFRAC_SPACING, fontSize);

        Vector2 numeratorPosition = { 0 };
        numeratorPosition.x = position.x + (size.x - numeratorSize.x) / 2.0f;
        numeratorPosition.y = position.y;
        rDrawRayTeX(font, numerator, numeratorPosition, fontSize, color);

        Rectangle ruleRec = { 0 };
        ruleRec.x = position.x;
        ruleRec.y = position.y + numeratorSize.y + spacing;
        ruleRec.width = size.x;
        ruleRec.height = MU_TO_PIXELS((float)TEXFRAC_THICKNESS, fontSize);
        DrawRectangleRec(ruleRec, color);

        Vector2 denominatorPosition = { 0 };
        denominatorPosition.x = position.x + (size.x - denominatorSize.x) / 2.0f;
        denominatorPosition.y = ruleRec.y + ruleRec.height + spacing;
        rDrawRayTeX(font, denominator, denominatorPosition, fontSize, color);

    }
        break;

    case TEXMODE_HORIZONTAL:
    {
        float yOffsetExtra = 0.0f;
        for (int i = 0; i < tex->horizontal.elementCount; ++i)
        {
            RayTeX *element = tex->horizontal.content[i].ptr;
            if (element->mode == TEXMODE_FRAC)
            {
                float spacing = MU_TO_PIXELS((float)TEXFRAC_SPACING, fontSize);
                const Vector2 numeratorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_NUMERATOR].ptr, fontSize);
                const Vector2 denominatorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_DENOMINATOR].ptr, fontSize);
                float excess = (numeratorSize.y - denominatorSize.y + spacing) / 2;
                if (excess > yOffsetExtra) yOffsetExtra = excess;
            }
        }
        position.y += yOffsetExtra;
        Vector2 horizontalSize = rMeasureRayTeX(font, tex, fontSize);
        for (int i = 0; i < tex->horizontal.elementCount; ++i)
        {
            RayTeX *element = tex->horizontal.content[i].ptr;
            const Vector2 size = rMeasureRayTeX(font, element, fontSize);
            float yOffset = (horizontalSize.y - size.y) / 2;
            if (element->mode == TEXMODE_FRAC)
            {
                float spacing = MU_TO_PIXELS((float)TEXFRAC_SPACING, fontSize);
                const Vector2 numeratorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_NUMERATOR].ptr, fontSize);
                const Vector2 denominatorSize = rMeasureRayTeX(font, element->frac.content[TEX_FRAC_DENOMINATOR].ptr, fontSize);
                yOffset = yOffset - (numeratorSize.y - denominatorSize.y + spacing) / 2;
            }
            Vector2 positionWithOffset = { 0 };
            positionWithOffset.x = position.x;
            positionWithOffset.y = position.y + yOffset;
            rDrawRayTeX(font, element, positionWithOffset, fontSize, color);
            position.x += size.x;
        }
    }
        break;

    case TEXMODE_VERTICAL:
    {
        Vector2 verticalSize = rMeasureRayTeX(font, tex, fontSize);
        for (int i = 0; i < tex->vertical.elementCount; ++i)
        {
            const RayTeX *element = tex->vertical.content[i].ptr;
            const Vector2 size = rMeasureRayTeX(font, element, fontSize);
            float xOffset = (verticalSize.x - size.x) / 2;
            Vector2 positionWithOffset = { 0 };
            positionWithOffset.x = position.x + xOffset;
            positionWithOffset.y = position.y;
            rDrawRayTeX(font, element, positionWithOffset, fontSize, color);
            position.y += size.y;
        }
    }
        break;

    case TEXMODE_MATRIX: // todo
        TRACELOG(LOG_WARNING, "RAYTEX: TEXMODE_MATRIX draw not yet implemented");
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex->mode);
    }
}

void DrawRayTeX(RayTeX tex, int x, int y, int fontSize, Color color)
{
    DrawRayTeXEx(GetFontDefault(), tex, x, y, fontSize, color);
}

void DrawRayTeXEx(Font font, RayTeX tex, int x, int y, int fontSize, Color color)
{
    Vector2 position = { 0 };
    position.x = (float)x;
    position.y = (float)y;
    rDrawRayTeX(&font, &tex, position, (float)fontSize, color);
}

static void rDrawRayTeXCentered(const Font *font, const RayTeX *tex, Rectangle rec, float fontSize, Color color)
{
    Vector2 texSize = rMeasureRayTeX(font, tex, fontSize);
    Vector2 position = { 0 };
    position.x = rec.x + (rec.width - texSize.x) / 2.0f;
    position.y = rec.y + (rec.height - texSize.y) / 2.0f;
    rDrawRayTeX(font, tex, position, fontSize, color);
}

void DrawRayTeXCentered(RayTeX tex, int x, int y, int width, int height, int fontSize, Color color)
{
    Rectangle rec = { 0 };
    rec.x = (float)x;
    rec.y = (float)y;
    rec.width = (float)width;
    rec.height = (float)height;
    DrawRayTeXCenteredRec(tex, rec, fontSize, color);
}

void DrawRayTeXCenteredRec(RayTeX tex, Rectangle rec, int fontSize, Color color)
{
    DrawRayTeXCenteredPro(GetFontDefault(), tex, rec, (float)fontSize, color);
}

void DrawRayTeXCenteredPro(Font font, RayTeX tex, Rectangle rec, float fontSize, Color color)
{
    rDrawRayTeXCentered(&font, &tex, rec, fontSize, color);
}

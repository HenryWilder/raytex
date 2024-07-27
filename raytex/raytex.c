#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#include "raytex.h"

#define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

enum {
    TEXFRAC_THICKNESS = 1,
};

RayTexSymbol RayTeXSymbolFromName(const char *name)
{
    // todo: Implement this as a lookup table once there are a lot of symbols
    if (strcmp(name, "neq") == 0) return TEXSYMBOL_NEQ;

    TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \"%s\"", name);
}

int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize)
{
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
        return MeasureText("=", fontSize);

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
}

int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize)
{
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
        return fontSize;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
}

void DrawRayTeXSymbol(RayTexSymbol symbol, int x, int y, int fontSize, Color color)
{
    int width = MeasureRayTeXSymbolWidth(symbol, fontSize);
    int height = MeasureRayTeXSymbolHeight(symbol, fontSize);
    int fontBaseSize = GetFontDefault().baseSize;
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
        DrawText("=", x, y, fontSize, color);
        Vector2 crossBottomLeft = { (float)x, (float)(y + height) };
        Vector2 crossTopRight = { (float)(x + width), (float)y };
        DrawLineEx(crossBottomLeft, crossTopRight, (float)(fontSize / fontBaseSize), color);
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
}

int MeasureRayTeXWidth(RayTeX tex)
{
    switch (tex.mode)
    {
    case TEXMODE_TEXT:
        return MeasureText(tex.text.content, tex.text.fontSize);

    case TEXMODE_SYMBOL:
        return MeasureRayTeXSymbolWidth(tex.symbol.content, tex.symbol.fontSize);

    case TEXMODE_FRAC:
    {
        int numeratorWidth = MeasureRayTeXWidth(tex.frac.content[TEX_FRAC_NUMERATOR]);
        int denominatorWidth = MeasureRayTeXWidth(tex.frac.content[TEX_FRAC_DENOMINATOR]);
        return (numeratorWidth > denominatorWidth ? numeratorWidth : denominatorWidth) + tex.frac.spacing * 2;
    }

    case TEXMODE_HORIZONTAL:
    {
        int totalWidth = 0;
    }

    case TEXMODE_VERTICAL:
    {
        int maxWidth = 0;
    }

    case TEXMODE_MATRIX:
    {
        // todo: matrix width
        TRACELOG(LOG_WARNING, "RAYTEX: Matrix width not implemented");
        return;
    }

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

int MeasureRayTeXHeight(RayTeX tex)
{
    switch (tex.mode)
    {
    case TEXMODE_TEXT:
        return tex.text.fontSize;

    case TEXMODE_SYMBOL:
        return MeasureRayTeXSymbolHeight(tex.symbol.content, tex.symbol.fontSize);

    case TEXMODE_FRAC:
    {
        int numeratorHeight = MeasureRayTeXHeight(tex.frac.content[TEX_FRAC_NUMERATOR]);
        int denominatorHeight = MeasureRayTeXHeight(tex.frac.content[TEX_FRAC_DENOMINATOR]);
        return numeratorHeight + denominatorHeight + tex.frac.spacing * 2 + TEXFRAC_THICKNESS;
    }

    case TEXMODE_HORIZONTAL:
    case TEXMODE_VERTICAL:
    case TEXMODE_MATRIX:
    {
        // todo: matrix height
        TRACELOG(LOG_WARNING, "RAYTEX: Matrix height not implemented");
        return;
    }

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

RayTeX GenRayTeXBlank()
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_TEXT;
    element.text.color = BLACK;
    element.text.fontSize = 0;
    element.text.content = "";
    TRACELOG(LOG_INFO, "RAYTEX: TeX blank element generated successfully");
    return element;
}

RayTeX GenRayTeXText(const char *content, int fontSize, Color color)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_TEXT;
    element.text.color = color;
    element.text.fontSize = fontSize;
    element.text.content = content;
    TRACELOG(LOG_INFO, "RAYTEX: TeX text element generated successfully");
    return element;
}

RayTeX GenRayTeXSymbol(RayTexSymbol symbol, int fontSize, Color color)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_SYMBOL;
    element.symbol.color = color;
    element.symbol.fontSize = fontSize;
    element.symbol.content = symbol;
    TRACELOG(LOG_INFO, "RAYTEX: TeX symbol element generated successfully");
    return element;
}

// WARNING: Shallow copies of the numerator and denominator are created.
// Unloading them outside of the fraction will also unload them for the fraction,
// and unloading the fraction will also unload them outside of the fraction.
RayTeX GenRayTeXFrac(RayTeX numerator, RayTeX denominator, int spacing, Color color)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_FRAC;
    element.frac.color = color;
    element.frac.spacing = spacing;
    element.frac.content = RL_MALLOC(sizeof(RayTeX) * 2);
    if (element.frac.content != NULL)
    {
        element.frac.content[TEX_FRAC_NUMERATOR] = numerator;
        element.frac.content[TEX_FRAC_DENOMINATOR] = denominator;
        TRACELOG(LOG_INFO, "RAYTEX: TeX fraction element generated successfully");
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXFraction() failed to allocate");
    return element;
}

RayTeX GenRayTeXHorizontal(int count, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_HORIZONTAL;
    element.horizontal.elementCount = count;
    element.horizontal.content = RL_MALLOC(count * sizeof(RayTeX));
    if (element.horizontal.content != NULL)
    {
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; ++i)
        {
            element.horizontal.content[i] = va_arg(args, RayTeX);
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX horizontal element generated successfully");
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXHorizontal() failed to allocate");
    return element;
}

RayTeX GenRayTeXVertical(int count, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_VERTICAL;
    element.vertical.elementCount = count;
    element.vertical.content = RL_MALLOC(count * sizeof(RayTeX));
    if (element.vertical.content != NULL)
    {
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; ++i)
        {
            element.vertical.content[i] = va_arg(args, RayTeX);
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical element generated successfully");
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXVertical() failed to allocate");
    return element;
}

RayTeX GenRayTeXMatrix(int rowCount, int columnCount, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_MATRIX;
    element.matrix.rowCount = rowCount;
    element.matrix.columnCount = columnCount;
    int elementCount = rowCount * columnCount;
    element.matrix.content = RL_MALLOC(elementCount * sizeof(RayTeX));
    if (element.matrix.content != NULL)
    {
        va_list args;
        va_start(args, elementCount);
        for (int i = 0; i < elementCount; ++i)
        {
            element.matrix.content[i] = va_arg(args, RayTeX);
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX matrix element generated successfully");
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXMatrix() failed to allocate");
    return element;
}

typedef struct RayTeXPathBuilder {
    TeXPathPartKind kind;
    union {
        // End of path
        TeXField field;

        struct {
            union {
                // Horizontal/Vertical
                int index;

                // Matrix
                struct {
                    int rowIndex;
                    int columnIndex;
                };
            };
            struct RayTeXPathBuilder *next;
        };
    };
} RayTeXPathBuilder;

static RayTeXPathBuilder *_GetRayTeXElementFieldLocation(TeXField field)
{
    RayTeXPathBuilder *path = RL_MALLOC(sizeof(RayTeXPathBuilder));
    if (path != NULL)
    {
        path->kind = TEXPATHPARTKIND_FIELD;
        path->field = field;
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: _GetRayTeXElementFieldLocation() failed to allocate");
    return path;
}

RayTeXPathBuilder *GetRayTeXElementFieldLocation(RayTeX tex, TeXField field)
{
    switch (field)
    {
    case TEXFIELD_TEXT_FONTSIZE:
    case TEXFIELD_TEXT_COLOR:
    case TEXFIELD_TEXT_CONTENT:
        if (tex.mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXMEMBER_TEXT_... only valid for TEXMODE_TEXT");
        break;

    case TEXFIELD_SYMBOL_FONTSIZE:
    case TEXFIELD_SYMBOL_COLOR:
    case TEXFIELD_SYMBOL_CONTENT:
        if (tex.mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXMEMBER_SYMBOL_... only valid for TEXMODE_SYMBOL");
        break;

    case TEXFIELD_FRAC_SPACING:
    case TEXFIELD_FRAC_COLOR:
        if (tex.mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXMEMBER_FRAC_... only valid for TEXMODE_FRAC");
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown field [%i]", field);
    }

    return _GetRayTeXElementFieldLocation(field);
}

RayTeXPathBuilder *GetRayTeXFieldLocation(RayTeXPathBuilder *path, TeXField field)
{
    return _GetRayTeXElementFieldLocation(field);
}

RayTeXPathBuilder *GetRayTeXSubLocation(RayTeX tex, int index, RayTeXPathBuilder *subPath)
{
    RayTeXPathBuilder *path = RL_MALLOC(sizeof(RayTeXPathBuilder));
    if (path != NULL)
    {
        path->kind = TEXPATHPARTKIND_CHILD;
        path->index = index;
        path->next = subPath;
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GetRayTeXSubLocation() failed to allocate");
    return path;
}

RayTeXPathBuilder *GetRayTeXMatrixSubLocation(RayTeX tex, int rowIndex, int columnIndex, RayTeXPathBuilder *subPath)
{
    RayTeXPathBuilder *path = RL_MALLOC(sizeof(RayTeXPathBuilder));
    if (path != NULL)
    {
        path->kind = TEXPATHPARTKIND_CELL;
        path->rowIndex = rowIndex;
        path->columnIndex = columnIndex;
        path->next = subPath;
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GetRayTeXMatrixSubLocation() failed to allocate");
    return path;
}

void UnloadRayTeXPathBuilder(RayTeXPathBuilder *fromBuilder, RayTeXPathBuilder *untilBuilder)
{
    int unloadedCount = 0;
    RayTeXPathBuilder *current = fromBuilder;
    while (current != untilBuilder)
    {
        if (unloadedCount > 256 &&
            unloadedCount % 64 == 0 /* don't be annoying */)
        {
            TRACELOG(LOG_WARNING,
                     "RAYTEX: I've unloaded %i RayTeXPathBuilders starting at fromBuilder [%p] while looking for untilBuilder [%p], "
                     "are you sure untilBuilder comes after fromBuilder in fromBuilder's path?",
                     unloadedCount, fromBuilder, untilBuilder);
        }
        RayTeXPathBuilder *next = fromBuilder;
        RL_FREE(fromBuilder);
        fromBuilder = next;
        ++unloadedCount;
    }
}

void UnloadRayTeXPathBuilderCompletely(RayTeXPathBuilder *builder)
{
    UnloadRayTeXPathBuilder(builder, NULL);
}

RayTeXPath GenRayTeXPath(RayTeXPathBuilder *builder)
{
    int partCount = 0;
    const RayTeXPathBuilder *current = builder;
    while (current->kind != TEXPATHPARTKIND_FIELD)
    {
        ++partCount;
        current = current->next;
    }
    RayTeXPath path = { 0 };
    path.parts = RL_MALLOC(partCount * sizeof(RayTeXPathPart));
    current = &builder;
    if (path.parts != NULL)
    {
        for (int i = 0; i < partCount; ++i)
        {
            RayTeXPathPart part = { 0 };
            part.kind = current->kind;
            switch (part.kind)
            {
            case TEXPATHPARTKIND_FIELD:
                part.field = current->field;
                break;
            case TEXPATHPARTKIND_CHILD:
                part.index = current->index;
                break;
            case TEXPATHPARTKIND_CELL:
                part.rowIndex = current->rowIndex;
                part.columnIndex = current->columnIndex;
                break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown path part kind [%i]", part.kind);
            }
            path.parts[i] = part;

            current = current->next;
        }
        TRACELOG(LOG_INFO, "RAYTEX: RayTeX path generated successfully");
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXPath() failed to allocate");
    return path;
}

void UnloadRayTeXPath(RayTeXPath path)
{
    RL_FREE(path.parts);
    TRACELOG(LOG_INFO, "RAYTEX: RayTeX path unloaded successfully");
}

void SetRayTeXValue(RayTeX tex, RayTeXPath path, void *value)
{
    RayTeXPathPart *part;
    RayTeX *target = &tex;
    for (part = path.parts; part->kind != TEXPATHPARTKIND_FIELD; ++part)
    {
        TeXMode mode = target->mode;
        switch (part->kind)
        {
        case TEXPATHPARTKIND_FIELD:
            __assume(false); // Should not be reachable due to for-loop condition
            break;

        case TEXPATHPARTKIND_CHILD:
            switch (mode)
            {
            case TEXMODE_TEXT:
                TRACELOG(LOG_ERROR, "RAYTEX: TEXMODE_TEXT has no children and cannot be indexed");
                break;

            case TEXMODE_SYMBOL:
                TRACELOG(LOG_ERROR, "RAYTEX: TEXMODE_SYMBOL has no children and cannot be indexed");
                break;

            case TEXMODE_MATRIX:
                TRACELOG(LOG_ERROR, "RAYTEX: TEXMODE_MATRIX cannot be indexed with TEXPATHPARTKIND_CHILD, use TEXPATHPARTKIND_CELL instead");
                break;

            case TEXMODE_FRAC:
            {
                int nextIndex = part->index;
                if (nextIndex < 0)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: path index (%i) cannot be negative", nextIndex);
                    // nextIndex = 0;
                }
                if (nextIndex > TEX_FRAC_DENOMINATOR)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: TEXMODE_FRAC index (%i) cannot be higher than TEX_FRAC_DENOMINATOR (%i)", nextIndex, TEX_FRAC_DENOMINATOR);
                    // nextIndex = TEX_FRAC_DENOMINATOR;
                }
                target = &(target->frac.content[nextIndex]);
            }
                break;

            case TEXMODE_HORIZONTAL:
            {
                int nextIndex = part->index;
                if (nextIndex < 0)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: path index (%i) cannot be negative", nextIndex);
                    // nextIndex = 0;
                }
                int backIndex = target->horizontal.elementCount - 1;
                if (nextIndex > backIndex)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: path index (%i) cannot be higher than the index of the last element (%i)", nextIndex, backIndex);
                    // nextIndex = backIndex;
                }
                target = &(target->horizontal.content[nextIndex]);
            }
                break;

            case TEXMODE_VERTICAL:
            {
                int nextIndex = part->index;
                if (nextIndex < 0)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: path index (%i) cannot be negative", nextIndex);
                    // nextIndex = 0;
                }
                int backIndex = target->vertical.elementCount - 1;
                if (nextIndex > backIndex)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: path index (%i) cannot be higher than the index of the last element (%i)", nextIndex, backIndex);
                    // nextIndex = backIndex;
                }
                target = &(target->vertical.content[nextIndex]);
            }
                break;

            default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown tex mode [%i]", mode);
            }
            break;

        case TEXPATHPARTKIND_CELL:
        {
            if (mode != TEXMODE_MATRIX) TRACELOG(LOG_WARNING, "RAYTEX: TEXPATHPARTKIND_CELL can only be used to index TEXMODE_MATRIX", mode);

            int rowIndex = part->rowIndex;
            if (rowIndex < 0)
            {
                TRACELOG(LOG_WARNING, "RAYTEX: row index (%i) cannot be negative", rowIndex);
                // rowIndex = 0;
            }
            int lastRowIndex = target->matrix.rowCount - 1;
            if (rowIndex > lastRowIndex)
            {
                TRACELOG(LOG_WARNING, "RAYTEX: row index (%i) cannot be higher than the index of the last row (%i)", rowIndex, lastRowIndex);
                // rowIndex = lastRowIndex;
            }
                
            int columnIndex = part->columnIndex;
            if (columnIndex < 0)
            {
                TRACELOG(LOG_WARNING, "RAYTEX: column index (%i) cannot be negative", columnIndex);
                // columnIndex = 0;
            }
            int columnCount = target->matrix.columnCount;
            int lastColumnIndex = columnCount - 1;
            if (columnIndex > lastColumnIndex)
            {
                TRACELOG(LOG_WARNING, "RAYTEX: column index (%i) cannot be higher than the index of the last column (%i)", columnIndex, lastColumnIndex);
                // columnIndex = lastColumnIndex;
            }

            int index = rowIndex * columnCount + columnIndex;
            target = &(target->matrix.content[index]);
        }
            break;

        default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown path part kind [%i]", part->kind);
        }
    }

    switch (part->field)
    {
    case TEXFIELD_TEXT_FONTSIZE:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_TEXT_FONTSIZE only valid for TEXMODE_TEXT");
        target->text.fontSize = *(int*)(value);
        break;
    case TEXFIELD_TEXT_COLOR:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_TEXT_COLOR only valid for TEXMODE_TEXT");
        target->text.color = *(Color*)(value);
        break;
    case TEXFIELD_TEXT_CONTENT:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_TEXT_CONTENT only valid for TEXMODE_TEXT");
        target->text.content = *(const char**)(value);
        break;

    case TEXFIELD_SYMBOL_FONTSIZE:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_SYMBOL_FONTSIZE only valid for TEXMODE_SYMBOL");
        target->symbol.fontSize = *(int*)(value);
        break;
    case TEXFIELD_SYMBOL_COLOR:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_SYMBOL_COLOR only valid for TEXMODE_SYMBOL");
        target->symbol.color = *(Color*)(value);
        break;
    case TEXFIELD_SYMBOL_CONTENT:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_SYMBOL_CONTENT only valid for TEXMODE_SYMBOL");
        target->symbol.content = *(RayTexSymbol*)(value);
        break;

    case TEXFIELD_FRAC_SPACING:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_FRAC_SPACING only valid for TEXMODE_FRAC");
        target->frac.spacing = *(int*)(value);
        break;
    case TEXFIELD_FRAC_COLOR:
        if (target->mode != TEXMODE_TEXT) TRACELOG(LOG_WARNING, "RAYTEX: field TEXFIELD_FRAC_COLOR only valid for TEXMODE_FRAC");
        target->frac.color = *(Color*)(value);
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown TeX field [%i]", part->field);
    }
}

void UnloadRayTeX(RayTeX tex)
{
    switch (tex.mode)
    {
    case TEXMODE_TEXT:
        TRACELOG(LOG_INFO, "RAYTEX: TeX text element unloaded successfully");
        break;

    case TEXMODE_SYMBOL:
        TRACELOG(LOG_INFO, "RAYTEX: TeX symbol element unloaded successfully");
        break;

    case TEXMODE_FRAC:
        UnloadRayTeX(tex.frac.content[TEX_FRAC_NUMERATOR]);
        UnloadRayTeX(tex.frac.content[TEX_FRAC_DENOMINATOR]);
        RL_FREE(tex.frac.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX fraction element unloaded successfully");
        break;

    case TEXMODE_HORIZONTAL:
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            UnloadRayTeX(tex.horizontal.content[i]);
        }
        RL_FREE(tex.horizontal.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX horizontal element unloaded successfully");
        break;

    case TEXMODE_VERTICAL:
        for (int i = 0; i < tex.vertical.elementCount; ++i)
        {
            UnloadRayTeX(tex.vertical.content[i]);
        }
        RL_FREE(tex.vertical.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical element unloaded successfully");
        break;

    case TEXMODE_MATRIX:
        for (int i = 0; i < tex.matrix.rowCount * tex.matrix.columnCount; ++i)
        {
            UnloadRayTeX(tex.matrix.content[i]);
        }
        RL_FREE(tex.matrix.content);
        TRACELOG(LOG_INFO, "RAYTEX: TeX matrix element unloaded successfully");
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Failed to unload TeX element with unknown mode [%i]", tex.mode);
    }
}

void DrawRayTeX(RayTeX tex, int x, int y)
{
    switch (tex.mode)
    {
    case TEXMODE_TEXT:
        DrawText(tex.text.content, x, y, tex.text.fontSize, tex.text.color);
        break;

    case TEXMODE_SYMBOL:
        DrawRayTeXSymbol(tex.symbol.content, x, y, tex.symbol.fontSize, tex.symbol.color);
        break;

    case TEXMODE_FRAC:
    {
        RayTeX numerator = tex.frac.content[TEX_FRAC_NUMERATOR];
        RayTeX denominator = tex.frac.content[TEX_FRAC_DENOMINATOR];
        int numeratorHeight = MeasureRayTeXHeight(numerator);

        int width = MeasureRayTeXWidth(tex);

        int numeratorWidth = MeasureRayTeXWidth(numerator);
        int numeratorPaddingLeft = (width - numeratorWidth) / 2;

        int denominatorWidth = MeasureRayTeXWidth(denominator);
        int denominatorPaddingLeft = (width - denominatorWidth) / 2;

        DrawRayTeX(
            numerator,
            x + numeratorPaddingLeft,
            y);

        DrawRectangle(
            x,
            y + numeratorHeight + tex.frac.spacing,
            width,
            TEXFRAC_THICKNESS,
            tex.frac.color);

        DrawRayTeX(
            denominator,
            x + denominatorPaddingLeft,
            y + numeratorHeight + tex.frac.spacing * 2 + TEXFRAC_THICKNESS);
    }
        break;

    case TEXMODE_HORIZONTAL: // todo
    case TEXMODE_VERTICAL: // todo
    case TEXMODE_MATRIX: // todo

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

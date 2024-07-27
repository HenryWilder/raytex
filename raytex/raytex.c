#include <string.h>
#include <stdlib.h>
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
        return MU_TO_PIXELS(RELSPACE_SIZE*2, fontSize) + MeasureText("=", fontSize);

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
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
    {
        int space = MU_TO_PIXELS(RELSPACE_SIZE, fontSize);
        DrawText("=", x + space, y, fontSize, color);
        Vector2 crossBottomLeft = { (float)(x + space), (float)(y + height) };
        Vector2 crossTopRight = { (float)(x + width - space), (float)y };
        DrawLineV(crossBottomLeft, crossTopRight, color);
    }
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol [%i]", symbol);
    }
}

int MeasureRayTeXWidthEx(Font font, RayTeX tex, int fontSize)
{
    if (tex.isOverridingFontSize) fontSize = tex.overrideFontSize;
    if (tex.overrideFont != NULL) font = *tex.overrideFont;
    switch (tex.mode)
    {
    case TEXMODE_SPACE:
        return MU_TO_PIXELS(tex.space.size, fontSize);

    case TEXMODE_VSPACE:
        return 0;

    case TEXMODE_TEXT:
        return MeasureText(tex.text.content, fontSize);

    case TEXMODE_SYMBOL:
        return MeasureRayTeXSymbolWidth(tex.symbol.content, fontSize);

    case TEXMODE_FRAC:
    {
        int numeratorWidth = MeasureRayTeXWidth(tex.frac.content[TEX_FRAC_NUMERATOR], fontSize);
        int denominatorWidth = MeasureRayTeXWidth(tex.frac.content[TEX_FRAC_DENOMINATOR], fontSize);
        return (numeratorWidth > denominatorWidth ? numeratorWidth : denominatorWidth) + tex.frac.spacing * 2;
    }

    case TEXMODE_HORIZONTAL:
    {
        int totalWidth = 0;
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            totalWidth += MeasureRayTeXWidth(tex.horizontal.content[i], fontSize);
        }
        return totalWidth;
    }

    case TEXMODE_VERTICAL:
    {
        int maxWidth = 0;
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            int width = MeasureRayTeXWidth(tex.horizontal.content[i], fontSize);
            if (width > maxWidth) maxWidth = width;
        }
        return maxWidth;
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

int MeasureRayTeXHeightEx(Font font, RayTeX tex, int fontSize)
{
    if (tex.isOverridingFontSize) fontSize = tex.overrideFontSize;
    if (tex.overrideFont != NULL) font = *tex.overrideFont;
    switch (tex.mode)
    {
    case TEXMODE_SPACE:
        return 0;

    case TEXMODE_VSPACE:
        return MU_TO_PIXELS(tex.space.size, fontSize);

    case TEXMODE_TEXT:
        return fontSize;

    case TEXMODE_SYMBOL:
        return MeasureRayTeXSymbolHeight(tex.symbol.content, fontSize);

    case TEXMODE_FRAC:
    {
        int numeratorHeight = MeasureRayTeXHeightEx(font, tex.frac.content[TEX_FRAC_NUMERATOR], fontSize);
        int denominatorHeight = MeasureRayTeXHeightEx(font, tex.frac.content[TEX_FRAC_DENOMINATOR], fontSize);
        return numeratorHeight + denominatorHeight + tex.frac.spacing * 2 + TEXFRAC_THICKNESS;
    }

    case TEXMODE_HORIZONTAL:
    {
        int maxHeight = 0;
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            int height = MeasureRayTeXHeightEx(font, tex.horizontal.content[i], fontSize);
            if (height > maxHeight) maxHeight = height;
        }
        return maxHeight;
    }

    case TEXMODE_VERTICAL:
    {
        int totalHeight = 0;
        for (int i = 0; i < tex.horizontal.elementCount; ++i)
        {
            totalHeight += MeasureRayTeXHeightEx(font, tex.horizontal.content[i], fontSize);
        }
        return totalHeight;
    }

    case TEXMODE_MATRIX:
    {
        // todo: matrix height
        TRACELOG(LOG_WARNING, "RAYTEX: Matrix height not implemented");
        return;
    }

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

int MeasureRayTeXWidth(RayTeX tex, int fontSize)
{
    return MeasureRayTeXWidthEx(GetFontDefault(), tex, fontSize);
}

int MeasureRayTeXHeight(RayTeX tex, int fontSize)
{
    return MeasureRayTeXHeightEx(GetFontDefault(), tex, fontSize);
}

RayTeX SetRayTeXColor(RayTeX tex, Color color)
{
    tex.isOverridingColor = true;
    tex.overrideColor = color;
    return tex;
}

RayTeX SetRayTeXFontSize(RayTeX tex, int fontSize)
{
    tex.isOverridingFontSize = true;
    tex.overrideFontSize = fontSize;
    return tex;
}

RayTeX SetRayTeXFont(RayTeX tex, Font font)
{
    tex.overrideFont = RL_MALLOC(sizeof(Font));
    if (tex.overrideFont) *tex.overrideFont = font;
    else TRACELOG(LOG_ERROR, "RAYTEX: SetRayTeXFont() failed to allocate");
    return tex;
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

RayTeX GenRayTeXSymbol(RayTexSymbol symbol)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_SYMBOL;
    element.symbol.content = symbol;
    TRACELOG(LOG_INFO, "RAYTEX: TeX symbol element generated successfully");
    return element;
}

// WARNING: Shallow copies of the numerator and denominator are created.
// Unloading them outside of the fraction will also unload them for the fraction,
// and unloading the fraction will also unload them outside of the fraction.
RayTeX GenRayTeXFrac(RayTeX numerator, RayTeX denominator, int spacing)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_FRAC;
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

// WARNING: Shallow copies of the elements are created.
// Unloading them outside of the horizontal will also unload them for the horizontal,
// and unloading the horizontal will also unload them outside of the horizontal.
RayTeX GenRayTeXHorizontal(TeXAlign alignContent, int count, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_HORIZONTAL;
    element.horizontal.alignContent = alignContent;
    element.horizontal.elementCount = count;
    element.horizontal.content = RL_MALLOC(count * sizeof(RayTeX));
    if (element.horizontal.content != NULL)
    {
        int argC = 0;
        va_list args;
        va_start(args, count);
        for (int i = 0; i < count; ++i)
        {
            element.horizontal.content[i] = va_arg(args, RayTeX);
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
RayTeX GenRayTeXVertical(TeXAlign alignContent, int count, ...)
{
    RayTeX element = { 0 };
    element.mode = TEXMODE_VERTICAL;
    element.horizontal.alignContent = alignContent;
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
        TRACELOG(LOG_INFO, "RAYTEX: TeX vertical with %i elements generated successfully", count);
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXVertical() failed to allocate");
    return element;
}

// WARNING: Shallow copies of the elements are created.
// Unloading them outside of the matrix will also unload them for the matrix,
// and unloading the matrix will also unload them outside of the matrix.
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
        va_start(args, columnCount);
        for (int i = 0; i < elementCount; ++i)
        {
            element.matrix.content[i] = va_arg(args, RayTeX);
        }
        va_end(args);
        TRACELOG(LOG_INFO, "RAYTEX: TeX matrix with %i elements (%i rows x %i columns) generated successfully", elementCount, rowCount, columnCount);
    }
    else TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXMatrix() failed to allocate");
    return element;
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

void DrawRayTeXEx(Font font, RayTeX tex, int x, int y, int fontSize, Color color)
{
    if (tex.isOverridingColor) color = tex.overrideColor;
    if (tex.isOverridingFontSize) fontSize = tex.overrideFontSize;
    if (tex.overrideFont != NULL) font = *tex.overrideFont;

    switch (tex.mode)
    {
    case TEXMODE_TEXT:
        DrawText(tex.text.content, x, y, fontSize, color);
        break;

    case TEXMODE_SYMBOL:
        DrawRayTeXSymbol(tex.symbol.content, x, y, fontSize, color);
        break;

    case TEXMODE_FRAC:
    {
        RayTeX numerator = tex.frac.content[TEX_FRAC_NUMERATOR];
        RayTeX denominator = tex.frac.content[TEX_FRAC_DENOMINATOR];
        int numeratorHeight = MeasureRayTeXHeight(numerator, fontSize);

        int width = MeasureRayTeXWidthEx(font, tex, fontSize);

        int numeratorWidth = MeasureRayTeXWidthEx(font, numerator, fontSize);
        int numeratorPaddingLeft = (width - numeratorWidth) / 2;

        int denominatorWidth = MeasureRayTeXWidthEx(font, denominator, fontSize);
        int denominatorPaddingLeft = (width - denominatorWidth) / 2;

        DrawRayTeXEx(
            font,
            numerator,
            x + numeratorPaddingLeft,
            y,
            fontSize, color);

        DrawRectangle(
            x,
            y + numeratorHeight + tex.frac.spacing,
            width,
            TEXFRAC_THICKNESS,
            color);

        DrawRayTeXEx(
            font,
            denominator,
            x + denominatorPaddingLeft,
            y + numeratorHeight + tex.frac.spacing * 2 + TEXFRAC_THICKNESS,
            fontSize, color);
    }
        break;

    case TEXMODE_HORIZONTAL:
    {
        const TeXAlign alignContent = tex.horizontal.alignContent;
        if (alignContent == HORIZONTAL_TEXALIGN_TOP)
        {
            for (int i = 0; i < tex.horizontal.elementCount; ++i)
            {
                const RayTeX element = tex.horizontal.content[i];
                const int width = MeasureRayTeXWidthEx(font, element, fontSize);
                DrawRayTeXEx(font, element, x, y, fontSize, color);
                x += width;
            }
        }
        else
        {
            int horizontalHeight = MeasureRayTeXHeightEx(font, tex, fontSize);
            for (int i = 0; i < tex.horizontal.elementCount; ++i)
            {
                RayTeX element = tex.horizontal.content[i];
                int width = MeasureRayTeXWidthEx(font, element, fontSize);
                int height = MeasureRayTeXHeightEx(font, element, fontSize);
                int yOffset;
                switch (alignContent)
                {
                case HORIZONTAL_TEXALIGN_CENTER:
                    yOffset = (horizontalHeight - height) / 2;
                    break;
                case HORIZONTAL_TEXALIGN_BOTTOM:
                    yOffset = horizontalHeight - height;
                    break;

                default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown align [%i]", alignContent);
                }
                DrawRayTeXEx(font, element, x, y + yOffset, fontSize, color);
                x += width;
            }
        }
    }
        break;

    case TEXMODE_VERTICAL: // todo
    {
        const TeXAlign alignContent = tex.vertical.alignContent;
        if (alignContent == VERTICAL_TEXALIGN_LEFT)
        {
            for (int i = 0; i < tex.vertical.elementCount; ++i)
            {
                const RayTeX element = tex.vertical.content[i];
                const int width = MeasureRayTeXWidthEx(font, element, fontSize);
                DrawRayTeXEx(font, element, x, y, fontSize, color);
                x += width;
            }
        }
        else
        {
            int verticalWidth = MeasureRayTeXWidthEx(font, tex, fontSize);
            for (int i = 0; i < tex.vertical.elementCount; ++i)
            {
                RayTeX element = tex.vertical.content[i];
                int width = MeasureRayTeXWidthEx(font, element, fontSize);
                int height = MeasureRayTeXHeightEx(font, element, fontSize);
                int xOffset;
                switch (alignContent)
                {
                case VERTICAL_TEXALIGN_CENTER:
                    xOffset = (verticalWidth - width) / 2;
                    break;
                case VERTICAL_TEXALIGN_RIGHT:
                    xOffset = verticalWidth - width;
                    break;

                default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown align [%i]", alignContent);
                }
                DrawRayTeXEx(font, element, x + xOffset, y, fontSize, color);
                y += height;
            }
        }
    }
        break;

    case TEXMODE_MATRIX: // todo
        TRACELOG(LOG_WARNING, "RAYTEX: TEXMODE_MATRIX draw not yet implemented");
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

void DrawRayTeXCenteredEx(Font font, RayTeX tex, int x, int y, int width, int height, int fontSize, Color color)
{
    int texWidth  = MeasureRayTeXWidth (tex, fontSize);
    int texHeight = MeasureRayTeXHeight(tex, fontSize);
    DrawRayTeXEx(font, tex, (width - texWidth) / 2, (height - texHeight) / 2, fontSize, color);
}

void DrawRayTeXCenteredRecEx(Font font, RayTeX tex, Rectangle rec, int fontSize, Color color)
{
    DrawRayTeXCenteredEx(font, tex, (int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, fontSize, color);
}

void DrawRayTeX(RayTeX tex, int x, int y, int fontSize, Color color)
{
    DrawRayTeXEx(GetFontDefault(), tex, x, y, fontSize, color);
}

void DrawRayTeXCentered(RayTeX tex, int x, int y, int width, int height, int fontSize, Color color)
{
    DrawRayTeXCenteredEx(GetFontDefault(), tex, x, y, width, height, fontSize, color);
}

void DrawRayTeXCenteredRec(RayTeX tex, Rectangle rec, int fontSize, Color color)
{
    DrawRayTeXCenteredRecEx(GetFontDefault(), tex, rec, fontSize, color);
}

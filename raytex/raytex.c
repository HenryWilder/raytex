#include <string.h>
#include <stdlib.h>
#include <raylib.h>
#include "raytex.h"

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
    case TEXMODE_MODE_TEXT:
        return MeasureText(tex.text, tex.fontSize);

    case TEXMODE_MODE_SYMBOL:
        return MeasureRayTeXSymbolWidth(tex.symbol, tex.fontSize);

    case TEXMODE_MODE_FRAC:
    {
        int numeratorWidth = MeasureRayTeXWidth(tex.frac[TEXFRAC_NUMERATOR]);
        int denominatorWidth = MeasureRayTeXWidth(tex.frac[TEXFRAC_DENOMINATOR]);
        return (numeratorWidth > denominatorWidth ? numeratorWidth : denominatorWidth) + tex.spacing * 2;
    }

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

int MeasureRayTeXHeight(RayTeX tex)
{
    switch (tex.mode)
    {
    case TEXMODE_MODE_TEXT:
        return tex.fontSize;

    case TEXMODE_MODE_SYMBOL:
        return MeasureRayTeXSymbolHeight(tex.symbol, tex.fontSize);

    case TEXMODE_MODE_FRAC:
    {
        int numeratorHeight = MeasureRayTeXHeight(tex.frac[TEXFRAC_NUMERATOR]);
        int denominatorHeight = MeasureRayTeXHeight(tex.frac[TEXFRAC_DENOMINATOR]);
        return numeratorHeight + denominatorHeight + tex.spacing * 2 + TEXFRAC_THICKNESS;
    }

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

RayTeX GenRayTeXText(const char *content, int fontSize, Color color)
{
    return CLITERAL(RayTeX)
    {
        .mode = TEXMODE_MODE_TEXT,
        .color = color,
        .fontSize = fontSize,
        .text = content,
    };
}

RayTeX GenRayTeXSymbol(RayTexSymbol symbol, int fontSize, Color color)
{
    return CLITERAL(RayTeX)
    {
        .mode = TEXMODE_MODE_SYMBOL,
        .color = color,
        .fontSize = fontSize,
        .symbol = symbol,
    };
}

// WARNING: Shallow copies of the numerator and denominator are created.
// Unloading them will also unload them in the fraction, and unloading the fraction will unload them.
RayTeX GenRayTeXFraction(RayTeX numerator, RayTeX denominator, int spacing, Color color)
{
    RayTeX *frac = RL_MALLOC(sizeof(RayTeX) * 2);
    if (frac == NULL)
    {
        TRACELOG(LOG_ERROR, "RAYTEX: GenRayTeXFraction() failed to allocate");
        return;
    }
    frac[TEXFRAC_NUMERATOR] = numerator;
    frac[TEXFRAC_DENOMINATOR] = denominator;
    return CLITERAL(RayTeX)
    {
        .mode = TEXMODE_MODE_FRAC,
        .color = color,
        .spacing = spacing,
        .frac = frac,
    };
}

void UnloadRayTeX(RayTeX tex)
{
    if (tex.mode == TEXMODE_MODE_FRAC)
    {
        UnloadRayTeX(tex.frac[TEXFRAC_NUMERATOR]);
        UnloadRayTeX(tex.frac[TEXFRAC_DENOMINATOR]);
        RL_FREE(tex.frac);
    }
}

void DrawRayTeX(RayTeX tex, int x, int y)
{
    switch (tex.mode)
    {
    case TEXMODE_MODE_TEXT:
        DrawText(tex.text, x, y, tex.fontSize, tex.color);
        break;

    case TEXMODE_MODE_SYMBOL:
        DrawRayTeXSymbol(tex.symbol, x, y, tex.fontSize, tex.color);
        break;

    case TEXMODE_MODE_FRAC:
    {
        RayTeX numerator = tex.frac[TEXFRAC_NUMERATOR];
        RayTeX denominator = tex.frac[TEXFRAC_DENOMINATOR];
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
            y + numeratorHeight + tex.spacing,
            width,
            TEXFRAC_THICKNESS,
            tex.color);

        DrawRayTeX(
            denominator,
            x + denominatorPaddingLeft,
            y + numeratorHeight + tex.spacing * 2 + TEXFRAC_THICKNESS);
    }
        break;

    default: TRACELOG(LOG_WARNING, "RAYTEX: Unknown mode [%i]", tex.mode);
    }
}

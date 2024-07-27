#include <string.h>
#include <raylib.h>
#include "raytex.h"

#define TEX_BUFFER_SIZE 4096

RayTexSymbol RayTeXSymbolFromName(const char *name)
{
    // todo: Implement this as a lookup table once there are a lot of symbols
    if (strcmp(name, "neq") == 0) return TEXSYMBOL_NEQ;

    TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \"%s\"", name);
    return -1;
}

int MeasureRayTeXSymbolWidth(RayTexSymbol symbol, int fontSize)
{
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
        return MeasureText("=", fontSize);

    default:
        TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \'%i\'", symbol);
        return MeasureText("?", fontSize) + 4;
    }
}

int MeasureRayTeXSymbolHeight(RayTexSymbol symbol, int fontSize)
{
    switch (symbol)
    {
    case TEXSYMBOL_NEQ:
        return fontSize;

    default:
        TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \'%i\'", symbol);
        return fontSize;
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
        Vector2 crossBottomLeft = { x, y + height };
        Vector2 crossTopRight = { x + width, y };
        DrawLineEx(crossBottomLeft, crossTopRight, (float)(fontSize / fontBaseSize), color);
        break;

    default:
        TRACELOG(LOG_WARNING, "RAYTEX: Unknown symbol \'%i\'", symbol);
        DrawRectangleLines(x, y, MeasureText("?", fontSize) + 4, fontSize, color);
        DrawText("?", x + 2, y, fontSize, color);
        break;
    }
}

int MeasureRayTeXWidth(RayTeX tex)
{
    return 0;
}

int MeasureRayTeXHeight(RayTeX tex)
{
    return 0;
}

RayTeX GenRayTeXText(const char *content, int mode, int fontSize, Color color)
{
    return CLITERAL(RayTeX)
    {
        .mode = TEXMODE_MODE_TEXT,
        .fontSize = fontSize,
        .color = color,
        .childCount = 0,
        .children = NULL,
        .content = content,
    };
}

void UnloadRayTeX(RayTeX tex)
{
    for (int i = 0; i < tex.childCount; ++i)
    {
        UnloadRayTeX(tex.children[i]);
    }
    RL_FREE(tex.children);
}

void AttachRayTeXChild(RayTeX *parent, RayTeX child, int index)
{
    if (index < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: AttachRayTeXChild() index (%i) cannot be negative", index);
        //index = 0;
    }
    else if (index > parent->childCount)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: AttachRayTeXChild() index (%i) cannot be higher than the index after the last child (%i)", index, parent->childCount);
        //index = parent->childCount;
    }

    int newChildCount = parent->childCount + 1;
    RL_REALLOC(parent->children, newChildCount);
    for (int i = newChildCount; i > index; --i)
    {
        parent->children[i] = parent->children[i - 1];
    }
    parent->childCount = newChildCount;
    parent->children[index] = child;
}

RayTeX DetachRayTeXChild(RayTeX *parent, int index)
{
    int lastChildIndex = parent->childCount - 1;
    if (index < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: DetachRayTeXChild() index (%i) cannot be negative", index);
        //index = 0;
    }
    else if (index > lastChildIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: DetachRayTeXChild() index (%i) cannot be higher than the index of the last child (%i)", index, lastChildIndex);
        //index = lastChildIndex;
    }

    RayTeX detachedChild = parent->children[index];
    
    int newChildCount = parent->childCount - 1;
    for (int i = index; i < newChildCount; ++i)
    {
        parent->children[i] = parent->children[i + 1];
    }
    parent->childCount = newChildCount;

    return detachedChild;
}

RayTeX *GetRayTeXChild(RayTeX parent, int index)
{
    int lastChildIndex = parent.childCount - 1;
    if (index < 0)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: GetRayTeXChild() index (%i) cannot be negative", index);
        //index = 0;
    }
    else if (index > lastChildIndex)
    {
        TRACELOG(LOG_WARNING, "RAYTEX: GetRayTeXChild() index (%i) cannot be higher than the index of the last child (%i)", index, lastChildIndex);
        //index = lastChildIndex;
    }

    return &(parent.children[index]);
}

void SetRayTeXChildMode(RayTeX parent, int index, int mode)
{
    parent.children[index].mode = mode;
}

void SetRayTeXChildFontSize(RayTeX parent, int index, int fontSize)
{
    parent.children[index].fontSize = fontSize;
}

void SetRayTeXChildColor(RayTeX parent, int index, Color color)
{
    parent.children[index].color = color;
}

void SetRayTeXChildContent(RayTeX parent, int index, const char *content)
{
    parent.children[index].content = content;
}

void DrawRayTeX(RayTeX tex, int x, int y)
{
    int xOffset = 0;
    int yOffset = 0;
    const char *content = tex.content;
    char buffer[TEX_BUFFER_SIZE] = "";
    bool isInSymbol = false;
    while (*content != '\0')
    {
        int iBuffer = 0;
        if (!isInSymbol)
        {
            for (; iBuffer < TEX_BUFFER_SIZE && *content != '\0'; ++content, ++iBuffer)
            {
                if (*content == '\\')
                {
                    if (*(content + 1) == '\0')
                    {
                        TRACELOG(LOG_WARNING, "RAYTEX: DrawRayTeX() content cannot end with \'\\\'");
                    }

                    if (*(content + 1) != '\\')
                    {
                        isInSymbol = true;
                        break;
                    }
                }
                buffer[iBuffer] = *content;
            }
            buffer[iBuffer] = '\0';
            DrawText(buffer, x + xOffset, y + yOffset, tex.fontSize, tex.color);
            xOffset += MeasureText(buffer, tex.fontSize);
        }
        else
        {
            ++content; // Consume '\\' character
            for (; *content != '\0'; ++content, ++iBuffer)
            {
                if (iBuffer >= TEX_BUFFER_SIZE)
                {
                    TRACELOG(LOG_WARNING, "RAYTEX: DrawRayTeX() command cannot be longer than %i characters", TEX_BUFFER_SIZE);
                }

                char ch = *content;
                if (!(ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z'))
                {
                    isInSymbol = false;
                    break;
                }

                buffer[iBuffer] = *content;
            }
            buffer[iBuffer] = '\0';
            RayTexSymbol symbol = RayTeXSymbolFromName(buffer);
            DrawRayTeXSymbol(symbol, x + xOffset, y + yOffset, tex.fontSize, tex.color);
            xOffset += MeasureRayTeXSymbolWidth(symbol, tex.fontSize);
        }
    }
}

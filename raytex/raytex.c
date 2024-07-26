#include <raylib.h>
#include "raytex.h"

#if defined(SUPPORT_TRACELOG)
    #define TRACELOG(level, ...) TraceLog(level, __VA_ARGS__)

    #if defined(SUPPORT_TRACELOG_DEBUG)
        #define TRACELOGD(...) TraceLog(LOG_DEBUG, __VA_ARGS__)
    #else
        #define TRACELOGD(...) (void)0
    #endif
#else
    #define TRACELOG(level, ...) (void)0
    #define TRACELOGD(...) (void)0
#endif

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
    DrawText(tex.content, x, y, tex.fontSize, tex.color);
}

#ifndef TIMELINE_PARAMS_H
#define TIMELINE_PARAMS_H

// LO/PO
#define LO 0
#define PO 1

// Scaling factor
//#define SCALING_FACTOR ( ((float)100) / (3600) )

// Size and position
#define TIMELINE_DEFAULT_HEIGHT (20)
#define TIMELINE_DEFAULT_WIDTH (QApplication::desktop()->screenGeometry().width() - 10)
#define EDITOR_DEFAULT_HEIGHT (800)
#define EDITOR_DEFAULT_WIDTH TIMELINE_DEFAULT_WIDTH

#define MAXIMIZED 1

#define BRANCH_SEPARATION (50.0)
//#define MARGIN (20.0)
#define TOP_MARGIN (50.0)
#define LEFT_MARGIN (50.0)
#define BOTTOM_MARGIN (20.0)
#define RIGHT_MARGIN (50.0)
#define AXIS_BOTTOM_MARGIN (10.0)

// Text
#define TEXT_COLOR (Qt::red)
//#define EDITOR_LEFT_EXCLUSIVE_HIGHLIGHT_COLOR (QColor(Qt::yellow).lighter(160))
//#define EDITOR_RIGHT_EXCLUSIVE_HIGHLIGHT_COLOR (QColor(Qt::red).lighter(200))
//#define EDITOR_COMMON_HIGHLIGHT_COLOR (QColor(Qt::blue).lighter(150))
#define EDITOR_COMMON_HIGHLIGHT_COLOR (QColor(Qt::yellow).lighter(160))
#define EDITOR_EXCLUSIVE_HIGHLIGHT_COLOR (QColor(Qt::red).lighter(180))

// Points
#define POINT_MINIMUM_DISTANCE (5.0)

#define POINT_DEFAULT_RADIUS (5.0)
#define POINT_DEFAULT_OUTLINE_WIDTH (1.0)
#define POINT_HEAD_OUTLINE_WIDTH (3.0)

#define POINT_DEFAULT_OUTLINE_COLOR (Qt::darkBlue)
#define POINT_HEAD_OUTLINE_COLOR (QColor(200, 0, 0, 255))

#define POINT_DEFAULT_BG_COLOR (Qt::blue)
#define POINT_SELECTED_BG_COLOR (Qt::red)
#define POINT_HOVER_BG_COLOR (QColor(POINT_DEFAULT_BG_COLOR).lighter())
#define POINT_HOVER_SELECTED_BG_COLOR (QColor(POINT_SELECTED_BG_COLOR).lighter())

#define POINT_CURRENT_DEFAULT_BG_COLOR (Qt::green)
#define POINT_CURRENT_HOVER_BG_COLOR (QColor(POINT_CURRENT_DEFAULT_BG_COLOR).lighter())
#define POINT_CURRENT_SELECTED_BG_COLOR (QColor(POINT_CURRENT_DEFAULT_BG_COLOR).darker())
#define POINT_CURRENT_HOVER_SELECTED_BG_COLOR (QColor(POINT_CURRENT_DEFAULT_BG_COLOR).darker())

#define ANIMATION_TIME (200)
#define ANIMATION_EASING_CURVE (QTimeLine::EaseOutCurve)

// Lines
#define LINE_DEFAULT_COLOR (QColor(0,0,0,255*0.4))
#define LINE_DEFAULT_WIDTH (1.0)
#define LINE_DEFAULT_Z_VALUE (-1)

// Ticks
#define TICK_DEFAULT_COLOR (Qt::black)
#define TICK_DEFAULT_Z_VALUE (-2)
#define TICK_DEFAULT_WIDTH (2.0)
#define TICK_DEFAULT_HEIGHT (8.0)

// Axis
#define AXIS_DEFAULT_COLOR (Qt::black)
#define AXIS_DEFAULT_WIDTH (2.0)
#define AXIS_DEFAULT_TICK_SEPARATION (100.0)
#define AXIS_DEFAULT_Z_VALUE (-3)

// Tags
#define TAG_MAX_LENGTH (8)

// Objects
#define OBJECTS_DIR "files/"
#define DIFF_FILE "files/diff"

#endif

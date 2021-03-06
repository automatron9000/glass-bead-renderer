#include <stddef.h>
#include <stdlib.h>
#include "canvas.h"

static Color* pixel_offset(const Canvas* canvas, Point point) {
  return canvas->pixels + point.y * canvas->width + point.x;
}

Canvas* canvas_create(int16_t width, int16_t height) {
  Canvas* canvas = malloc(sizeof(Canvas));
  canvas->width = width;
  canvas->height = height;
  canvas->pitch = sizeof(Color) * width;
  canvas->pixels = malloc(canvas->pitch * height);

  return canvas;
}

void canvas_fill(Canvas* canvas, Color color) {
  for (int ix = 0; ix < canvas->width * canvas->height; ++ix) {
    canvas->pixels[ix] = color;
  }
}

void canvas_draw_pixel(Canvas* canvas, Color color, Point point) {
  Color* pixel = pixel_offset(canvas, point);
  *pixel = color;
}

void canvas_draw_line(Canvas* canvas, Color color, Point start, Point end) {
  int x0 = start.x,
    y0 = start.y,
    dx = abs(end.x - x0),
    dy = abs(end.y - y0),
    step_x = x0 < end.x ? 1 : -1,
    step_y = y0 < end.y ? 1 : -1,
    correction = (dx > dy ? dx : -dy) / 2;

  for (;;) {
    canvas_draw_pixel(canvas, color, (Point) {x0, y0});

    if (x0 == end.x && y0 == end.y) {
      break;
    }

    int current_correction = correction;
    if (current_correction > -dx) {
      correction -= dy;
      x0 += step_x;
    }
    if (current_correction < dy) {
      correction += dx;
      y0 += step_y;
    }
  }
}

void canvas_glitch_line(Canvas* canvas, Color color, Point start, Point end) {
  int correction = 0,
    dx = end.x - start.x,
    dy = end.y - start.y,
    dx2 = dx * 2,
    dy2 = dy * 2;

  ptrdiff_t x_step = 1,
    y_step = canvas->width;

  if (dx < 0) {
    x_step = -1;
    dx = -dx;
  }

  if (dy < 0) {
    y_step = -canvas->width;
    dy = -dy;
  }

  Color* pixels = pixel_offset(canvas, start);

  if (dx > dy) {
    correction = dy2 - dx;

    for (int ix = 0; ix <= dx; ++ix) {
      *pixels = color;

      if (correction >= 0) {
        correction -= dx2;
        pixels += y_step;
      }

      correction += dy2;
      pixels += x_step;
    }
  }
  else {
    correction = dx2 - dy;

    for (int ix = 0; ix <= dy; ++ix) {
      *pixels = color;

      if (correction >= 0) {
        correction -= dy2;
        pixels += x_step;
      }

      correction += dx2;
      pixels += y_step;
    }
  }
}


void canvas_destroy(Canvas* canvas) {
  free(canvas->pixels);
  free(canvas);
}

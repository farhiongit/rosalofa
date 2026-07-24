#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define lengthof(array) (sizeof (array) / sizeof (*array))
#define between(x, a, b) ((a) < (b) ? ((a) <= (x) && (x) < (b)) : ((b) <= (x) && (x) < (a)))

typedef struct {
  double x, y;
} Point;

typedef struct {
  Point initial, terminal;
} Segment;

static double
polygon_algebric_area (size_t nb_vertices, Point *vertex) {
  double area = 0;
  for (size_t i = 0; i < nb_vertices; i++) {
    size_t j = (i + 1) % nb_vertices;
    size_t k = (i + nb_vertices - 1) % nb_vertices;
    area += vertex[i].x * (vertex[k].y - vertex[j].y);
  }
  return area / 2;
}

static int
is_inside_polygon (Point p, size_t nb_vertices, Point *vertex, int oriented) {
  int is_inside = 0;
  for (size_t i = 0; i < nb_vertices; i++) {
    size_t j = (i + 1) % nb_vertices;
    if (between (p.y, vertex[i].y, vertex[j].y)
        && (p.x <= ((vertex[j].x - vertex[i].x) * (p.y - vertex[i].y) / (vertex[j].y - vertex[i].y) + vertex[i].x)))
      is_inside = 1 - is_inside;
  }
  // Exterior rings are clockwise, and holes are counterclockwise.
  return (oriented && polygon_algebric_area (nb_vertices, vertex) < 0) /* hole */ ? !is_inside : is_inside;
}

static Point /* not really a Point here */
segments_intersection (Segment s1, Segment s2) {
  Point u = { s1.terminal.x - s1.initial.x, s1.terminal.y - s1.initial.y };
  Point v = { s2.terminal.x - s2.initial.x, s2.terminal.y - s2.initial.y };
  Point t = { s2.initial.x - s1.initial.x, s2.initial.y - s1.initial.y };

  double A = v.x * u.y - u.x * v.y;
  if (iszero (A))
    return (Point){
      INFINITY, (u.x * v.x < 0) ? -INFINITY : INFINITY
    };
  return (Point){
    (v.x * t.y - v.y * t.x) / A, (u.x * t.y - u.y * t.x) / A
  };
}

static int
polygons_union (size_t n1, Point *p1, size_t n2, Point *p2, size_t *nu, Point **u) {
  int res = 0;
  Point *r = 0;
  size_t nr = 0;

  size_t ip = 1;
  size_t iv = 0;
  Point from = *p1;

  int done = 0;
  do {
    Point *p = ip == 1 ? p1 : p2;
    size_t n = ip == 1 ? n1 : n2;
    assert (iv < n);
    Point to = p[(iv + 1) % n];
    fprintf (stderr, "[(%g, %g) ; (%g, %g)] on %zu\n", from.x, from.y, to.x, to.y, ip);
    size_t switch_p = 0;
    for (size_t i = 0; i < (ip == 1 ? n2 : n1); i++) {
      size_t j = (i + 1) % (ip == 1 ? n2 : n1);
      Point I = segments_intersection ((Segment){
                                           from, to },
                                       (Segment){ (ip == 1 ? p2 : p1)[i], (ip == 1 ? p2 : p1)[j] });
      if (isfinite (I.x) && 0 < I.x && I.x <= 1 && isfinite (I.y) && 0 <= I.y && I.y < 1) {
        fprintf (stderr, "[(%g, %g) ; (%g, %g)] X [(%g, %g) ; (%g, %g)] = ",
                 from.x, from.y, to.x, to.y,
                 (ip == 1 ? p2 : p1)[i].x, (ip == 1 ? p2 : p1)[i].y,
                 (ip == 1 ? p2 : p1)[j].x, (ip == 1 ? p2 : p1)[j].y);
        to = (Point){
          from.x + I.x * (to.x - from.x), from.y + I.x * (to.y - from.y)
        };
        fprintf (stderr, "(%g, %g) => Switch\n", to.x, to.y);
        switch_p = i + 1;
      }
    }
    if (switch_p) {
      ip = 3 - ip; // 1 <-> 2
      iv = switch_p - 1;
      res = 1;
      p = ip == 1 ? p1 : p2;
      n = ip == 1 ? n1 : n2;
    } else
      iv = (iv + 1) % n;
    r = realloc (r, ++nr * sizeof (*r));
    r[nr - 1] = to;
    fprintf (stderr, "r (%g, %g)\n", r[nr - 1].x, r[nr - 1].y);

    from = to;
    done = (p == p1 && iv == 0);
  } while (!done);

  if (nu && u && res) {
    *nu = nr;
    *u = r;
  } else
    free (r);
  return res;
}

static int
polygons_intersect (size_t n1, Point *p1, size_t n2, Point *p2) {
  return polygons_union (n1, p1, n2, p2, 0, 0) || is_inside_polygon (*p1, n2, p2, 1) || is_inside_polygon (*p2, n1, p1, 1);
}

static double
distance (Point a, Point b) {
  return sqrt ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

static double
distance_to_segment (Point a, Segment s) {
  if (iszero (distance (s.initial, s.terminal)))
    return NAN;
  Point triangle[3] = { a, s.initial, s.terminal };
  return fabs (polygon_algebric_area (3, triangle) / distance (s.initial, s.terminal) * 2);
}

static double
dot_product (Segment s1, Segment s2) {
  return (s1.terminal.x - s1.initial.x) * (s2.terminal.x - s2.initial.x) + (s1.terminal.y - s1.initial.y) * (s2.terminal.y - s2.initial.y);
}

static int
circle_contains_segment (Point c, double r, Segment s) {
  return (distance (c, s.initial) <= r && distance (c, s.terminal) <= r);
}

static int
circle_intersect_segment (Point c, double r, Segment s) {
  if (iszero (r) || circle_contains_segment (c, r, s))
    return 0; // Totally included
  assert (distance (c, s.initial) > r || distance (c, s.terminal) > r);
  if (distance (c, s.initial) <= r || distance (c, s.terminal) <= r)
    return 1; // Partially included
  assert (distance (c, s.initial) > r && distance (c, s.terminal) > r);
  if (!isfinite (distance_to_segment (c, s)) || distance_to_segment (c, s) > r)
    return 0;
  Segment s1 = { s.initial, c };
  Segment s2 = { s.terminal, s.initial };
  Segment s3 = { s.terminal, c };
  if (dot_product (s, s1) >= 0 && dot_product (s2, s3) >= 0)
    return 1;
  return 0;
}

int
main () {
  Point I = segments_intersection ((Segment){ { 0, 0 }, { -.1, -.1 } }, (Segment){ { 1, 0 }, { .8, .2 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);
  I = segments_intersection ((Segment){ { 0, 0 }, { 1, 0 } }, (Segment){ { 0.5, 0.5 }, { 0.5, 1.5 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);
  I = segments_intersection ((Segment){ { 0, 0 }, { -.1, -.1 } }, (Segment){ { 0, 0 }, { -.1, -.1 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);

  Point polygon3[] = { { 0, 0 }, { 1, 1 } }; // clockwise = surface.
  fprintf (stdout, "Polygon area = %g\n", polygon_algebric_area (lengthof (polygon3), polygon3));

  Point square[] = { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } }; // counterclockwise = hole.
  double area = polygon_algebric_area (lengthof (square), square);
  fprintf (stdout, "Polygon area = %g\n", area);

  Point p = { 0.5, 0.5 };
  fprintf (stdout, "%s\n", is_inside_polygon (p, lengthof (square), square, 0) ? "In" : "Out");
  fprintf (stdout, "%s\n", is_inside_polygon (p, lengthof (square), square, 1) ? "In" : "Out");

  Point polygon2[] = { { 0.5, 1.5 }, { 1.5, 1.5 }, { 1.5, 0.5 }, { 0.5, 0.5 } }; // clockwise = fill.
  double area2 = polygon_algebric_area (lengthof (polygon2), polygon2);
  fprintf (stdout, "Polygon area = %g\n", area2);

  size_t nu = 0;
  Point *u = 0;
  if (polygons_union (lengthof (square), square, lengthof (polygon2), polygon2, &nu, &u)) {
    fprintf (stdout, "%g\n", polygon_algebric_area (nu, u));
    free (u);
  }
  if (polygons_union (lengthof (polygon2), polygon2, lengthof (square), square, &nu, &u)) {
    fprintf (stdout, "%g\n", polygon_algebric_area (nu, u));
    free (u);
  }
  fprintf (stdout, "Do %s intersect.\n", polygons_intersect (lengthof (square), square, lengthof (polygon2), polygon2) ? "" : "not ");

  Point star[30];
  for (size_t i = 0; i < lengthof (star) / 2; i++) {
    star[2 * i] = (Point){ .5 * cos ((double)(2 * i) * 2 * M_PI / lengthof (star)), .5 * sin ((double)(2 * i) * 2 * M_PI / lengthof (star)) };
    star[2 * i + 1] = (Point){ 1.5 * cos ((double)(2 * i + 1) * 2 * M_PI / lengthof (star)), 1.5 * sin ((double)(2 * i + 1) * 2 * M_PI / lengthof (star)) };
  }
  for (size_t i = 0; i < lengthof (star); i++)
    fprintf (stdout, "(%g, %g)\n", star[i].x, star[i].y);
  if (polygons_union (lengthof (star), star, lengthof (square), square, &nu, &u)) {
    for (size_t i = 0; i < nu; i++)
      fprintf (stdout, "(%g, %g)\n", u[i].x, u[i].y);
    free (u);
  }

  for (size_t i = 0; i < lengthof (square); i++) {
    size_t j = (i + 1) % lengthof (square);
    if (circle_intersect_segment ((Point){ 5, 5 }, sqrt (2) * 5 - 1.4142135, (Segment){ square[i], square[j] })) {
      fprintf (stdout, "Do intersect.\n");
      break;
    }
  }
  for (size_t i = 0; i < lengthof (square); i++) {
    size_t j = (i + 1) % lengthof (square);
    if (circle_intersect_segment ((Point){ 0, 5 }, 4.0001, (Segment){ square[i], square[j] })) {
      fprintf (stdout, "Do intersect.\n");
      break;
    }
  }
}

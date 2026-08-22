#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define lengthof(array) (sizeof (array) / sizeof (*array))
#define between(x, a, b) ((a) < (b) ? ((a) <= (x) && (x) < (b)) : ((b) <= (x) && (x) < (a)))
#undef iszero
#define iszero(x) (fpclassify (x) == FP_ZERO)
#define feq(a, b) (fabs ((double)(a) - (double)(b)) < 1e-8)

typedef struct {
  double x, y;
} Point;

// Vectors
// https://en.wikipedia.org/wiki/Linear_map, https://en.wikipedia.org/wiki/Affine_transformation
typedef Point Vector; // Vector from origin to point

static Vector
to_vector (Point initial, Point terminal) {
  return (Vector){ terminal.x - initial.x, terminal.y - initial.y };
}

static double
norm (Vector v) {
  return sqrt (v.x * v.x + v.y * v.y);
}

static double
dot_product (Vector a, Vector b) {
  return a.x * b.x + a.y * b.y;
}

typedef struct {
  Vector u, v;
} Referential;

static double
det (Referential f) {
  return f.u.x * f.v.y - f.u.y * f.v.x;
}

static Referential
inv (Referential f) {
  double d = det (f);
  if (iszero (d))
    return (Referential){ { NAN, NAN }, { NAN, NAN } };                         // Not invertible
  return (Referential){ { f.v.y / d, -f.u.y / d }, { -f.v.x / d, f.u.x / d } }; // adj(f) / det(f)
}

static Vector
linear_transform (Referential a, Vector v) {
  return (Vector){ a.u.x * v.x + a.v.x * v.y, a.u.y * v.x + a.v.y * v.y }; // ax
}

// Points
static double
distance (Point a, Point b) {
  return norm (to_vector (a, b));
}

// Lines and segments
typedef struct {
  Point initial, terminal;
} Segment;

static Segment
to_segment (Point initial, Point terminal) {
  return (Segment){ initial, terminal };
}

static Vector
segment_to_vector (Segment s) {
  return to_vector (s.initial, s.terminal);
}

static double
length (Segment s) {
  return distance (s.initial, s.terminal);
}

static double /* Signed, negative if s turns clockwise with respect to a */
distance_to_segment (Point a, Segment s) {
  double base_length = length (s);
  if (iszero (base_length))
    return NAN;
  // https://en.wikipedia.org/wiki/Determinant
  return det ((Referential){ to_vector (a, s.initial), to_vector (a, s.terminal) }) / base_length;
}

static Point /* not really a Point here */
lines_intersection (Segment s1, Segment s2) {
  Point u = { s1.terminal.x - s1.initial.x, s1.terminal.y - s1.initial.y };
  Point v = { s2.terminal.x - s2.initial.x, s2.terminal.y - s2.initial.y };
  Point t = { s2.initial.x - s1.initial.x, s2.initial.y - s1.initial.y };

  double A = v.x * u.y - u.x * v.y;
  if (iszero (A))
    return (Point){ NAN, NAN }; // Do not intersect.
  return (Point){ (v.x * t.y - v.y * t.x) / A, (u.x * t.y - u.y * t.x) / A };
}

static Point /* intersection */
segments_intersection (Segment s1, Segment s2) {
  Point I = lines_intersection (s1, s2);
  if (isfinite (I.x) && 0 < I.x && I.x <= 1 && isfinite (I.y) && 0 <= I.y && I.y < 1)
    return (Point){ s1.initial.x + I.x * (s1.terminal.x - s1.initial.x), s1.initial.y + I.x * (s1.terminal.y - s1.initial.y) };
  else
    return (Point){ NAN, NAN }; // Do not intersect.
}

static double
segment_dot_product (Segment s1, Segment s2) {
  return dot_product (segment_to_vector (s1), segment_to_vector (s2));
}

// Polygons
static double /* Oriented, can be negative */
polygon_algebric_area (size_t nb_vertices, Point *vertex) {
  double area = 0;
  for (size_t i = 0; i < nb_vertices; i++) {
    size_t j = (i + 1) % nb_vertices;
    size_t k = (i + nb_vertices - 1) % nb_vertices;
    area += vertex[i].x * (vertex[k].y - vertex[j].y);
  }
  return area / 2;
}

static int /* Exterior (filled) rings are clockwise, and holes are counterclockwise. */
is_inside_polygon (Point p, size_t nb_vertices, Point *vertex) {
  int is_inside = 0;
  for (size_t i = 0; i < nb_vertices; i++) {
    size_t j = (i + 1) % nb_vertices;
    if (between (p.y, vertex[i].y, vertex[j].y)
        && (p.x <= ((vertex[j].x - vertex[i].x) * (p.y - vertex[i].y) / (vertex[j].y - vertex[i].y) + vertex[i].x)))
      is_inside = 1 - is_inside;
  }
  return polygon_algebric_area (nb_vertices, vertex) < 0 /* hole */ ? !is_inside : is_inside;
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
      Point I = segments_intersection ((Segment){ from, to },
                                       (Segment){ (ip == 1 ? p2 : p1)[i], (ip == 1 ? p2 : p1)[j] });
      if (isfinite (I.x)) {
        fprintf (stderr, "[(%g, %g) ; (%g, %g)] X [(%g, %g) ; (%g, %g)] = ",
                 from.x, from.y, to.x, to.y,
                 (ip == 1 ? p2 : p1)[i].x, (ip == 1 ? p2 : p1)[i].y,
                 (ip == 1 ? p2 : p1)[j].x, (ip == 1 ? p2 : p1)[j].y);
        to = I;
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
  // return polygons_union (n1, p1, n2, p2, 0, 0);
  for (size_t i1 = 0; i1 < n1; i1++)
    for (size_t i2 = 0; i2 < n2; i2++)
      if (isfinite (segments_intersection ((Segment){ p1[i1], p1[(i1 + 1) % n1] }, (Segment){ p2[i2], p2[(i2 + 1) % n2] }).x))
        return 1;

  return 0;
}

static int
polygon_contains_polygon (size_t n1, Point *p1, size_t n2, Point *p2) {
  return !polygons_intersect (n1, p1, n2, p2) && is_inside_polygon (*p2, n1, p1);
}

static int
polygons_overlap (size_t n1, Point *p1, size_t n2, Point *p2) {
  return polygons_intersect (n1, p1, n2, p2) || is_inside_polygon (*p2, n1, p1) || is_inside_polygon (*p1, n2, p2);
}

// Circles
static double
circle_area (double r) {
  return M_PI * r * r;
}

static int
circle_contains_segment (Point c, double r, Segment s) {
  return (distance (c, s.initial) <= r && distance (c, s.terminal) <= r);
}

static int
circle_intersects_segment (Point c, double r, Segment s) {
  if (iszero (r) || circle_contains_segment (c, r, s))
    return 0; // Totally inside
  assert (distance (c, s.initial) > r || distance (c, s.terminal) > r);
  if (distance (c, s.initial) <= r || distance (c, s.terminal) <= r)
    return 1; // One end inside
  assert (distance (c, s.initial) > r && distance (c, s.terminal) > r);
  if (!isfinite (distance_to_segment (c, s)) || fabs (distance_to_segment (c, s)) > r)
    return 0; // Totally outside
  Segment s1 = { s.initial, c };
  Segment s2 = { s.terminal, s.initial };
  Segment s3 = { s.terminal, c };
  if (segment_dot_product (s, s1) >= 0 && segment_dot_product (s2, s3) >= 0)
    return 1; // The segment crosses the circle
  return 0;   // The line holding the segment crosses the circle, but the segment does not.
}

static double /* Signed, negative if vectors turn clockwise */
ellipse_area (Vector ra, Vector rb) {
  return circle_area (1) * det ((Referential){ ra, rb });
}

static int
ellipse_contains_segment (Point c, Vector ra, Vector rb, Segment s) {
  // https://en.wikipedia.org/wiki/Change_of_basis
  Referential t = inv ((Referential){ ra, rb });
  if (!isfinite (t.u.x))
    return 0;
  return circle_contains_segment (linear_transform (t, c), 1, (Segment){ linear_transform (t, s.initial), linear_transform (t, s.terminal) });
}

static int
ellipse_intersects_segment (Point c, Vector ra, Vector rb, Segment s) {
  Referential t = inv ((Referential){ ra, rb });
  if (!isfinite (t.u.x))
    return 0;
  return circle_intersects_segment (linear_transform (t, c), 1, (Segment){ linear_transform (t, s.initial), linear_transform (t, s.terminal) });
}

// Tests
int
main () {
  fprintf (stdout, "%g\n", distance_to_segment ((Point){ 1, 1 }, to_segment ((Point){ 1, 0 }, (Point){ 0, 1 })));

  Point I = lines_intersection ((Segment){ { 0, 0 }, { -.1, -.1 } }, (Segment){ { 1, 0 }, { .8, .2 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);
  I = lines_intersection ((Segment){ { 0, 0 }, { 1, 0 } }, (Segment){ { 0.5, 0.5 }, { 0.5, 1.5 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);
  I = lines_intersection ((Segment){ { 0, 0 }, { -.1, -.1 } }, (Segment){ { 0, 0 }, { -.1, -.1 } });
  fprintf (stdout, "(%g, %g)\n", I.x, I.y);

  fprintf (stdout, "Polygon area = %g\n", polygon_algebric_area (1, (Point[]){ { 1, 1 } }));
  fprintf (stdout, "Polygon area = %g\n", polygon_algebric_area (2, (Point[]){ { 0, 0 }, { 1, 1 } }));

  Point square[] = { { -1, -1 }, { 1, -1 }, { 1, 1 }, { -1, 1 } }; // counterclockwise = hole.
  double area = polygon_algebric_area (lengthof (square), square);
  fprintf (stdout, "Polygon area = %g\n", area);

  Point p = { 0.5, 0.5 };
  fprintf (stdout, "%s\n", is_inside_polygon (p, lengthof (square), square) ? "In" : "Out");

  Point polygon2[] = { { 0.5, 1.5 }, { 1.5, 1.5 }, { 1.5, 0.5 }, { 0.5, 0.5 } }; // clockwise = fill.
  p = (Point){ 1, 1 };
  fprintf (stdout, "%s\n", is_inside_polygon (p, lengthof (square), square) ? "In" : "Out");
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
  fprintf (stdout, "Does %scontain.\n", polygon_contains_polygon (lengthof (square), square, lengthof (polygon2), polygon2) ? "" : "not ");
  fprintf (stdout, "Do %soverlap.\n", polygons_overlap (lengthof (square), square, lengthof (polygon2), polygon2) ? "" : "not ");

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

  fprintf (stdout, "%g\n", distance_to_segment ((Point){ 1, 1 }, (Segment){ { 1, 2 }, { 2, 1 } }));
  fprintf (stdout, "Circle area = %g\n", circle_area (2));
  fprintf (stdout, "Does %scontain.\n", circle_contains_segment ((Point){ 1, 1 }, 2, (Segment){ { 1, 2 }, { 2, 1 } }) ? "" : "not ");
  fprintf (stdout, "%g\n", distance_to_segment ((Point){ 1, 1 }, (Segment){ { 4, 0 }, { 0, 4 } }));
  fprintf (stdout, "Do %sintersect.\n", circle_intersects_segment ((Point){ 1, 1 }, 2, (Segment){ { 4, 0 }, { 0, 4 } }) ? "" : "not ");

  for (size_t i = 0; i < lengthof (square); i++) {
    size_t j = (i + 1) % lengthof (square);
    if (circle_intersects_segment ((Point){ 5, 5 }, sqrt (2) * 5 - 1.4142135, (Segment){ square[i], square[j] })) {
      fprintf (stdout, "Do intersect..\n");
      break;
    }
  }
  for (size_t i = 0; i < lengthof (square); i++) {
    size_t j = (i + 1) % lengthof (square);
    if (circle_intersects_segment ((Point){ 0, 5 }, 4.0001, (Segment){ square[i], square[j] })) {
      fprintf (stdout, "Do intersect...\n");
      break;
    }
  }

  fprintf (stdout, "Ellipse area = %g\n", ellipse_area ((Vector){ 0, 2 }, (Vector){ 1, 0 }));
  fprintf (stdout, "Does %scontain.\n", ellipse_contains_segment ((Point){ 0, 0 }, (Vector){ 0, 2 }, (Vector){ 1, 0 }, (Segment){ { -0.1, 1.5 }, { 0.1, 1.5 } }) ? "" : "not ");
  fprintf (stdout, "Do %sintersect.\n", ellipse_intersects_segment ((Point){ 0, 0 }, (Vector){ 0, 2 }, (Vector){ 1, 0 }, (Segment){ { -0.5, 1.9 }, { 0.5, 1.9 } }) ? "" : "not ");
}

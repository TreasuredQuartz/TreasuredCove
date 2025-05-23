// Fill out your copyright notice in the Description page of Project Settings.


#include "MeshExtension/Delaunauy.h"
#include "Misc/RuntimeErrors.h"

namespace Delaunay {

    //@see https://stackoverflow.com/questions/33333363/built-in-mod-vs-custom-mod-function-improve-the-performance-of-modulus-op/33333636#33333636
    inline size_t fast_mod(const size_t i, const size_t c) {
        return i >= c ? i % c : i;
    }

    // Kahan and Babuska summation, Neumaier variant; accumulates less FP error
    inline double sum(const TArray<double>& x) {
        double sum = x[0];
        double err = 0.0;

        for (size_t i = 1; i < x.Num(); i++) {
            const double k = x[i];
            const double m = sum + k;
            err += FMath::Abs(sum) >= FMath::Abs(k) ? sum - m + k : k - m + sum;
            sum = m;
        }
        return sum + err;
    }

    inline double dist(
        const double ax,
        const double ay,
        const double bx,
        const double by) {
        const double dx = ax - bx;
        const double dy = ay - by;
        return dx * dx + dy * dy;
    }

    inline double circumradius(const Point& p1, const Point& p2, const Point& p3)
    {
        Point d = Point::vector(p1, p2);
        Point e = Point::vector(p1, p3);

        const double bl = d.magnitude2();
        const double cl = e.magnitude2();
        const double det = Point::determinant(d, e);

        Point radius((e.y() * bl - d.y() * cl) * 0.5 / det,
            (d.x() * cl - e.x() * bl) * 0.5 / det);

        if ((bl > 0.0 || bl < 0.0) &&
            (cl > 0.0 || cl < 0.0) &&
            (det > 0.0 || det < 0.0))
            return radius.magnitude2();
        return TNumericLimits<double>::Max();
    }

    inline double circumradius(
        const double ax,
        const double ay,
        const double bx,
        const double by,
        const double cx,
        const double cy) {
        const double dx = bx - ax;
        const double dy = by - ay;
        const double ex = cx - ax;
        const double ey = cy - ay;

        const double bl = dx * dx + dy * dy;
        const double cl = ex * ex + ey * ey;
        const double d = dx * ey - dy * ex;

        const double x = (ey * bl - dy * cl) * 0.5 / d;
        const double y = (dx * cl - ex * bl) * 0.5 / d;

        if ((bl > 0.0 || bl < 0.0) && (cl > 0.0 || cl < 0.0) && (d > 0.0 || d < 0.0)) {
            return x * x + y * y;
        }
        else {
            return TNumericLimits<double>::Max();
        }
    }

    inline bool clockwise(const Point& p0, const Point& p1, const Point& p2)
    {
        Point v0 = Point::vector(p0, p1);
        Point v1 = Point::vector(p0, p2);
        double det = Point::determinant(v0, v1);
        double dist = v0.magnitude2() + v1.magnitude2();
        double dist2 = Point::dist2(v0, v1);
        if (det == 0)
        {
            return false;
        }
        double reldet = FMath::Abs(dist / det);
        if (reldet > 1e14)
            return false;
        return det < 0;
    }

    inline bool clockwise(double px, double py, double qx, double qy,
        double rx, double ry)
    {
        Point p0(px, py);
        Point p1(qx, qy);
        Point p2(rx, ry);
        return clockwise(p0, p1, p2);
    }

    inline bool counterclockwise(const Point& p0, const Point& p1, const Point& p2)
    {
        Point v0 = Point::vector(p0, p1);
        Point v1 = Point::vector(p0, p2);
        double det = Point::determinant(v0, v1);
        double dist = v0.magnitude2() + v1.magnitude2();
        double dist2 = Point::dist2(v0, v1);
        if (det == 0)
            return false;
        double reldet = FMath::Abs(dist / det);
        if (reldet > 1e14)
            return false;
        return det > 0;
    }

    inline bool counterclockwise(double px, double py, double qx, double qy,
        double rx, double ry)
    {
        Point p0(px, py);
        Point p1(qx, qy);
        Point p2(rx, ry);
        return counterclockwise(p0, p1, p2);
    }


    inline Point circumcenter(
        const double ax,
        const double ay,
        const double bx,
        const double by,
        const double cx,
        const double cy) {
        const double dx = bx - ax;
        const double dy = by - ay;
        const double ex = cx - ax;
        const double ey = cy - ay;

        const double bl = dx * dx + dy * dy;
        const double cl = ex * ex + ey * ey;
        //ABELL - This is suspect for div-by-0.
        const double d = dx * ey - dy * ex;

        const double x = ax + (ey * bl - dy * cl) * 0.5 / d;
        const double y = ay + (dx * cl - ex * bl) * 0.5 / d;

        return Point(x, y);
    }

    inline bool in_circle(
        const double ax,
        const double ay,
        const double bx,
        const double by,
        const double cx,
        const double cy,
        const double px,
        const double py) {
        const double dx = ax - px;
        const double dy = ay - py;
        const double ex = bx - px;
        const double ey = by - py;
        const double fx = cx - px;
        const double fy = cy - py;

        const double ap = dx * dx + dy * dy;
        const double bp = ex * ex + ey * ey;
        const double cp = fx * fx + fy * fy;

        return (dx * (ey * cp - bp * fy) -
            dy * (ex * cp - bp * fx) +
            ap * (ex * fy - ey * fx)) < 0.0;
    }

    inline bool check_pts_equal(double x1, double y1, double x2, double y2) {
        return FMath::Abs(x1 - x2) <= DBL_EPSILON &&
            FMath::Abs(y1 - y2) <= DBL_EPSILON;
    }

    // monotonically increases with real angle, but doesn't need expensive trigonometry
    inline double pseudo_angle(const double dx, const double dy) {
        const double p = dx / (FMath::Abs(dx) + FMath::Abs(dy));
        return (dy > 0.0 ? 3.0 - p : 1.0 + p) / 4.0; // [0..1)
    }


    Delaunay::Delaunay(TArray<double> const& in_coords)
        : coords(in_coords), m_points(in_coords)
    {
        size_t n = coords.Num() >> 1;

        TArray<size_t> ids;
        ids.Reserve(n);

        for (int i = 0; i < n; ++i)
        {
            ids.Add(i);
        }

        double max_x = TNumericLimits<double>::Lowest();
        double max_y = TNumericLimits<double>::Lowest();
        double min_x = TNumericLimits<double>::Max();
        double min_y = TNumericLimits<double>::Max();
        for (const Point& p : m_points)
        {
            min_x = FMath::Min(p.x(), min_x);
            min_y = FMath::Min(p.y(), min_y);
            max_x = FMath::Max(p.x(), max_x);
            max_y = FMath::Max(p.y(), max_y);
        }
        double width = max_x - min_x;
        double height = max_y - min_y;
        double span = width * width + height * height; // Everything is square dist.

        Point center((min_x + max_x) / 2, (min_y + max_y) / 2);

        size_t i0 = INVALID_INDEX;
        size_t i1 = INVALID_INDEX;
        size_t i2 = INVALID_INDEX;

        // pick a seed point close to the centroid
        double min_dist = TNumericLimits<double>::Max();
        for (size_t i = 0; i < m_points.size(); ++i)
        {
            const Point& p = m_points[i];
            const double d = Point::dist2(center, p);
            if (d < min_dist) {
                i0 = i;
                min_dist = d;
            }
        }

        const Point& p0 = m_points[i0];

        min_dist = TNumericLimits<double>::Max();

        // find the point closest to the seed
        for (size_t i = 0; i < n; i++) {
            if (i == i0) continue;
            const double d = Point::dist2(p0, m_points[i]);
            if (d < min_dist && d > 0.0) {
                i1 = i;
                min_dist = d;
            }
        }

        const Point& p1 = m_points[i1];

        double min_radius = TNumericLimits<double>::Max();

        // find the third point which forms the smallest circumcircle
        // with the first two
        for (size_t i = 0; i < n; i++) {
            if (i == i0 || i == i1) continue;

            const double r = circumradius(p0, p1, m_points[i]);
            if (r < min_radius) {
                i2 = i;
                min_radius = r;
            }
        }

        if (!(min_radius < TNumericLimits<double>::Max())) {
            FRuntimeErrors::LogRuntimeIssue(ELogVerbosity::Error, "__DelaunayTriangulation", 1, FText::FromString("Cannot link edge"));
        }

        const Point& p2 = m_points[i2];

        if (counterclockwise(p0, p1, p2))
            Swap(i1, i2);

        double i0x = p0.x();
        double i0y = p0.y();
        double i1x = m_points[i1].x();
        double i1y = m_points[i1].y();
        double i2x = m_points[i2].x();
        double i2y = m_points[i2].y();

        m_center = circumcenter(i0x, i0y, i1x, i1y, i2x, i2y);

        // Calculate the distances from the center once to avoid having to
        // calculate for each compare.  This used to be done in the comparator,
        // but GCC 7.5+ would copy the comparator to iterators used in the
        // sort, and this was excruciatingly slow when there were many points
        // because you had to copy the vector of distances.
        TArray<double> dists;
        dists.Reserve(m_points.size());
        for (const Point& p : m_points)
            dists.Push(dist(p.x(), p.y(), m_center.x(), m_center.y()));

        // sort the points by distance from the seed triangle circumcenter
        
        ids.Sort([&dists](size_t i, size_t j) { return dists[i] < dists[j]; });

        // initialize a hash table for storing edges of the advancing convex hull
        m_hash_size = static_cast<size_t>(FMath::CeilToDouble(sqrt(n)));
        m_hash.Init(INVALID_INDEX, m_hash_size);
        
        // initialize arrays for tracking the edges of the advancing convex hull
        hull_prev.SetNumZeroed(n);
        hull_next.SetNumZeroed(n);
        hull_tri.SetNumZeroed(n);

        hull_start = i0;

        size_t hull_size = 3;

        hull_next[i0] = hull_prev[i2] = i1;
        hull_next[i1] = hull_prev[i0] = i2;
        hull_next[i2] = hull_prev[i1] = i0;

        hull_tri[i0] = 0;
        hull_tri[i1] = 1;
        hull_tri[i2] = 2;

        m_hash[hash_key(i0x, i0y)] = i0;
        m_hash[hash_key(i1x, i1y)] = i1;
        m_hash[hash_key(i2x, i2y)] = i2;

        // ABELL - Why are we doing this is n < 3?  There is no triangulation if
        //  there is no triangle.

        size_t max_triangles = n < 3 ? 1 : 2 * n - 5;
        triangles.Reserve(max_triangles * 3);
        halfedges.Reserve(max_triangles * 3);
        add_triangle(i0, i1, i2, INVALID_INDEX, INVALID_INDEX, INVALID_INDEX);
        double xp = NAN; 
        double yp = NAN;

        // Go through points based on distance from the center.
        for (size_t k = 0; k < n; k++) {
            const size_t i = ids[k];
            const double x = coords[2 * i];
            const double y = coords[2 * i + 1];

            // skip near-duplicate points
            if (k > 0 && check_pts_equal(x, y, xp, yp))
                continue;
            xp = x;
            yp = y;

            //ABELL - This is dumb.  We have the indices.  Use them.
            // skip seed triangle points
            if (check_pts_equal(x, y, i0x, i0y) ||
                check_pts_equal(x, y, i1x, i1y) ||
                check_pts_equal(x, y, i2x, i2y)) continue;

            // find a visible edge on the convex hull using edge hash
            size_t start = 0;

            size_t key = hash_key(x, y);
            for (size_t j = 0; j < m_hash_size; j++) {
                start = m_hash[fast_mod(key + j, m_hash_size)];

                // ABELL - Not sure how hull_next[start] could ever equal start
                // I *think* hull_next is just a representation of the hull in one
                // direction.
                if (start != INVALID_INDEX && start != hull_next[start])
                    break;
            }

            //ABELL
            // Make sure what we found is on the hull.
            check(hull_prev[start] != start);
            check(hull_prev[start] != INVALID_INDEX);

            start = hull_prev[start];
            size_t e = start;
            size_t q;

            // Advance until we find a place in the hull where our current point
            // can be added.
            while (true)
            {
                q = hull_next[e];
                if (Point::equal(m_points[i], m_points[e], span) ||
                    Point::equal(m_points[i], m_points[q], span))
                {
                    e = INVALID_INDEX;
                    break;
                }
                if (counterclockwise(x, y, coords[2 * e], coords[2 * e + 1],
                    coords[2 * q], coords[2 * q + 1]))
                    break;
                e = q;
                if (e == start) {
                    e = INVALID_INDEX;
                    break;
                }
            }

            // ABELL
            // This seems wrong.  Perhaps we should check what's going on?
            if (e == INVALID_INDEX)     // likely a near-duplicate point; skip it
                continue;

            // add the first triangle from the point
            size_t t = add_triangle(
                e,
                i,
                hull_next[e],
                INVALID_INDEX,
                INVALID_INDEX,
                hull_tri[e]);

            hull_tri[i] = legalize(t + 2); // Legalize the triangle we just added.
            hull_tri[e] = t;
            hull_size++;

            // walk forward through the hull, adding more triangles and
            // flipping recursively
            size_t next = hull_next[e];
            while (true)
            {
                q = hull_next[next];
                if (!counterclockwise(x, y, coords[2 * next], coords[2 * next + 1],
                    coords[2 * q], coords[2 * q + 1]))
                    break;
                t = add_triangle(next, i, q,
                    hull_tri[i], INVALID_INDEX, hull_tri[next]);
                hull_tri[i] = legalize(t + 2);
                hull_next[next] = next; // mark as removed
                hull_size--;
                next = q;
            }

            // walk backward from the other side, adding more triangles and flipping
            if (e == start) {
                while (true)
                {
                    q = hull_prev[e];
                    if (!counterclockwise(x, y, coords[2 * q], coords[2 * q + 1],
                        coords[2 * e], coords[2 * e + 1]))
                        break;
                    t = add_triangle(q, i, e,
                        INVALID_INDEX, hull_tri[e], hull_tri[q]);
                    legalize(t + 2);
                    hull_tri[q] = t;
                    hull_next[e] = e; // mark as removed
                    hull_size--;
                    e = q;
                }
            }

            // update the hull indices
            hull_prev[i] = e;
            hull_start = e;
            hull_prev[next] = i;
            hull_next[e] = i;
            hull_next[i] = next;

            m_hash[hash_key(x, y)] = i;
            m_hash[hash_key(coords[2 * e], coords[2 * e + 1])] = e;
        }
    }

    double Delaunay::get_hull_area()
    {
        TArray<double> hull_area;
        size_t e = hull_start;
        size_t cnt = 1;
        do {
            hull_area.Push((coords[2 * e] - coords[2 * hull_prev[e]]) *
                (coords[2 * e + 1] + coords[2 * hull_prev[e] + 1]));
            cnt++;
            e = hull_next[e];
        } while (e != hull_start);
        return sum(hull_area);
    }

    double Delaunay::get_triangle_area()
    {
        TArray<double> vals;
        for (size_t i = 0; i < triangles.Num(); i += 3)
        {
            const double ax = coords[2 * triangles[i]];
            const double ay = coords[2 * triangles[i] + 1];
            const double bx = coords[2 * triangles[i + 1]];
            const double by = coords[2 * triangles[i + 1] + 1];
            const double cx = coords[2 * triangles[i + 2]];
            const double cy = coords[2 * triangles[i + 2] + 1];
            double val = FMath::Abs((by - ay) * (cx - bx) - (bx - ax) * (cy - by));
            vals.Push(val);
        }
        return sum(vals);
    }

    size_t Delaunay::legalize(size_t a) {
        size_t i = 0;
        size_t ar = 0;
        m_edge_stack.Empty();

        // recursion eliminated with a fixed-size stack
        while (true) {
            const size_t b = halfedges[a];

            /* if the pair of triangles doesn't satisfy the Delaunay condition
            * (p1 is inside the circumcircle of [p0, pl, pr]), flip them,
            * then do the same check/flip recursively for the new pair of triangles
            *
            *           pl                    pl
            *          /||\                  /  \
            *       al/ || \bl            al/    \a
            *        /  ||  \              /      \
            *       /  a||b  \    flip    /___ar___\
            *     p0\   ||   /p1   =>   p0\---bl---/p1
            *        \  ||  /              \      /
            *       ar\ || /br             b\    /br
            *          \||/                  \  /
            *           pr                    pr
            */
            const size_t a0 = 3 * (a / 3);
            ar = a0 + (a + 2) % 3;

            if (b == INVALID_INDEX) {
                if (i > 0) {
                    i--;
                    a = m_edge_stack[i];
                    continue;
                }
                else {
                    //i = INVALID_INDEX;
                    break;
                }
            }

            const size_t b0 = 3 * (b / 3);
            const size_t al = a0 + (a + 1) % 3;
            const size_t bl = b0 + (b + 2) % 3;

            const size_t p0 = triangles[ar];
            const size_t pr = triangles[a];
            const size_t pl = triangles[al];
            const size_t p1 = triangles[bl];

            const bool illegal = in_circle(
                coords[2 * p0],
                coords[2 * p0 + 1],
                coords[2 * pr],
                coords[2 * pr + 1],
                coords[2 * pl],
                coords[2 * pl + 1],
                coords[2 * p1],
                coords[2 * p1 + 1]);

            if (illegal) {
                triangles[a] = p1;
                triangles[b] = p0;

                auto hbl = halfedges[bl];

                // Edge swapped on the other side of the hull (rare).
                // Fix the halfedge reference
                if (hbl == INVALID_INDEX) {
                    size_t e = hull_start;
                    do {
                        if (hull_tri[e] == bl) {
                            hull_tri[e] = a;
                            break;
                        }
                        e = hull_prev[e];
                    } while (e != hull_start);
                }
                link(a, hbl);
                link(b, halfedges[ar]);
                link(ar, bl);
                size_t br = b0 + (b + 1) % 3;

                if (i < m_edge_stack.Num()) {
                    m_edge_stack[i] = br;
                }
                else {
                    m_edge_stack.Push(br);
                }
                i++;

            }
            else {
                if (i > 0) {
                    i--;
                    a = m_edge_stack[i];
                    continue;
                }
                else {
                    break;
                }
            }
        }
        return ar;
    }

    size_t Delaunay::hash_key(const double x, const double y) const {
        const double dx = x - m_center.x();
        const double dy = y - m_center.y();
        return fast_mod(
            static_cast<size_t>(FMath::RoundHalfFromZero(FMath::Floor(pseudo_angle(dx, dy) * static_cast<double>(m_hash_size)))),
            m_hash_size);
    }

    size_t Delaunay::add_triangle(
        size_t i0,
        size_t i1,
        size_t i2,
        size_t a,
        size_t b,
        size_t c) {
        size_t t = triangles.Num();
        triangles.Push(i0);
        triangles.Push(i1);
        triangles.Push(i2);
        link(t, a);
        link(t + 1, b);
        link(t + 2, c);
        return t;
    }

    void Delaunay::link(const size_t a, const size_t b) {
        size_t s = halfedges.Num();
        if (a == s) {
            halfedges.Push(b);
        }
        else if (a < s) {
            halfedges[a] = b;
        }
        else {
            FRuntimeErrors::LogRuntimeIssue(ELogVerbosity::Error, "__DelaunayTriangulation", 2, FText::FromString("Cannot link edge"));
        }
        if (b != INVALID_INDEX) {
            size_t s2 = halfedges.Num();
            if (b == s2) {
                halfedges.Push(a);
            }
            else if (b < s2) {
                halfedges[b] = a;
            }
            else {
                FRuntimeErrors::LogRuntimeIssue(ELogVerbosity::Error, "__DelaunayTriangulation", 3, FText::FromString("Cannot link edge"));
            }
        }
    }

} //namespace delaunator
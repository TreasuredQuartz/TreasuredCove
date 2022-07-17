// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
namespace Delaunay {

    constexpr size_t INVALID_INDEX =
        TNumericLimits<size_t>::Max();

    /**
    *
    */
    class TREASUREDCOVE_API Point
    {
    public:
        Point(double x, double y) : m_x(x), m_y(y)
        {}
        Point() : m_x(0), m_y(0)
        {}

        double x() const
        {
            return m_x;
        }

        double y() const
        {
            return m_y;
        }

        double magnitude2() const
        {
            return m_x * m_x + m_y * m_y;
        }

        static double determinant(const Point& p1, const Point& p2)
        {
            return p1.m_x * p2.m_y - p1.m_y * p2.m_x;
        }

        static Point vector(const Point& p1, const Point& p2)
        {
            return Point(p2.m_x - p1.m_x, p2.m_y - p1.m_y);
        }

        static double dist2(const Point& p1, const Point& p2)
        {
            Point vec = vector(p1, p2);
            return vec.m_x * vec.m_x + vec.m_y * vec.m_y;
        }

        static bool equal(const Point& p1, const Point& p2, double span)
        {
            double dist = dist2(p1, p2) / span;

            // ABELL - This number should be examined to figure how how
            // it correlates with the breakdown of calculating determinants.
            return dist < 1e-20;
        }

    private:
        double m_x;
        double m_y;
    };

    /**
    *
    */
    class TREASUREDCOVE_API Points
    {
    public:
        using const_iterator = Point const*;

        Points(const TArray<double>& coords) : m_coords(coords)
        {}

        const Point& operator[](size_t offset)
        {
            return reinterpret_cast<const Point&>(
                *(m_coords.GetData() + (offset * 2)));
        };

        Points::const_iterator begin() const
        {
            return reinterpret_cast<const Point*>(m_coords.GetData());
        }
        Points::const_iterator end() const
        {
            return reinterpret_cast<const Point*>(
                m_coords.GetData() + m_coords.Num());
        }
        size_t size() const
        {
            return m_coords.Num() / 2;
        }

    private:
        const TArray<double>& m_coords;
    };

    /**
    *
    */
    class TREASUREDCOVE_API Delaunay {

    public:
        TArray<double> const& coords;
        Points m_points;

        // 'triangles' stores the indices to the 'X's of the input
        // 'coords'.
        TArray<size_t> triangles;

        // 'halfedges' store indices into 'triangles'.  If halfedges[X] = Y,
        // It says that there's an edge from X to Y where a) X and Y are
        // both indices into triangles and b) X and Y are indices into different
        // triangles in the array.  This allows you to get from a triangle to
        // its adjacent triangle.  If the a triangle edge has no adjacent triangle,
        // its half edge will be INVALID_INDEX.
        TArray<size_t> halfedges;

        TArray<size_t> hull_prev;
        TArray<size_t> hull_next;

        // This contains indexes into the triangles array.
        TArray<size_t> hull_tri;
        size_t hull_start;

        inline Delaunay(TArray<double> const& in_coords);
        inline double get_hull_area();
        inline double get_triangle_area();

    private:
        TArray<size_t> m_hash;
        Point m_center;
        size_t m_hash_size;
        TArray<size_t> m_edge_stack;

        inline size_t legalize(size_t a);
        inline size_t hash_key(double x, double y) const;
        inline size_t add_triangle(
            size_t i0,
            size_t i1,
            size_t i2,
            size_t a,
            size_t b,
            size_t c);
        inline void link(size_t a, size_t b);
    };

} //namespace delaunator

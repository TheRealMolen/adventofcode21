#pragma once

#include "harness.h"
#include <ostream>


template<typename T>
struct Pt3
{
    using el_type = T;
    using type = Pt3<el_type>;

    el_type x, y, z;

    Pt3() = default;
    Pt3(el_type _x, el_type _y, el_type _z) : x(_x), y(_y), z(_z)   { /**/ }

    explicit Pt3(const string& str)
    {
        istringstream is(str);
        is >> x >> "," >> y >> "," >> z;
    }

    type& operator+=(const type& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }
    type& operator-=(const type& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }
    type& operator+=(el_type t)
    {
        x += t;
        y += t;
        z += t;
        return *this;
    }
    type& operator-()
    {
        x = -x;
        y = -y;
        z = -z;
        return *this;
    }

    type operator+(const type& other) const
    {
        return { x + other.x, y + other.y, z + other.z };
    }
    type operator-(const type& other) const
    {
        return { x - other.x, y - other.y, z - other.z };
    }

    el_type magSq() const { return x*x + y*y + z*z; }

    auto operator<=>(const type& rhs) const = default;
};

using Pt3i = Pt3<int>;


template<typename T>
ostream& operator<<(ostream& os, const Pt3<T>& pt)
{
    os << pt.x << "," << pt.y << "," << pt.z;
    return os;
}

#pragma once

#include <vector>

inline float toFloat(const double d) { return static_cast<float>(d); }
inline int toInt(const size_t in) { return static_cast<int>(in); }
inline int toInt(const unsigned int in) { return static_cast<int>(in); }

template<class C>inline int numElements(const C& c) { return toInt(c.size()); }

template<class C> inline std::vector<typename C::value_type> toVector(const C& c)
{
    return std::vector<typename C::value_type>(c.begin(), c.end());
}



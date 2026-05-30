#ifndef OOP_MATHUTILS_H
#define OOP_MATHUTILS_H

// Funcție șablon cu sens: plafonează o valoare într-un interval [min, max]
template <typename T>
T clamp(T val, T min, T max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

#endif // OOP_MATHUTILS_H
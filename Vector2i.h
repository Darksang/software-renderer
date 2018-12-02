#ifndef _VECTOR2I_H
#define _VECTOR2I_H

#include <cmath>

struct Vector2i {

    int x, y;

    Vector2i() {
        x = 0;
        y = 0;
    }

    Vector2i(int a, int b) {
        x = a;
        y = b;
    }

    int & operator [](int i) {
		return ((&x)[i]);
	}

    const int & operator [](int i) const {
		return ((&x)[i]);
	}

    Vector2i & operator +=(const Vector2i & v) {
		x += v.x;
		y += v.y;

		return (*this);
	}

    Vector2i & operator -=(const Vector2i & v) {
		x -= v.x;
		y -= v.y;

		return (*this);
	}
};

inline Vector2i operator +(const Vector2i & a, const Vector2i & b) {
	return (Vector2i(a.x + b.x, a.y + b.y));
}

inline Vector2i operator -(const Vector2i & a, const Vector2i & b) {
	return (Vector2i(a.x - b.x, a.y - b.y));
}

#endif
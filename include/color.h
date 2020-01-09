#ifndef COLOR_H
#define COLOR_H

struct Color {
  float r, g, b;
  Color() : r(0), g(0), b(0) {}
  Color(float c) : r(c), g(c), b(c) {}
  Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b) {}
  Color operator*(float f) const { return Color(r * f, g * f, b * f); }
  Color operator*(Vector3 f) const { return Color(r * f.x, g * f.y, b * f.z); }
  Color operator*(Color c) const { return Color(r * c.r, g * c.g, b * c.b); }
  Color operator/(float f) const { return Color(r / f, g / f, b / f); }
  Color operator+(Color c) const { return Color(r + c.r, g + c.g, b + c.b); }
  Color &operator+=(const Color &c) {
    r += c.r, g += c.g, b += c.b;
    return *this;
  }
  Color &operator*=(const Color &c) {
    r *= c.r, g *= c.g, b *= c.b;
    return *this;
  }

  Color &clamp() {
    if (r < 0.0)
      r = 0.0;
    if (g < 0.0)
      g = 0.0;
    if (b < 0.0)
      b = 0.0;
    if (r > 1.0)
      r = 1.0;
    if (g > 1.0)
      g = 1.0;
    if (b > 1.0)
      b = 1.0;
    return *this;
  }
};

#endif

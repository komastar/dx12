#include "WMColor.h"

using namespace WildMini::Graphics;

const WMColor WMColor::transparent =    { 0.0f, 0.0f, 0.0f, 1.0f };
const WMColor WMColor::white =          { 1.0f, 1.0f, 1.0f, 1.0f };
const WMColor WMColor::black =          { 0.0f, 0.0f, 0.0f, 1.0f };
const WMColor WMColor::red =            { 1.0f, 0.0f, 0.0f, 1.0f };
const WMColor WMColor::green =          { 0.0f, 1.0f, 0.0f, 1.0f };
const WMColor WMColor::blue =           { 0.0f, 0.0f, 1.0f, 1.0f };
const WMColor WMColor::cyan =           { 0.0f, 1.0f, 1.0f, 1.0f };
const WMColor WMColor::magenta =        { 1.0f, 0.0f, 1.0f, 1.0f };
const WMColor WMColor::yellow =         { 1.0f, 1.0f, 0.0f, 1.0f };

WildMini::Graphics::WMColor::WMColor()
    : r(0.0f)
    , g(0.0f)
    , b(0.0f)
    , a(0.0f)
{
}

WildMini::Graphics::WMColor::WMColor(float rgba)
    : r(rgba)
    , g(rgba)
    , b(rgba)
    , a(rgba)
{
}

WildMini::Graphics::WMColor::WMColor(float _r, float _g, float _b, float _a)
    : r(_r)
    , g(_g)
    , b(_b)
    , a(_a)
{
}

WMColor WildMini::Graphics::WMColor::Clamp()
{
    r = WildMini::Clamp(r, 0.0f, 1.0f);
    g = WildMini::Clamp(g, 0.0f, 1.0f);
    b = WildMini::Clamp(b, 0.0f, 1.0f);
    a = WildMini::Clamp(a, 0.0f, 1.0f);
    return *this;
}

WMColor WildMini::Graphics::WMColor::operator+(const WMColor& color) const
{
    return WMColor(
        this->r + color.r
        , this->g + color.g
        , this->b + color.b
        , this->a + color.a
    ).Clamp();
}

WMColor WildMini::Graphics::WMColor::operator-(const WMColor& color) const
{
    return WMColor(
        this->r - color.r
        , this->g - color.g
        , this->b - color.b
        , this->a - color.a
    ).Clamp();
}

WMColor WildMini::Graphics::WMColor::operator*(float v) const
{
    return WMColor(
        this->r * v
        , this->g * v
        , this->b * v
        , this->a * v
    ).Clamp();
}

WMColor WildMini::Graphics::WMColor::operator/(float v) const
{
    v = 1.0f / v;
    return WMColor(
        this->r * v
        , this->g * v
        , this->b * v
        , this->a * v
    ).Clamp();
}

bool WildMini::Graphics::WMColor::operator==(const WMColor& color) const
{
    return this->r == color.r
        && this->g == color.g
        && this->b == color.b
        && this->a == color.a;
}

bool WildMini::Graphics::WMColor::operator!=(const WMColor& color) const
{
    return this->r != color.r
        || this->g != color.g
        || this->b != color.b
        || this->a != color.a;
}

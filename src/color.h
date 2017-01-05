/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef COLOR_H
#define COLOR_H

#include <set>
#include <sstream>
#include <clipper/clipper.hpp>

namespace cura {
extern ClipperLib::FollowingZFill colorFill;

class ColorCache;

class Color
{
public:
    float r;
    float g;
    float b;

private:
    Color() : r(0), g(0), b(0) {}
    Color(float red, float green, float blue) : r(red), g(green), b(blue) {}

    friend class ColorCache;
};


static inline const Color *toColor(const ClipperLib::cInt &value) {
    return reinterpret_cast<const Color *>(value);
}
static inline ClipperLib::cInt toClipperInt(const Color *color) {
    return reinterpret_cast<ClipperLib::cInt>(color);
}

// TODO: This goes somewhere else
template<typename T>
std::string toString(const T &value) {
    std::stringstream sstr;
    sstr << value;
    return sstr.str();
}

static inline std::ostream &operator<<(std::ostream &out, const Color &color) {
    return out << "Color(" << color.r << ", " << color.g << ", " << color.b << ")";
}


enum SliceRegionType { srtInfill, srtBorder, srtUnoptimized };

class RegionColoring
{
public:
    RegionColoring(SliceRegionType type, const Color *color) : type(type), color(color) {}

    inline const Color *getColor(const ClipperLib::IntPoint &pt) const {
        switch(type) {
            case srtBorder:
                return color;
            case srtUnoptimized:
                return toColor(pt.Z);
            case srtInfill: default:
                return nullptr;
        }
    }

    inline bool isInfill() {
        return type == srtInfill;
    }

    inline bool operator==(const RegionColoring &other) const {
        if (other.type != type) return false;
        return type != srtBorder || other.color == color;
    }
private:
    SliceRegionType type;
    const Color *color;
    RegionColoring();
};

struct ColorComparator {
    bool operator() (const Color& self, const Color& other) {
        if (self.r != other.r)
            return self.r < other.r;
        if (self.g != other.g)
            return self.g < other.g;
        if (self.b != other.b)
            return self.b < other.b;
        return false;
    }
};

class ColorCache
{
public:
    static ColorCache& inst();
    static const Color* badColor;
    const Color* getColor(const float r, const float g, const float b);

private:
    typedef std::set<Color, ColorComparator> ColorSet;
    typedef ColorSet::iterator ColorIterator;
    
    static ColorCache instance;

    ColorSet cache;
    const ColorIterator createColor(const Color &c);
};

const RegionColoring defaultColoring(srtInfill, ColorCache::badColor);
const RegionColoring unoptimizedColoring(srtUnoptimized, ColorCache::badColor);
}//namespace cura

#endif//COLOR_H

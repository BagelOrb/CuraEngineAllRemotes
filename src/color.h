/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#ifndef COLOR_H
#define COLOR_H

#include <list>
#include <vector>
#include <set>
#include <sstream>
#include <clipper/clipper.hpp>

namespace cura {
extern ClipperLib::FollowingZFill colorFill;

namespace cura {

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

class ColorExtents
{
public:
    typedef std::list<ColorExtent>::iterator iterator;
    typedef std::list<ColorExtent>::const_iterator const_iterator;

    void addExtent(const Color *color, const ClipperLib::IntPoint &p1, const ClipperLib::IntPoint &p2);
    void transferFront(ClipperLib::cInt distance, ColorExtents &other);
    void preMoveExtents(ColorExtents &other);
    void reverse();
    void resize(const ClipperLib::IntPoint &p1, const ClipperLib::IntPoint &p2);
    inline bool canCombine(const ColorExtents &other) {return other.useX == useX;}
    inline ClipperLib::cInt getLength() const {return totalLength;}
    inline ClipperLib::cInt intDistance(const ClipperLib::IntPoint &p1, const ClipperLib::IntPoint &p2) const {
        return abs(useX ? p1.X - p2.X : p1.Y - p2.Y);
    }
    iterator begin() {return extents.begin();}
    iterator end() {return extents.end();}
    const_iterator begin() const {return extents.begin();}
    const_iterator end() const {return extents.end();}
    unsigned int size() const {return extents.size();}
    std::string toString() const;

private:
    ColorExtents(const ClipperLib::IntPoint &from, const ClipperLib::IntPoint &to)
      : totalLength(0),
        useX(abs(to.X - from.X) > abs(to.Y - from.Y)) {}
    ColorExtents(const ColorExtents &other)
      : extents(other.extents),
        totalLength(other.totalLength),
        useX(other.useX) {}
    ColorExtents(bool useX)
      : totalLength(0),
        useX(useX) {}
    std::list<ColorExtent> extents;
    ClipperLib::cInt totalLength;
    bool useX;

    friend class ExtentsManager; // only an ExtentsManager can create a ColorExtents
};

class ColorExtentsRef
{
public:
    ColorExtentsRef(ColorExtents *ref)  : soul(ref) {}
    ColorExtentsRef(ClipperLib::cInt z) : soul(reinterpret_cast<ColorExtents *>(z)) {assert(soul);}
    ClipperLib::cInt toClipperInt() const {return reinterpret_cast<ClipperLib::cInt>(soul);}
    ColorExtents       &operator*()       {return *soul;}
    ColorExtents const &operator*() const {return *soul;}
    ColorExtents       *operator->()       {return soul;}
    ColorExtents const *operator->() const {return soul;}

private:
    ColorExtents *soul;
};

class ExtentsManager : public ClipperLib::FollowingZFill {
public:
    static ExtentsManager& inst();
    ColorExtentsRef create(const ClipperLib::IntPoint &p1, const ClipperLib::IntPoint &p2);
    ColorExtentsRef clone(const ColorExtentsRef &other);
    ColorExtentsRef empty_like(const ColorExtentsRef &proto);
    ~ExtentsManager();

    virtual void OnFinishOffset(ClipperLib::Path &poly) override;
    virtual ClipperLib::cInt Clone(ClipperLib::cInt z) override;
    virtual void ReverseZ(ClipperLib::cInt z) override;
    virtual ClipperLib::cInt StripBegin(ClipperLib::cInt z, const ClipperLib::IntPoint &from, const ClipperLib::IntPoint &to, const ClipperLib::IntPoint &pt);

private:
    static ExtentsManager instance;

    std::vector<ColorExtents *> allocatedExtents;
    ExtentsManager() {}
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

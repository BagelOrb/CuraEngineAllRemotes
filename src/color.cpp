/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include <assert.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include "color.h"
#include "utils/intpoint.h"
#include "utils/logoutput.h"

using std::string;
using std::ostringstream;
using std::endl;
using std::cout;
using ClipperLib::cInt;
using ClipperLib::Path;

namespace cura {
ClipperLib::FollowingZFill colorFill;

// ---------------- class ColorCache --------------------
ColorCache ColorCache::instance;

const Color* ColorCache::badColor = ColorCache::inst().getColor(-1.0f, -1.0f, -1.0f);

ColorCache& ColorCache::inst() {
    return instance;
}

const Color* ColorCache::getColor(const float r, const float g, const float b) {
    Color target(r,g,b);
    ColorIterator elem = cache.find(target);
    if (elem == cache.end())
        elem = createColor(target);
    return &(*elem);
}

const ColorCache::ColorIterator ColorCache::createColor(const Color& c) {
    std::pair<ColorIterator, bool> result = cache.insert(c);
    assert(result.second);
    return result.first;
}

}

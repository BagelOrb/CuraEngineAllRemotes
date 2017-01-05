#include <assert.h>

#include "src/color.h"

using cura::Color;
using cura::ColorCache;

int main(int argc, char** argv) {

    ColorCache& cache = ColorCache::inst();

    const Color* c1 = cache.getColor(0.01f, 0.02f, 0.03f);
    const Color* c2 = cache.getColor(0.01f, 0.04f, 0.03f);

    assert(c1 != c2);

    const Color* c3 = cache.getColor(0.01f, 0.04f, 0.03f);

    assert(c3 == c2);

    printf("Very Success!\n");
}

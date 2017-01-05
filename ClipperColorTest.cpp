#include "libs/clipper/clipper.hpp"
#include "src/color.h"
#include <stdio.h>

using namespace ClipperLib;
using namespace cura;

int main(int argc, char **argv) {
	/**
	 *  Clip this:
	 *         /\ /\
	 *        /  X  \
	 *       /__/_\__\
	 */

	Color colors[] = {
		{0,0,0},
		{1,0,0},
		{0,1,0},
		{0,0,1},
		{0.5,0.5,0},
		{0,0.5,0.5}
	};

	// Input winding must be ccw
	Path p0;
	p0 << IntPoint(0, 0, reinterpret_cast<cInt>(&colors[0]));
	p0 << IntPoint(4, 0, reinterpret_cast<cInt>(&colors[1]));
	p0 << IntPoint(2, 2, reinterpret_cast<cInt>(&colors[2]));

	Path p1;
	p1 << IntPoint(2, 0, reinterpret_cast<cInt>(&colors[3]));
	p1 << IntPoint(6, 0, reinterpret_cast<cInt>(&colors[4]));
	p1 << IntPoint(4, 2, reinterpret_cast<cInt>(&colors[5]));

	Clipper clipper;
	clipper.AddPath(p0, ptSubject, true);
	clipper.AddPath(p1, ptClip, true);
	clipper.ZFillFunction(&flatColorCallback);

	Paths output;
	clipper.Execute(ctUnion, output, pftNonZero, pftNonZero);

	for (Path path : output) {
		printf("\nPath\n");
		for (IntPoint p : path) {
			Color *pColor = reinterpret_cast<Color*>(p.Z);
			if (pColor) {
				printf("(%d, %d): (%f, %f, %f)\n", p.X, p.Y, pColor->r, pColor->g, pColor->b);
			} else {
				printf("(%d, %d): NULL!!\n", p.X, p.Y);
			}
		}
	}

	return 0;
}
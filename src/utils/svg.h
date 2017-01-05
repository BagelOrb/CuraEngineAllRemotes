#ifndef UTILS_SVG_H
#define UTILS_SVG_H

#include <fstream>
#include "polygon.h"
#include "../color.h"

namespace cura {

class SVGBuilder {
public:
	SVGBuilder(const std::string &filename, int canvasWidth = 500) : out(filename), width(canvasWidth - 2*border) {
		out << "<html><body>" << std::endl;
	}

	void drawSVG(Polygons &polys, const RegionColoring &coloring) {
		beginSVG(polys);
		drawPolygons(polys, coloring);
		endSVG();
	}
	void beginSVG(const AABB &size) {
		this->size = size;
		recalculate();
		writeHeader();
	}
	void beginSVG(const Polygons &polys) {
		size.calculate(polys);
		recalculate();
		writeHeader();
	}
	void endSVG() {
		out << "</g></svg>" << std::endl;
	}
	void beginGroup() {
		out << "<g>" << std::endl;
	}
	void endGroup() {
		out << "</g>" << std::endl;
	}
	void drawPolygons(Polygons &polys, const RegionColoring &coloring) {
		beginGroup();
		for (unsigned int c = 0; c < polys.size(); c++) {
			drawPolygon(polys[c], coloring);
		}
		endGroup();
	}
	void drawPolygon(PolygonRef poly, const RegionColoring &coloring) {
		char hexstr[16];
		const Point *prev = &poly.back();
		beginGroup();
        for(unsigned int k = 0; k < poly.size(); k++) {
            //TODO: print regions, not just outlines
            const Color *color = coloring.getColor(poly[k]);
            if (!color || color == ColorCache::badColor)
            	color = ColorCache::inst().getColor(0,0,0);

            int colorInt = (int(color->r*255) << 16) | (int(color->g*255) << 8) | int(color->b*255);
			snprintf(hexstr, sizeof hexstr, "%06X", colorInt);
            
            float startX = float(prev->X  -  size.min.X)/diff.X * width + border;
            float startY = float(prev->Y  -  size.min.Y)/diff.Y * width + border;
            float endX  =  float(poly[k].X - size.min.X)/diff.X * width + border;
            float endY  =  float(poly[k].Y - size.min.Y)/diff.Y * width + border;

            out << "<path marker-mid='url(#MidMarker)' stroke=\"#" << hexstr << "\" ";
            out << "d=\"M "<<startX<<","<<startY<<" L "<<endX<<","<<endY<<"\" />" << std::endl;
            prev = &poly[k];
        }
        endGroup();
	}
	void write(const std::string &html) {
		out << html << std::endl;
	}

	~SVGBuilder() {
		out << "</body></html>" << std::endl;
	}
private:
	void recalculate() {
		diff = size.max - size.min;
		height = width * diff.Y / diff.X;
	}
	void writeHeader() {
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style='width:" << (width + 2*border) << "px;height:" << (height + 2*border) << "px;background-color:gray;'>" << std::endl;
        out << "<marker id='MidMarker' viewBox='0 0 10 10' refX='5' refY='5' markerUnits='strokeWidth' markerWidth='10' markerHeight='10' stroke='lightblue' stroke-width='2' fill='none' orient='auto'>" << std::endl;
        out << "<path d='M 0 0 L 10 5 M 0 10 L 10 5'/>" << std::endl;
        out << "</marker>" << std::endl;
        out << "<g fill-rule='evenodd' style=\"fill: gray; stroke:black;stroke-width:1\">" << std::endl;
	}
	std::ofstream out;
	AABB size;
	Point diff;
	int width;
	int height; // set from width based on size, in recalculate()
	const int border = 10;
};

} // namespace cura
#endif//UTILS_SVG_H
/** Copyright (C) 2013 David Braam - Released under terms of the AGPLv3 License */
#include "inset.h"
#include "polygonOptimizer.h"

namespace cura {

Polygons DelaminationPrevention(Polygons& p,int offset)
{
    // Seul le premier polygône est un mur extérieur, les autres sont des murs intérieurs, on ne les change pas
    int64_t dmax = 7000; // Distance maximale en microns entre deux décrochements
    int64_t declen = offset; // Longueur de chaque décrochement en microns
    Polygons ret;
    PolygonRef pext = p[0];
    Polygon np;
    Point p0;
    Point pprev;
    for (unsigned int i=0;i<pext.size();i++)
    {
        if (i == 0)
        {
            p0 = pext[i];
            pprev = pext[i];
        }
        while (vSize2(pext[i] - p0) > dmax * dmax)
        {
            // Je dois trouver l'intersection entre le le segment [ pprev , pext[i] ] et le cercle de centre p0 et de rayon dmax
            double a = (pext[i].X - pprev.X)*(pext[i].X - pprev.X) + (pext[i].Y - pprev.Y)*(pext[i].Y - pprev.Y);
            double b = 2.0*(pext[i].X - pprev.X)*(pprev.X - p0.X) + 2.0*(pext[i].Y - pprev.Y)*(pprev.Y - p0.Y);
            double c = (pprev.X - p0.X)*(pprev.X - p0.X) + (pprev.Y - p0.Y)*(pprev.Y - p0.Y) - dmax*dmax;
            double delta = b*b - 4.0*a*c;
            double lambda = (-b + sqrt(delta)) / 2.0 / a;
            if (lambda < -0.001 || lambda > 1.001)
                lambda = (-b - sqrt(delta)) / 2.0 / a;
            Point p1;
            p1.X = pprev.X + lambda * (pext[i].X - pprev.X);
            p1.Y = pprev.Y + lambda * (pext[i].Y - pprev.Y);
            // Le point p1 est sur le segment [ pprev , pext[i] ] et à distance dmax de p0 
            Point vect = p1 - pprev;
            np.add(p1 - normal(vect,offset/2));
            np.add(p1 + normal(crossZ(vect),declen));
            np.add(p1);
            p0 = p1;
            pprev = p1;
        }
        np.add(pext[i]);
        pprev = pext[i];
    }
    ret.add(np);
    for (unsigned int nPoly=1;nPoly<p.size();nPoly++)
    {
        ret.add(p[nPoly]);
    }
    return ret;
}

void generateInsets(SliceLayerPart* part, int offset, int insetCount)
{
    part->combBoundery = part->outline.offset(-offset);
    if (insetCount == 0)
    {
        part->insets.push_back(part->outline);
        return;
    }
    
    for(int i=0; i<insetCount; i++)
    {
        part->insets.push_back(Polygons());
        part->insets[i] = DelaminationPrevention(part->outline,offset).offset(-offset * i - offset/2);
        optimizePolygons(part->insets[i]);
        if (part->insets[i].size() < 1)
        {
            part->insets.pop_back();
            break;
        }
    }
}

void generateInsets(SliceLayer* layer, int offset, int insetCount)
{
    for(unsigned int partNr = 0; partNr < layer->parts.size(); partNr++)
    {
        generateInsets(&layer->parts[partNr], offset, insetCount);
    }
    
    //Remove the parts which did not generate an inset. As these parts are too small to print,
    // and later code can now assume that there is always minimal 1 inset line.
    for(unsigned int partNr = 0; partNr < layer->parts.size(); partNr++)
    {
        if (layer->parts[partNr].insets.size() < 1)
        {
            layer->parts.erase(layer->parts.begin() + partNr);
            partNr -= 1;
        }
    }
}

}//namespace cura

#include <stdlib.h>
#include <stdio.h>
#include "cubic.h"

Cubic::Cubic() : nbPoints(0)
{
    size = CUBIC_DEFAULT_POINTS;
    points = (Point *)malloc(size*sizeof(Point));
    polynoms = (Polynom *)malloc(size*sizeof(Polynom));
}

Cubic::~Cubic()
{
    free(points);
    free(polynoms);
}
        
void Cubic::checkSize()
{
    if (nbPoints+1 >= size) {
        size *= 2;
        points = (Point *)realloc(points, size*sizeof(Point));
        polynoms = (Polynom *)realloc(polynoms, size*sizeof(Polynom));
    }
}

void Cubic::clear()
{
    nbPoints = 0;
}

float Cubic::getXMax()
{
    if (nbPoints == 0) {
        return 0.0;
    }

    return points[nbPoints-1].x;
}

void Cubic::addPoint(float x, float y, float yp)
{
    checkSize();

    Point point;
    point.x = x;
    point.y = y;
    point.yp = yp;
    points[nbPoints] = point;

    if (nbPoints > 0) {
        Point previous = points[nbPoints-1];
        Polynom p;
        p.a = 2*previous.y + previous.yp - 2*y + yp;
        p.b = -3*previous.y - 2*previous.yp + 3*y - yp;
        p.c = previous.yp;
        p.d = previous.y;
        polynoms[nbPoints-1] = p;
    }

    nbPoints++;
}

float Cubic::get(float x)
{
    int i;
    
    for (i=0; i<nbPoints; i++) {
        if (points[i].x >= x) {
            break;
        }
    }

    if (i == 0 || i == nbPoints) {
        return 0.0;
    }

    Point A = points[i-1];
    Point B = points[i];
    Polynom f = polynoms[i-1];
    x = (x-A.x)/(B.x-A.x);

    float tx = x;
    float result = f.d;
    result += f.c * tx;
    tx *= x;
    result += f.b * tx;
    tx *= x;
    result += f.a * tx;

    return result;
}

float Cubic::getMod(float x)
{
    float maxX = getXMax();

    if (x < 0.0 || x > maxX) {
        x -= maxX*(int)(x/maxX);
    }
    
    return get(x);
}


#undef POINT_X
#undef POINT_Y

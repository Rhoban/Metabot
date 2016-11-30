#ifndef _ROBOTCAMPUS_CUBIC_H
#define _ROBOTCAMPUS_CUBIC_H

#ifndef CUBIC_DEFAULT_POINTS
#define CUBIC_DEFAULT_POINTS 16
#endif

/**
 * Cubic splines
 */
class Cubic
{
    public:
        Cubic();
        ~Cubic();

        /**
         * Add a point (x, y) to the function
         */
        void addPoint(float x, float y, float yp);

        /**
         * Gets the xMax of the function
         */
        float getXMax();

        /**
         * Get the value for the given x
         */
        float get(float x);

        /**
         * Gets the value for the given x, modulo the size of the function
         */
        float getMod(float x);

        /**
         * Checks if the function can contain one more element
         */
        void checkSize();

        void clear();

    protected:
        struct Point
        {
            float x, y, yp;
        };

        /**
         * Cubic points
         */
        Point *points;

        /**
         * Number of points
         */
        int nbPoints;

        /**
         * Array size
         */
        int size;

        struct Polynom
        {
            float a, b, c, d;
        };

        /**
         * The nth item of this array is the slope between the nth point and the
         * n+1th point
         */
        Polynom *polynoms;
};

#endif

#ifndef SPLINE_H
#define SPLINE_H

#include "Point.h"

struct Spline {
	int numControlPoints;
	struct Point *points;
};

void createControlMatrix(int index, Spline spline, float *matrixOut) {
	if (index > 0 && index < spline.numControlPoints - 1) {
		Point p;
		for (int i = 0; i < 4; i++) {
			p = spline.points[index + i - 1];
			matrixOut[3 * i] = p.x;
			matrixOut[(3 * i) + 1] = p.y;
			matrixOut[(3 * i) + 2] = p.z;
		}
	}
}

void interpolatePoints(Point p1, Point p2, std::vector<Point> &pOut, 
	float *basisMatrix, float *controlMatrix, float &maxDistance, int &index, float u, float uMin, float uMax, float* uVector, float* uBasis) {
	//std::cout << p1.x << ", " << p1.y << ", " << p1.z << std::endl;
	if (u > 1.0f)
	{
		std::cout << std::endl;
	}
	float dist = p1.Distance(p2);
	if (dist >= maxDistance) {
		uVector[0] = u * u * u;
		uVector[1] = u * u;
		uVector[2] = u;
		uVector[3] = 1;
		uBasis[0] = 0;
		uBasis[1] = 0;
		uBasis[2] = 0;
		uBasis[3] = 0;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				uBasis[i] = uBasis[i] + uVector[j] * basisMatrix[(j * 4) + i];
			}
		}
		Point midPoint = Point();
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 4; j++) {
				if (i == 0)
				{
					midPoint.x = midPoint.x + uBasis[j] * controlMatrix[(j * 3) + i];
				}
				else if (i == 1)
				{
					midPoint.y = midPoint.y + uBasis[j] * controlMatrix[(j * 3) + i];
				}
				else
				{
					midPoint.z = midPoint.z + uBasis[j] * controlMatrix[(j * 3) + i];
				}
			}
		}
		interpolatePoints(p1, midPoint, pOut, basisMatrix, controlMatrix, maxDistance, index, (uMax - u) / 2 + uMin, uMin, u, uVector, uBasis);
		pOut.push_back(midPoint);
		index++;
		interpolatePoints(midPoint, p2, pOut, basisMatrix, controlMatrix, maxDistance, index, (uMax + u) / 2, u, uMax, uVector, uBasis);

	}

}

#endif
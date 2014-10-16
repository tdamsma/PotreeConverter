

#ifndef QUANTIZATION_H
#define QUANTIZATION_H


#include <math.h>
#include <algorithm>

#include "Vector3.h"

using std::min;
using std::max;
using std::log;
using std::ceil;

struct Quantization{

	Vector3<double> offset;
	Vector3<double> scale;

	Quantization(){

	}

	const double x(unsigned int X) const {
		return X * scale.x + offset.x;
	}

	const double y(unsigned int Y) const {
		return Y * scale.y + offset.y;
	}

	const double z(unsigned int Z) const {
		return Z * scale.z + offset.z;
	}

	const unsigned int X(double x) const {
		return (int)((x - offset.x) / scale.x);
	}

	const unsigned int Y(double y) const {
		return (int)((y - offset.y) / scale.y);
	}

	const unsigned int Z(double z) const {
		return (int)((z - offset.z) / scale.z);
	}

};



#endif
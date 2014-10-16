
#ifndef POINT_H
#define POINT_H

#include "Vector3.h"
#include "Quantization.hpp"

#include <iostream>

using std::ostream;

class Point{
private:
	unsigned int _X;
	unsigned int _Y; 
	unsigned int _Z;

public:
	Quantization *q;

	unsigned short intensity;
	unsigned char classification;

	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;

	Point(){
		this->q = new Quantization();

		setXYZ(0, 0, 0);

		this->intensity = 0;
		this->classification = 0;

		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	Point(Quantization *q){
		this->q = q;

		setXYZ(0, 0, 0);

		this->intensity = 0;
		this->classification = 0;

		this->r = 0;
		this->g = 0;
		this->b = 0;
		this->a = 255;
	}

	Point(Quantization *q, int X, int Y, int Z, unsigned char r, unsigned char g, unsigned char b){
		this->q = q;

		setXYZ(X, Y, Z);

		this->intensity = 0;
		this->classification = 0;

		this->r = r;
		this->g = g;
		this->b = b;
		this->a = 255;
	}

	Point(Quantization *q, int X, int Y, int Z){
		this->q = q;

		setXYZ(X, Y, Z);

		this->intensity = 0;
		this->classification = 0;

		this->r = 255;
		this->g = 255;
		this->b = 255;
		this->a = 255;
	}

	void setXYZ(unsigned int X, unsigned int Y, unsigned int Z){
		this->_X = X;
		this->_Y = Y;
		this->_Z = Z;
	}

	void setxyz(double x, double y, double z){
		this->_X = q->X(x);
		this->_Y = q->Y(y);
		this->_Z = q->Z(z);
	}

	const unsigned int X(){ return _X; }
	const unsigned int Y(){ return _Y; }
	const unsigned int Z(){ return _Z; }

	const double x() const {
		return q->x(_X);
	}

	const double y() const {
		return q->y(_Y);
	}

	const double z() const {
		return q->z(_Z);
	}

	const Vector3<double> xyz() const {
		return Vector3<double>(x(), y(), z());
	}


	const double distanceTo(const Point &p) const {
		return Vector3<double>(p.x() - x(), p.y() - y(), p.z() - z()).length(); 
	}

	const double squaredDistanceTo(const Point &p) const {
		return Vector3<double>(p.x() - x(), p.y() - y(), p.z() - z()).squaredLength(); 
	}

	friend ostream &operator<<( ostream &output,  const Point &value ){ 
		output << "[" << value.x() << ", " << value.y() << ", " << value.z() << "]" ;
		return output;            
	}

};


#endif
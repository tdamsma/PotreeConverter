

#ifndef POTPOINTREADER_H
#define POTPOINTREADER_H

#include <string>
#include <iostream>
#include <vector>
#include <memory>

#include "Point.h"
#include "PointReader.h"
#include "PointAttributes.hpp"
#include "BitReader.hpp"
#include "Quantization.hpp"

using std::string;

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;
using std::shared_ptr;

class POTPointReader : public PointReader{
private:
	AABB aabb;
	string path;
	vector<string> files;
	vector<string>::iterator currentFile;
	ifstream *reader;
	BitReader *bitReader;
	PointAttributes attributes;
	Point point;
	int pointCount;
	int pointsRead;
	Vector3<long long> min;
	Vector3<long long> max;
	Vector3<int> bits;
	Quantization q;


public:

	POTPointReader(string path);

	~POTPointReader();

	bool readNextPoint();

	Point getPoint();

	AABB getAABB();

	long numPoints();

	void close();

	Vector3<double> getScale();
};

#endif
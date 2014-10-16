

#ifndef LASPOINTREADER_H
#define LASPOINTREADER_H

#include <string>
#include <iostream>
#include <vector>

#include <liblas/liblas.hpp>

#include "Point.h"
#include "PointReader.h"
#include "Quantization.hpp"

using std::string;

using std::ifstream;
using std::cout;
using std::endl;
using std::vector;


class LIBLASReader{
public:

	ifstream stream;
	liblas::Reader reader;

	LIBLASReader(string path)
		: stream(path, std::ios::in | std::ios::binary)
		, reader(liblas::ReaderFactory().CreateWithStream(stream))
	{

	}

	~LIBLASReader(){
		if(stream.is_open()){
			stream.close();
		}
	}

	void close(){
		stream.close();
	}

	AABB getAABB(){
		AABB aabb;

		const liblas::Header &header = reader.GetHeader();

		Vector3<double> min = Vector3<double>(header.GetMinX(), header.GetMinY(), header.GetMinZ());
		Vector3<double> max = Vector3<double>(header.GetMaxX(), header.GetMaxY(), header.GetMaxZ());
		aabb.update(min);
		aabb.update(max);

		return aabb;
	}

	Vector3<double> getScale(){
		Vector3<double> scale(0);

		const liblas::Header &header = reader.GetHeader();

		scale.x = header.GetScaleX();
		scale.y = header.GetScaleY();
		scale.z = header.GetScaleZ();

		return scale;
	}

	Vector3<double> getOffset(){
		Vector3<double> offset(0);

		const liblas::Header &header = reader.GetHeader();

		offset.x = header.GetOffsetX();
		offset.y = header.GetOffsetY();
		offset.z = header.GetOffsetZ();

		return offset;
	}

};


class LASPointReader : public PointReader{
private:
	AABB aabb;
	string path;
	LIBLASReader *reader;
	vector<string> files;
	vector<string>::iterator currentFile;
	Quantization q;

public:

	LASPointReader(string path);

	~LASPointReader();

	bool readNextPoint();

	Point getPoint();

	AABB getAABB();

	long numPoints();

	void close();

	Vector3<double> getScale();
};

#endif
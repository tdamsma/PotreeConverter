
#ifndef POTPOINTWRITER_H
#define POTPOINTWRITER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "AABB.h"
#include "PointAttributes.hpp"
#include "PointWriter.hpp"
#include "Quantization.hpp"
#include "BitWriter.hpp"
#include "definitions.hpp"

using std::string;
using std::vector;
using std::fstream;
using std::ios;
using std::stringstream;



class POTPointWriter : public PointWriter{

public:
	string file;
	PointAttributes attributes;
	fstream *writer;
	vector<Point> points;
	Quantization q;
	AABB boundingBox;

	POTPointWriter(string file) {
		this->file = file;
		attributes.add(PointAttribute::POSITION_CARTESIAN);
		attributes.add(PointAttribute::COLOR_PACKED);

		q.scale = Vector3<double>(0.01);
	}

	POTPointWriter(string file, Quantization q) {
		this->file = file;
		attributes.add(PointAttribute::POSITION_CARTESIAN);
		attributes.add(PointAttribute::COLOR_PACKED);

		this->q = q;
	}

	POTPointWriter(string file, PointAttributes attributes, Quantization q) {
		this->file = file;
		this->attributes = attributes;
		this->q = q;
	}

	~POTPointWriter(){
		close();
	}

	void write(const Point &point){
		points.push_back(Point(point));

		boundingBox.update(point.xyz());
	}

	void close(){
		writer = new fstream(file, ios::out | ios::binary);

		vector<Vector3<long long>> diff;

		Vector3<long long> min(std::numeric_limits<long long>::max());
		Vector3<long long> max(std::numeric_limits<long long>::min());

		for(int i = 1; i < points.size(); i++){
			long long dx = (long long)points[i].X() - (long long)points[i-1].X();
			long long dy = (long long)points[i].Y() - (long long)points[i-1].Y();
			long long dz = (long long)points[i].Z() - (long long)points[i-1].Z();

			diff.push_back(Vector3<long long>(dx, dy, dz));
	
			min.x = std::min(min.x, dx);
			min.y = std::min(min.y, dy);
			min.z = std::min(min.z, dz);
						   
			max.x = std::max(max.x, dx);
			max.y = std::max(max.y, dy);
			max.z = std::max(max.z, dz);
		}

		//cout << "min: " << min << endl;
		//cout << "max: " << max << endl;
		//cout << "bbox: " << boundingBox << endl;

		Vector3<int> bits;
		bits.x = (max.x - min.x) == 0 ? 0 : ceil(log(max.x - min.x) / log(2));
		bits.y = (max.y - min.y) == 0 ? 0 : ceil(log(max.y - min.y) / log(2));
		bits.z = (max.z - min.z) == 0 ? 0 : ceil(log(max.z - min.z) / log(2));

		// write header
		string header = getHeader(min, max, bits, q);
		unsigned int headerSize = header.size();
		writer->write(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
		writer->write(header.c_str(), headerSize);

		
		BitWriter *bitWriter = new BitWriter(writer);

		// write first point without bit encoding
		//cout << "out: " << points[0].X() << ", " << points[0].Y() << ", " << points[0].Z() << endl;
		bitWriter->write(points[0].X(), 32);
		bitWriter->write(points[0].Y(), 32);
		bitWriter->write(points[0].Z(), 32);

		bitWriter->write(points[0].r, 8);
		bitWriter->write(points[0].g, 8);
		bitWriter->write(points[0].b, 8);

		// write remaining points encoded
		for(int i = 1; i < points.size(); i++){
			Point p = points[i];

			bitWriter->write(diff[i-1].x - min.x, bits.x);
			bitWriter->write(diff[i-1].y - min.y, bits.y);
			bitWriter->write(diff[i-1].z - min.z, bits.z);

			bitWriter->write(p.r, 8);
			bitWriter->write(p.g, 8);
			bitWriter->write(p.b, 8);	
		}

		bitWriter->close();
		delete bitWriter;

		writer->close();
		delete writer;
		writer = NULL;
	}

	string getHeader(Vector3<long long> min, Vector3<long long> max, Vector3<int> bits, Quantization q){
		stringstream ss;

		AABB &bb = boundingBox;

		ss << "{\n";
		ss << "\t\"version\": \"" << POTREE_FORMAT_VERSION << "\",\n";
		ss << "\t\"pointcount\": " << points.size() << ",\n";
		ss << "\t\"boundingBox\": {\n";
		ss << "\t\t\"min\": [" << bb.min.x << ", " << bb.min.y << ", " << bb.min.z << "],\n";
		ss << "\t\t\"max\": [" << bb.max.x << ", " << bb.max.y << ", " << bb.max.z << "]\n";
		ss << "\t},\n";
		ss << "\t\"attributes\": [\n";
		ss << "\t\t{\"name\": \"POSITION_CARTESIAN\",\n";
		ss << "\t\t\t\"encoding\": {\n";
		ss << "\t\t\t\t\"type\": \"difference\",\n";
		ss << "\t\t\t\t\"min\": [" << min.x << ", " << min.y << ", " << min.z << "],\n";
		ss << "\t\t\t\t\"max\": [" << max.x << ", " << max.y << ", " << max.z << "],\n";
		ss << "\t\t\t\t\"bits\": [" << bits.x << ", " << bits.y << ", " << bits.z << "]},\n";
		ss << "\t\t\t\"scale\": [" << q.scale.x << ", " << q.scale.y << ", " << q.scale.z << "],\n";
		ss << "\t\t\t\"offset\": [" << q.offset.x << ", " << q.offset.y << ", " << q.offset.z << "]\n";
		ss << "\t\t},\n";
		ss << "\t\t{\"name\": \"RGB\"}\n";
		ss << "\t]\n";
		ss << "}\n";

		return ss.str();

	}

};

#endif



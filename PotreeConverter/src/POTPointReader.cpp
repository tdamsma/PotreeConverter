




#include "POTPointReader.hpp"


#include <boost/regex.hpp>
#include "boost/assign.hpp"
#include "boost/algorithm/string.hpp"
#include <boost/filesystem.hpp>

#include "JSON.h"

using std::ios;
using namespace boost::assign;
using boost::split;
using boost::is_any_of;
using std::stod;
using std::stoi;
using std::stoll;
using boost::iends_with;
using boost::filesystem::is_regular_file;
using boost::filesystem::file_size;




POTPointReader::POTPointReader(string path){
	this->path = path;
	pointsRead = 0;

	reader = new ifstream(path, ios::in | ios::binary);
	//bitReader = new BitReader(reader);

	fs::path p(path);
	int fileSize = fs::file_size(p);

	char *buffer = new char[fileSize];
	reader->read(buffer, fileSize);

	int headerSize = 0;
	string header;

	{ // read header size
		reader->read(buffer, 4);
		headerSize = *reinterpret_cast<int*>(buffer);
	}

	{ // read header
		header = string(buffer + 4, buffer + 4 + headerSize);
	}

	buffer = buffer + 4 + headerSize;
	bitReader = new BitReader(buffer);

	JSONValue *vHeader = JSON::Parse(std::wstring(header.begin(), header.end()).c_str());
	JSONObject root = vHeader->AsObject();
	pointCount = (int)root[L"pointcount"]->AsNumber();

	JSONObject boundingBox = root[L"boundingBox"]->AsObject();
	JSONArray bbMin = boundingBox[L"min"]->AsArray();
	JSONArray bbMax = boundingBox[L"max"]->AsArray();

	JSONArray attributes = root[L"attributes"]->AsArray();
	JSONObject aPosition = attributes[0]->AsObject();
	JSONObject aRGB = attributes[1]->AsObject();
	
	JSONArray pScale = aPosition[L"scale"]->AsArray();
	JSONArray pOffset = aPosition[L"offset"]->AsArray();

	JSONObject encoding = aPosition[L"encoding"]->AsObject();
	std::wstring type = encoding[L"type"]->AsString();
	JSONArray eMin = encoding[L"min"]->AsArray();
	JSONArray eMax = encoding[L"max"]->AsArray();
	JSONArray eBits = encoding[L"bits"]->AsArray();
	
	Vector3<double> bMin(bbMin[0]->AsNumber(), bbMin[1]->AsNumber(), bbMin[2]->AsNumber());
	Vector3<double> bMax(bbMax[0]->AsNumber(), bbMax[1]->AsNumber(), bbMax[2]->AsNumber());
	
	min.x = (long long)eMin[0]->AsNumber();
	min.y = (long long)eMin[1]->AsNumber();
	min.z = (long long)eMin[2]->AsNumber();
					   
	max.x = (long long)eMax[0]->AsNumber();
	max.y = (long long)eMax[1]->AsNumber();
	max.z = (long long)eMax[2]->AsNumber();
							
	bits.x =(int)eBits[0]->AsNumber();
	bits.y =(int)eBits[1]->AsNumber();
	bits.z =(int)eBits[2]->AsNumber();
	
	q.scale.x = pScale[0]->AsNumber();
	q.scale.y = pScale[1]->AsNumber();
	q.scale.z = pScale[2]->AsNumber();

	q.offset.x = pOffset[0]->AsNumber();
	q.offset.y = pOffset[1]->AsNumber();
	q.offset.z = pOffset[2]->AsNumber();

	//cout << "pointcount: " << pointCount << endl;
	//cout << "min:"  << min << endl;
	//cout << "max:"  << max << endl;
	//cout << "bbmin:"  << bMin << endl;
	//cout << "bbmax:"  << bMax << endl;
	//cout << "bits:"  << bits << endl;
	//cout << "scale:"  << q.scale << endl;
	//cout << "offset:"  << q.offset << endl;

	point.q = &q;
}



POTPointReader::~POTPointReader(){

}


bool POTPointReader::readNextPoint(){


	if(pointsRead == 0){
		int X = bitReader->read(32);
		int Y = bitReader->read(32);
		int Z = bitReader->read(32);

		int r = bitReader->read(8);
		int g = bitReader->read(8);
		int b = bitReader->read(8);

		point.setXYZ(X, Y, Z);

		point.r = r;
		point.g = g;
		point.b = b;

		pointsRead++;

		return true;
	}else if(pointsRead < pointCount){

		int X = bitReader->read(bits.x) + min.x + point.X();
		int Y = bitReader->read(bits.y) + min.y + point.Y();
		int Z = bitReader->read(bits.z) + min.z + point.Z();

		int r = bitReader->read(8);
		int g = bitReader->read(8);
		int b = bitReader->read(8);

		point.setXYZ(X, Y, Z);

		point.r = r;
		point.g = g;
		point.b = b;

		pointsRead++;

		return true;
	}else{
		return false;
	}
}


Point POTPointReader::getPoint(){
	return point;
}

AABB POTPointReader::getAABB(){
	return aabb;
}

long POTPointReader::numPoints(){
	return pointCount;
}

void POTPointReader::close(){
	
	if(reader != NULL){
		delete bitReader;
		bitReader = NULL;

		reader->close();
		delete reader;
		reader = NULL;
	}
}

Vector3<double> POTPointReader::getScale(){
	return Vector3<double>();
}













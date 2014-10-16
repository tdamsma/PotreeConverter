
#ifndef BITWRITER_H
#define BITWRITER_H

#include <fstream>

using std::fstream;

class BitWriter{
public:

	fstream* fout;
	int buffer;
	int bitOffset;

	BitWriter(fstream *fout){
		this->fout = fout;

		buffer = 0;
		bitOffset = 0;
	}

	void write(int value, int bitcount){
		
		if(bitOffset + bitcount <= 32){
			int shiftlength = (32 - bitcount) - bitOffset;
			buffer = buffer | (value << shiftlength);

			bitOffset += bitcount;
		}else{
			int shiftlength = (bitOffset + bitcount) - 32;
			
			buffer = buffer | (value >> shiftlength);
			fout->write(reinterpret_cast<char*>(&buffer), 4);

			buffer = 0;
			bitOffset = 0;

			buffer = buffer | (value << (32 - shiftlength));

			bitOffset += shiftlength;
		}

		if(bitOffset == 32){
			fout->write(reinterpret_cast<char*>(&buffer), 4);

			bitOffset = 0;
			buffer = 0;
		}
		
	}

	void flush(){
		fout->flush();
	}

	void close(){
		if(bitOffset > 0){
			fout->write(reinterpret_cast<char*>(&buffer), 4);
		}

		fout->close();
	};

};


#endif
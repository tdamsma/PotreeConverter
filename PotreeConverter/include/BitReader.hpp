
#ifndef BITREADER_H
#define BITREADER_H

class BitReader{
public:

	char *data;
	int offset;
	int bitOffset;
	unsigned int value;

	BitReader(char *data){
		this->data = data;
		offset = 0;
		bitOffset = 0;
		value = 0;

		value = *reinterpret_cast<int*>(data + offset);
	}

	int read(int bitcount){
		
		if(bitOffset + bitcount <= 32){
			int unpacked = (value << bitOffset) >> (32 - bitcount);

			bitOffset += bitcount;

			if(bitOffset == 32){
				bitOffset = 0;
				offset += 4;
				value = *reinterpret_cast<int*>(data + offset);
			}

			return unpacked;
		}else{
			int unpacked = (value << bitOffset) >> (32 - bitcount);
			int remainingLength = (bitOffset + bitcount) - 32;

			bitOffset = 0;
			offset += 4;
			value = *reinterpret_cast<int*>(data + offset);

			unpacked = unpacked | (value >> (32 - remainingLength));

			bitOffset += remainingLength;

			return unpacked;
		}

		
	}

};


#endif
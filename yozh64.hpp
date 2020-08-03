#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <unordered_map>

typedef std::string string;

class yozh64 {
	unsigned char alphabet[64] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};
	std::unordered_map<unsigned char, unsigned int> deAlphabet = {{'A', 0}, {'B', 1}, {'C', 2}, {'D', 3}, {'E', 4}, {'F', 5}, {'G', 6}, {'H', 7}, {'I', 8}, {'J', 9}, {'K', 10}, {'L', 11}, {'M', 12}, {'N', 13}, {'O', 14}, {'P', 15}, {'Q', 16}, {'R', 17}, {'S', 18}, {'T', 19}, {'U', 20}, {'V', 21}, {'W', 22}, {'X', 23}, {'Y', 24}, {'Z', 25}, {'a', 26}, {'b', 27}, {'c', 28}, {'d', 29}, {'e', 30}, {'f', 31}, {'g', 32}, {'h', 33}, {'i', 34}, {'j', 35}, {'k', 36}, {'l', 37}, {'m', 38}, {'n', 39}, {'o', 40}, {'p', 41}, {'q', 42}, {'r', 43}, {'s', 44}, {'t', 45}, {'u', 46}, {'v', 47}, {'w', 48}, {'x', 49}, {'y', 50}, {'z', 51}, {'0', 52}, {'1', 53}, {'2', 54}, {'3', 55}, {'4', 56}, {'5', 57}, {'6', 58}, {'7', 59}, {'8', 60}, {'9', 61}, {'+', 62}, {'/', 63}};
	unsigned int dataSize;
	unsigned char * data;
	unsigned int encodedDataSize;
	unsigned char * encodedData;
	unsigned int decodedDataSize;
	unsigned char * decodedData;
	int mode = -1;
public:
	yozh64(string data_) {
		dataSize = data_.size();
		data = new unsigned char[dataSize];
		for (int i = 0; i < dataSize; i++)
			data[i] = data_[i];
	}
	yozh64(unsigned char * data_, unsigned int dataSize_) {
		dataSize = dataSize_;
		data = new unsigned char[dataSize];
		for (int i = 0; i < dataSize; i++)
			data[i] = data_[i];
	}
	void encode() {
		mode = 1;
		unsigned int charSize = sizeof(unsigned char)*8;
		encodedDataSize = (unsigned int)ceil(1.0*dataSize*charSize/6.0);
		encodedData = new unsigned char[encodedDataSize];
		std::fill(encodedData, encodedData+encodedDataSize, 0);
		unsigned int _masks[6] = {0xfc, 0x3f, 0xf0, 0x0f, 0xc0, 0x03};
		for (int i = 0, j = 0; i < encodedDataSize; i++) {
			if (i % 4 == 0) {
				encodedData[i] = alphabet[(data[j]&_masks[0]) >> 2];
			} else if (i % 4 == 1) {
				unsigned int _tmp = (data[j]&_masks[5]) << 4;
				if (j < dataSize-1) {
					_tmp |= ((data[j+1]&_masks[2]) >> 4);
				}
				encodedData[i] = alphabet[_tmp];
				j++;
			} else if (i % 4 == 2) {
				unsigned int _tmp = (data[j]&_masks[3]) << 2;
				if (j < dataSize-1) {
					_tmp |= ((data[j+1]&_masks[4]) >> 6);
				}
				encodedData[i] = alphabet[_tmp];
				j++;
			} else {
				encodedData[i] = alphabet[data[j]&_masks[1]];
				j++;
			}
		}
		return;
	}

	void decode() {
		mode = 0;
		unsigned int charSize = sizeof(unsigned char)*8;
		decodedDataSize = (dataSize * 6) / charSize;
		decodedData = new unsigned char [decodedDataSize];
		std::fill(decodedData, decodedData+decodedDataSize, 0);
		for (int i = 0, j = 0; i < dataSize; i++) {
			if (i % 4 == 0) {
				decodedData[j] |= (deAlphabet[data[i]] << 2);
			} else if (i % 4 == 1) {
				unsigned int _tmp = deAlphabet[data[i]];
				decodedData[j] |= (_tmp >> 4);
				if (j < decodedDataSize - 1) {
					decodedData[j + 1] |= (_tmp << 4);
				}
				j++;
			} else if (i % 4 == 2) {
				unsigned int _tmp = deAlphabet[data[i]];
				decodedData[j] |= (_tmp >> 2);
				if (j < decodedDataSize - 1) {
					decodedData[j + 1] |= (_tmp << 6);
				}
				j++;
			} else {
				decodedData[j] |= deAlphabet[data[i]];
				j++;
			}
		}
		return;
	}

	string getResult() {
		if (mode == -1) throw std::runtime_error("YOZH64 Error cannot convert unmanipulated data to string!");
		unsigned char * resAr;
		unsigned int resArSize;
		if (mode)
			resAr = encodedData, resArSize = encodedDataSize;
		else
			resAr = decodedData, resArSize = decodedDataSize;
		string result;
		result.reserve(resArSize);
		for (int i = 0; i < resArSize; i++)
			result += resAr[i];
		return result;
	}
};
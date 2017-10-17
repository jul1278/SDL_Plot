// CsvImport.h
#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>

// DateTimePricePair
struct DateTimePricePair {
	unsigned short year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	unsigned int millisec;

	unsigned int quote; 

	DateTimePricePair() {
		memset(this, 0, sizeof(DateTimePricePair)); 
	}
};


//---------------------------------------------------------------------------------------------------------------------
// Name: FastParse
// Desc:
//---------------------------------------------------------------------------------------------------------------------
void FastParse(const char* str, DateTimePricePair* dateTimePricePair) {
	char temp[32]; 

	uint16_t strLen = strlen(str); 
	uint8_t iStr = 0; 
	uint8_t i = 0; 
	 
	const char delim[] = "-- ::.";

	for (uint8_t delimIndex = 0; delimIndex < sizeof(delim); delimIndex++) {
		while(str[iStr] != delim[delimIndex] && iStr < strLen) {
			temp[i] = str[iStr]; 
			i++; 
			iStr++; 
		}
		
		temp[i] = 0; // set delim character to a terminator so the string ends
		iStr++; // move iStr past the delimiting character
		i = 0; 
		
		if (delimIndex == 0) {
			dateTimePricePair->year = atoi(temp);
		} else if (delimIndex == 1) {
			dateTimePricePair->month = atoi(temp);
		} else if (delimIndex == 2) {
			dateTimePricePair->day = atoi(temp);
		} else if (delimIndex == 3) {
			dateTimePricePair->hour = atoi(temp);
		} else if (delimIndex == 4) {
			dateTimePricePair->minute = atoi(temp);
		} else if (delimIndex == 5) {
			dateTimePricePair->second = atoi(temp);
		} else if (delimIndex == 6) {
			dateTimePricePair->millisec = atoi(temp);
		}
	}
}

//----------------------------------------------------------------------------------------------------------
// Name: StreamReadBlock
// Desc:
//----------------------------------------------------------------------------------------------------------
std::vector<DateTimePricePair> StreamReadBlock(std::string& filepath) {
    
    std::ifstream filestream(filepath);

    if (!filestream.is_open()) {
        
        std::cout << "Error opening file."; 
        return;
    }
    std::vector<DateTimePricePair> dateTimePricePairs; 
    dateTimePricePairs.clear(); 

    char buffer[bufferSize];
    memset(buffer, 0, sizeof(buffer));

    // We want to ignore the headers so discard characters until the first newline
    unsigned int offset = 0;
    while (filestream.get() != '\n') {
        offset++;
    }

    unsigned int bufferOffset = 0; 
    unsigned int headerCounter = 0; 

    DateTimePricePair dateTimePricePair;
    
    while (filestream) {

        filestream.read(&buffer[bufferOffset], sizeof(buffer) - bufferOffset);
        unsigned int lastCommaIndex = 0; 

        for (auto i = 0; i < sizeof(buffer); i++) {

            // read until we find a ',' then insert a null terminator
            if (buffer[i] == ',' || buffer[i] == '\n') {
                
                char lastChar = buffer[i];
                buffer[i] = 0; 

                if (headerCounter == 3) {
                    FastParse(&buffer[lastCommaIndex], &dateTimePricePair); 

                } else if (headerCounter == 4) {

                    // do this to avoid constructing an std::string
                    char bidStrBuffer[32];
                    unsigned int counter = lastCommaIndex; 
                    unsigned int copyCounter = 0; 

                    while (buffer[counter]) {
                        if (buffer[counter] != '.') {
                            bidStrBuffer[copyCounter] = buffer[counter]; 
                            copyCounter++; 
                        }

                        counter++; 
                    }

                    bidStrBuffer[copyCounter] = 0; 

                    dateTimePricePair.quote = std::atoi(bidStrBuffer);
                    dateTimePricePairs.push_back(dateTimePricePair);
                }

                if (headerCounter >= (numHeaders - 1) || lastChar == '\n') {
                    headerCounter = 0;

                } else {

                    headerCounter++;
                }

                lastCommaIndex = i + 1; // plus one because we want the element after
            }
            // we're at the last element
            else {
                
                if (i == sizeof(buffer) - 1) {

                    // copy the remaining characters back to the beginning of the buffer
                    memcpy(buffer, &buffer[lastCommaIndex], i - lastCommaIndex + 1);
                    bufferOffset = i - lastCommaIndex + 1; 

                    lastCommaIndex = 0; 
                }
            }

        }
    }

    filestream.close();

    return dateTimePricePairs; 
}
    
//----------------------------------------------------------------------------------------------------------
// Name: StreamReadBlock
// Desc:
//----------------------------------------------------------------------------------------------------------
std::vector<DateTimePricePair> ImportCsv(std::string& filepath) {
    auto csv = StreamReadBlock(filepath); 
    return csv; 
}
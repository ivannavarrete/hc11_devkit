
#ifndef SFILE_H
#define SFILE_H


// This class describes an s-file.
class SFile {
public:
	SFile(const char *file, int dtype);	// constructor
	~SFile();							// destructor
	unsigned char *GetData();			// get pointer to raw data
	int GetDataSize();					// get raw data size
	//void DebugDump();					// dump records

private:
	void ParseRecords(const char *);	// parse all s-file records from file
	void CreateRawData();				// convert s-file records to raw data
	void Delete();						// free records

private:
	struct record *records;				// linked list of records
	unsigned char *data;				// raw s-file data
	int dtype;							// raw data type
	int dsize;							// raw data size
};


// Record structure contains one s-file record.
struct record {
	char type[3];						// record type (ASCII)
	short len;							// record data length
	unsigned short addr;				// record load address
	unsigned char *data;				// record data
	short csum;							// record checksum
	struct record *next;				// pointer to next record 
};


// Different types of raw s-file data.
//#define SFILE_ADDR_ON		1			// address info
//#define SFILE_LEN_ON		2			// record length info
#define SFILE_COMPRESSED	3			// addr on, len on (1 | 2) = 3
#define SFILE_UNCOMPRESSED	4			// addr off, len off


#endif // SFILE_H

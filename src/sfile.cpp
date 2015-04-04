
//
// sfile.cpp
//
// This object represents an s-file. It contains data both in record format and
// in raw format.
//
// This object is used by main.cpp and debugger.cpp
//
 

#include <cstdlib>
#include <cstring>

#include <iostream>
#include <iomanip>
#include <fstream>
#include <new>

#include "sfile.h"
#include "error.h"
#include "ui.h"

using namespace std;


extern UI &ui;

// Constructor.
SFile::SFile(const char *file, int dtype) {
	records = NULL;
	data = NULL;
	this->dtype = dtype;
	dsize = 0;

	try {
		ParseRecords(file);
		CreateRawData();
	} catch (Error &err) {
		throw;			// rethrow the exception
	}
}


// Destructor.
SFile::~SFile() {
	Delete();
}


// Get pointer to raw data.
unsigned char *SFile::GetData() {
	return data;
}


// Get raw data size.
int SFile::GetDataSize() {
	return dsize;
}


// Parse all s-file records from file.
void SFile::ParseRecords(const char *file) {
	struct record *rec, *rec2, dummy_head;
	ifstream sfile;
	char buf[10];

	// init data
	records = rec = &dummy_head;
	dummy_head.next = NULL;

	try {
		// open file
		sfile.open(file, ios::in | ios::binary);
		if (!sfile) // fix the error message to reflect real error
			throw *(new Error("SFile::ParseRecords()", "can't open file"));

		// extract records from file
		//records = rec = &dummy_head;
		while (1) {
			rec2 = new struct record;
			memset(rec2, 0, sizeof(struct record));

			// get record type
			if (!sfile.read(rec2->type, 2)) {
				delete rec2;
				break;
			}
		
			// get record length
			if (!sfile.read(buf, 2))
				throw *(new Error("Sfile::ParseRecords()", "fucked up 1"));
			buf[2] = 0;
			rec2->len = strtol(buf, NULL, 16) - 3;

			// get record addr
			if (!sfile.read(buf, 4))
				throw *(new Error("SFile::ParseRecords()", "fucked up 2"));
			buf[4] = 0;
			rec2->addr = strtol(buf, NULL, 16);

			// get record data
			rec2->data = new unsigned char[rec2->len];
			buf[2] = 0;
			for (int i=0; i<rec2->len; i++) {
				if (!sfile.read(buf, 2))
					throw *(new Error("SFile::ParseRecords", "fucked up 3"));
				rec2->data[i] = strtol(buf, NULL, 16);
			}
		
			// get record checksum
			if (!sfile.read(buf, 2))
				throw *(new Error("SFile::ParseRecords", "fucked up 4"));
			buf[2] = 0;
			rec2->csum = strtol(buf, NULL, 16);

			// skip newline
			sfile.seekg(1, ios::cur);

			// link record into list
			rec->next = rec2;
			rec = rec2;
		}
	} catch (Error &err) {			// s-file is fucked up, abort
		records = records->next;	// skip dummy head
		Delete();					// free memories
		throw;						// rethrow the exception
	} catch (bad_alloc err) {		// memory allocation errors
		ui.ShowMsg("SFile::ParseRecords(): memory allocation exception\n");
		
		records = records->next;	// skip dummy head
		Delete();					// free memories

		exit(1);
	} catch (...) {					// unknown errors
		ui.ShowMsg("SFile::ParseRecords(): unknown exception\n");
		exit(1);
	}

	rec->next = NULL;				// terminate list
	records = records->next;		// skip dummy head
	sfile.close();
}


// Convert s-file records to raw data.
void SFile::CreateRawData() {
	struct record *rec;
	int start_mem, end_mem, tot_mem;

	// calculate total amount of memory needed
	switch (dtype) {
		case SFILE_UNCOMPRESSED:
			rec = records;
			while (rec && rec->next && strncmp(rec->type, "S1", 2))
				rec = rec->next;
			start_mem = rec->addr;

			rec = records;
			while (rec && rec->next && !strncmp(rec->next->type, "S1", 2))
				rec = rec->next;
			end_mem = rec->addr + rec->len;

			tot_mem = end_mem - start_mem;
			break;
		case SFILE_COMPRESSED:
			tot_mem = 0;

			rec = records;
			while (rec) {
				tot_mem += rec->len + 3;
				rec = rec->next;
			}
			
			break;
		default:
			// XXX throw some exception
			break;
	}

	// init object variable
	dsize = tot_mem;

	// allocate memory
	data = new unsigned char[dsize];
	memset(data, 0, dsize);

	// fill memory with raw data
	int i = 0;
	rec = records;
	while (rec) {
		if (dtype == SFILE_COMPRESSED) {
			memcpy(data+i, (char *)rec->addr, 2);
			memcpy(data+i, (char *)rec->len, 1);
		}
		
		memcpy(data+i, rec->data, rec->len);
		
		if (dtype == SFILE_COMPRESSED)
			i += rec->len + 3;
		else if (dtype == SFILE_UNCOMPRESSED && rec->next != NULL)
			i += rec->next->addr - rec->addr;

		rec = rec->next;
	}
}


// Free all memories.
void SFile::Delete() {
	struct record *rec;

	// delete all s-file records.
	while (records) {
		rec = records;
		if (records->data)
			delete [] records->data;
		records = records->next;
		delete rec;
	}

	// delete raw s-file data.
	delete [] data;
}


// Dump the s-file.
/*
void SFile::DebugDump() {
	struct record *rec;

	// dump raw data
	cout << hex << setfill('0');
	for (int i=0; i<dsize; i++)
		cout << setw(2) << (int)data[i];
	cout << "\n\n";

	// dump records list
	rec = records;
	while (rec) {
		cout << hex << setfill('0');
		cout.setf(ios::uppercase);

		cout << rec->type << " ";
		cout << setw(2) << rec->len << " ";
		cout << setw(4) << rec->addr << " ";
		for (int i=0; i<rec->len; i++)
			cout << setw(2) << (int)rec->data[i];
		if (rec->len > 0)
			cout << " ";
		cout << setw(2) << rec->csum << endl;
		
		rec = rec->next;
	}
	cout << endl;
}
*/

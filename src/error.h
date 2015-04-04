
//
// error.h
//
// This object is used in exception handling throughout the program. It probably
// doesn't work well in memory allocation errors, since when that happens we
// need to create an Error object, and for that we need more memory. This will
// probably generate another memory allocation error, which then will lead us
// to the 'unspecified (possibly double) memory allocation error' exception
// handler in main.cpp. But then again, it may not, since this object is quite
// small. Will test it later...
//


class Error {
public:
	Error(const char *method, const char *error);	// constructor

public:
	const char *method;					// the class::method of the exception
	const char *error;					// error string specifying the error
};

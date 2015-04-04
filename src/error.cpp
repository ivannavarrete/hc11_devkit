
//
// error.cpp
//


#include "error.h"


// Constructor.
Error::Error(const char *method, const char *error) {
	this->method = method;
	this->error = error;
}

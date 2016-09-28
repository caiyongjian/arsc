#pragma once

#include <qsharedpointer.h>
#include "ArscFile.h"

class ResultCallback {
public:
	virtual void onResult(ArscFile* result) = 0;
};
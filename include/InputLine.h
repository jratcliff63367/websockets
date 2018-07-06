#pragma once

#include <stdint.h>

// Handles input stream
// Returns true if the user hit 'enter' and we have a buffer of input
enum class InputMode
{
	NOTHING,			// No input
	NEW_CHAR,			// Added a new character
	ENTER,				// got an enter key
	ESCAPE,				// Got an escape key
	BACKSPACE,			// Processed a backspace key
	OVERFLOW			// Got a new char, but buffer is full
};

InputMode getInputLine(char *buffer,uint32_t maxLen,uint32_t &len);


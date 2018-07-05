#pragma once

#include <stdint.h>

// Handles input stream
// Returns true if the user hit 'enter' and we have a buffer of input

bool getInputLine(char *buffer,uint32_t maxLen,uint32_t &len);


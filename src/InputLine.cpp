#include "InputLine.h"
#include <stdio.h>
#include <conio.h>

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif


InputMode getInputLine(char *buf, uint32_t maxLen, uint32_t &len)
{
	InputMode ret = InputMode::NOTHING;

	if (kbhit())
	{
		char c = (char)getch();
		if (c == 10 || c == 13)
		{
			buf[len] = 0;
			ret = InputMode::ENTER;
			printf("\n");
		}
		else if (c == 27)
		{
			buf[len] = 0;
			ret = InputMode::ESCAPE;
		}
		else if (c == 8)
		{
			if (len)
			{
				len--;
				buf[len] = 0;
				printf("%c", c);
				printf(" ");
				printf("%c", c);
				ret = InputMode::BACKSPACE;
			}
		}
		else if (c >= 32 && c <= 127)
		{
			if (len < (maxLen - 1))
			{
				buf[len] = c;
				len++;
				buf[len] = 0;
				printf("%c", c);
				ret = InputMode::NEW_CHAR;
			}
			else
			{
				ret = InputMode::OVERFLOW;
			}
		}
	}
	return ret;
}
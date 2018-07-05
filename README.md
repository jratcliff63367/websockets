# websockets
This project is just a copy of websocketspp with non-boost ASIO such that it builds for windows and linux out of the box


Note: To build this on Linux you need 'ncurses' installed.
If you do not already have 'ncurses' installed you will get a compile error

The magic linux comand to install 'ncurses' is:

sudo apt-get install libncurses5-dev

Ncurses is a library that supports terminal commands.

For this demo we want to support a simple chat server and need
keyboard/terminal input for that.

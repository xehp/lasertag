/*
2016-08-08
Adapted for atmega328p
*/

#define VER_XSTR(s) VER_STR(s)
#define VER_STR(s) #s


#define VERSION_MAJOR 0
#define VERSION_MINOR 3
#define VERSION_DEBUG 16


#define VERSION_STRING "tag_sw " VER_XSTR(VERSION_MAJOR) "." VER_XSTR(VERSION_MINOR) "." VER_XSTR(VERSION_DEBUG)

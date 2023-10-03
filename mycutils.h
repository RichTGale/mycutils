/**
 * mycutils.h
 *
 * This file containsi public data-structures and function prototype declarations 
 * for various utility funtions and types, as well as enumerations relating
 * to them.
 *
 * Version: 1.0.2
 * Author: Richard Gale
 */

#ifndef MYCUTILS_H
#define MYCUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

/**
 * This is the number of nanoseconds in a second.
 */
#define NANOS_PER_SEC 1000000000

/********************************* Types *************************************/

typedef struct {
    int x;
    int y;
} vec2d;

/******************************** Maths **************************************/

/**
 * This function maps value x to a value within a desired range.
 */
double map(double x, double in_min,  double in_max, 
                     double out_min, double out_max);


/********************************* Time **************************************/

/**
 * This function returns true if a number of nano-seconds equal to or greater
 * than wait_time has elapsed since start.
 */
bool check_timer(struct timespec ts_start, uint64_t wait_time);

/**
 * This function obtains the current time, storing it in the timespec
 * provided to it.
 */
void start_timer(struct timespec* ts);

/**
 * This function returns a string that represent the current time.
 */
char* timestamp();

/******************************** In/Out *************************************/

/**
 * This function prints a prompt to the user, then assigns a string that is
 * input by the user to the buffer provided to it.
 */
void scans(char** buf, char* prompt);

/**
 * This function returns a char that was input by the user. It doesn't wait
 * for the user to press enter. (Not my code)
 */
char scanc_nowait();

/**
 * Closes the provided file stream. If there is an error, it is printed on
 * stderr and the program will exit.
 */
void closefs(FILE* fp);

/**
 * This function opens a file that has a name that matches fname. It opens the
 * file in the mode specified by mode.
 * If there is an error it will be printed on stderr and the program 
 * is exited. If the file is successfully opened, this function
 * will return a pointer to the file stream.
 */
FILE* openfs(char* fname, char* mode);

/**
 * This function assigns the next char in the file stream provided to it to
 * the buffer provided to it.
 */
bool readfsc(FILE* fstreamp, char* buf);

/**
 * This function assigns the next line in the file stream provided to it to
 * the string provided to it. It returns true if the line was read successfully
 * or false if EOF was reached. If an error occurs the program will exit.
 * Make sure to free() the buffer when you're finished with it.
 */
bool readfsl(FILE* fstreamp, char** buf);

/**
 * This function writes the char provided to it to the file stream provided to
 * it.
 */
void writefsc(FILE* fstreamp, char ch);

/**
 * This function writes the string provided to it to the file steam provided
 * to it.
 */
void writefss(FILE* fstreamp, char* str);


/******************************** Strings ************************************/

/**
 * This function returns the number of bytes a string will need to be
 * allocated based on the variable argument list and a format string that are
 * provided to this function.
 */
size_t vbytesfmt(va_list lp, char* fmt);

/**
 * This function dynamically allocates only the needed amount of memory to a
 * string based on the argument list, then concatenates the argument list into 
 * the supplied format and stores it in the supplied string pointer.
 */
void strfmt(char** sp, char *fmt, ...);

/**
 * This function removes the char element from the string provided to it which
 * is at the element number provided to it.
 */
void sdelelem(char** sp, unsigned elem);

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void sdelchar(char** sp, char remove);

/**
 * This function removes the last character before the null character
 * from the string at the string pointer provided to it.
 */
//void stringrmlast(char** s);

/******************************* Terminal ************************************/

#define LINE_HEIGHT 8
#define CHAR_WIDTH LINE_HEIGHT

enum directions {
    ABOVE,
    BELOW,
    BEFORE,
    AFTER
    };

enum termcolours { 
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    };

enum textmodes { 
    BOLD,
    NORMAL,
    BLINK,
    REVERSE,
    UNDERLINE
    };

/**
 * This function clears the terminal.
 */
void clear();

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's beginning.
 */
void clearb();

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's end.
 */
void clearf();

/**
 * This function clears the entire line that the terminal cursor is currently
 * on.
 */
void clearfb();

/**
 * This function returns the number of rows and columns of the terminal.
 */
vec2d get_res();

/**
 * This function moves the terminal cursor a number of rows or columns
 * equal to the number provided to the function, and in a direction that is
 * also provided.
 */
void move_cursor(enum directions direction, unsigned int n);

/**
 * This function prints the text file at the file path provided to it. It
 * prints the text file in the colours and mode that are provided to
 * the function.
 */
void print_fs_mod(char* filepath, vec2d origin, enum termcolours colour, 
                                                enum textmodes mode);

/**
 * This function prints the string provided to it at the position that is
 * also provided to the function.
 */
void print_str(char* str, vec2d pos);

/**
 * This function prints the string provided to it at the location
 * that is also provided. It prints the string in the colours and in the
 * mode provided.
 */
void print_str_mod(char* str, vec2d origin, enum termcolours fcol,
                                            enum textmodes mode);

/**
 * This function places the terminal cursor at the row and column numbers
 * provided to it.
 */
void put_cursor(unsigned int col, unsigned int row);

/**
 * This function sets the background colour of the terminal cursor.
 */
void text_bcol(enum termcolours c);

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void text_fcol(enum termcolours c);

/**
 * This function changes the terminal text-mode.
 */
void text_mode(enum textmodes m);

#endif // MYCUTILS_H

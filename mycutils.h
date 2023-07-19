/**
 * mycutils.h
 *
 * This file contains the public declarations for various
 * utility funtions.
 *
 * Author: Richard Gale
 * Version: 11th March, 2023
 */

#ifndef MYCUTILS_H
#define MYCUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // strerror()
#include <stdbool.h>
#include <stdarg.h>
#include <time.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <termios.h>

#define NANOS_PER_SEC 1000000000

/********************************* Types *************************************/

typedef struct {
    int x;
    int y;
} vec2d;

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
 * This function asks the user to input a char in response to a prompt supplied
 * to it, then stores it in the supplied char pointer. 
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
 * This function dynamically allocates only the needed amount of memory to a
 * string based on the argument list, then concatenates the argument list into 
 * the supplied format and stores it in the supplied string pointer.
 */
void stringf(char** sptr, char *fmt, ...);

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void stringrmc(char** str, char remove);

/**
 * This function removes the last character before the null character
 * from the string at the string pointer provided to it.
 */
void stringrmlast(char** s);

/******************************* Terminal ************************************/

#define LINE_HEIGHT 8
#define CHAR_WIDTH LINE_HEIGHT


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
 * This function sets the background colour of the terminal cursor.
 */
void curscolb(enum termcolours c);

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void curscolf(enum termcolours c);

/**
 * This function places the terminal cursor at the row and column numbers
 * provided to it.
 */
void cursput(unsigned int col, unsigned int row);

/**
 * This function moves the cursor left by the number of columns provided
 * to it.
 */
void cursputb(unsigned int ncols);

/**
 * This function moves the cursor right by the number of columns provided
 * to it.
 */
void cursputf(unsigned int ncols);

/**
 * This function moves the cursor up by the number of rows provided
 * to it.
 */
void cursputu(unsigned int nrows);

/**
 * This function clears the terminal.
 */
void termclear();

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's beginning.
 */
void termclearl();

/**
 * This function draws in the terminal base on the contents of a file.
 */
void termdrawfs(char* filepath, vec2d origin, vec2d bounds);

/**
 * This function draw a single row of an art file.
 */
void termdrawl(char* text, size_t text_len, vec2d origin, vec2d bounds);

/**
 * This function draws a string in the terminal.
 * The art files need to exist for each character.
 */
void termdraws(char* str, vec2d origin, vec2d bounds);

/**
 * This function prints the string provided to it into the the terminal
 * at the location specified by the vec2d that is also provided to the
 * function.
 */
void termprint(char* str, vec2d origin);

/**
 * This function returns the number of rows and columns of the terminal.
 */
vec2d termres();

/**
 * This function changes the terminal text-mode.
 */
void textmode(enum textmodes m);

#endif // MYCUTILS_H

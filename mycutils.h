
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
#include <termios.h>

/**
 * There is this many nanoseconds in a second.
 */
#define NANOS_PER_SEC 1000000000


/********************************* Types *************************************/

/**
 * The log data type structure.
 */
typedef struct log_data {
    FILE* fs;
    void (*out)(FILE* fs, char* fmt, ...);  /* Function pointer: fsout() */
} log;


typedef struct {
    int x;
    int y;
} cart2d;

/******************************** Maths **************************************/

/**
 * This function maps value x to a value within a desired range.
 */
double map(double x, double in_min,  double in_max, 
                     double out_min, double out_max);

/******************************** In / Out ***********************************/

/**
 * This function returns a new instance of the log data type.
 * */
log* log_init(char* fname);

/**
 * This function destroys/deallocates memory from an instance of the log
 * data type.
 * */
void log_term(log* m);

/**
 * This function opens a file that has a name that matches fname. It opens the
 * file in the mode specified by mode.
 * If there is an error it will be printed on stderr and the program 
 * is exited. If the file is successfully opened, this function
 * will return a pointer to the file stream.
 */
FILE* openfs(char* fname, char* mode);

/**
 * This function closes the file stream provided tp it. If there is an error,
 * it is printed on stderr and the program will exit.
 */
void closefs(FILE* fs);

/**
 * This function assigns the next char in the file stream provided to it to
 * the buffer provided to it. It returns true on success or false if EOF is
 * reached. It will exit the program if an error occurs.
 */
bool readfsc(FILE* fs, char* buf);

/**
 * This function assigns the next line in the file stream provided to it to
 * the string provided to it. It returns true if the line was read successfully
 * or false if EOF was reached. If an error occurs the program will exit.
 * Make sure to free() the buffer when you're finished with it.
 */
bool readfsl(FILE* fs, char** buf);

/**
 * This function writes the char provided to it to the file stream provided to
 * it.
 */
void writefsc(FILE* fs, char ch);

/**
 * This function writes the string provided to it to the file stream provided
 * to it.
 */
void writefss(FILE* fs, char* str);

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
char* strfmt(char* buf, char *fmt, ...);

/**
 * This function removes the char element from the string provided to it which
 * is at the element number/index provided to it.
 */
char* sdelelem(char* sp, unsigned elem);

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void sdelchar(char* sp, char remove);

/**
 * This function returns a string that represent the current time.
 * For reasons detailed in a comment within this function, you must
 * free() the string that this function returns.
 */
char* timestamp();

/**
 * This function outputs to a filestream.
 * It dynamically allocates the neccessary amount of memory to an internal
 * buffer that is based on the format string and argument list
 * parameters, then outputs the buffer to the filestream parameter.
 */
void fsout(FILE* fs, char *fmt, ...);

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

void exec(char* cmd);

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
cart2d get_res();

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
void print_fs_mod(char* filepath, cart2d origin, enum termcolours colour, 
                                                enum textmodes mode);

/**
 * This function prints the string provided to it at the position that is
 * also provided to the function.
 */
void print_str(char* str, cart2d pos);

/**
 * This function prints the string provided to it at the location
 * that is also provided. It prints the string in the colours and in the
 * mode provided.
 */
void print_str_mod(char* str, cart2d origin, enum termcolours fcol,
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

#endif

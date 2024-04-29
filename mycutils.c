/**
 * mycutils.c
 *
 * This file contains the definitions of various utility functions and types.
 *
 * Version: 1.0.2
 * Author: Richard Gale
 */

#include "mycutils.h"

/******************************** Maths **************************************/

/**
 * This function maps value x to a value within a desired range.
 */
double map(double x, double in_min,  double in_max, 
                     double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/********************************* Time **************************************/

/**
 * This function returns true if a number of nano-seconds equal to or greater
 * than wait_time has elapsed since start.
 */
bool check_timer(struct timespec start, uint64_t wait_time)
{
    const bool HAS_ELAPSED = true;      /* Value if time has elapsed. */
    const bool NOT_ELAPSED = false;     /* Value if time has not elapsed. */
    struct timespec current;            /* The current time. */
    struct timespec elapsed;            /* The time elapsed since start. */

    /* Obtaining the current time. */
    clock_gettime(CLOCK_REALTIME, &current);

    /* Calculating the elapsed time. */
    elapsed.tv_sec = current.tv_sec - start.tv_sec;
    elapsed.tv_nsec = current.tv_nsec - start.tv_nsec;

    /* Checking whether the time hasn't elapsed. */
    if ((elapsed.tv_sec * NANOS_PER_SEC) + elapsed.tv_nsec < wait_time)
        return NOT_ELAPSED;

    /* The time has elapsed. */
    return HAS_ELAPSED;
}

/**
 * This function obtains the current time and stores it in the timespec
 * that was provided to it.
 */
void start_timer(struct timespec* ts)
{
    char* tstamp;

    /* Obtaining the current time.*/
    if ((clock_gettime(CLOCK_REALTIME, ts)) != -1)
        return;
        
    /* An error occured so we are printing an error message. */
    fprintf(stderr, 
            "[ %s ] ERROR: in function start_timer(): %s\n",
            (tstamp = timestamp()), strerror(errno));

    /* De-allocating memory. */
    free(tstamp);

    /* Exiting the program. */
    exit(EXIT_FAILURE);
    
}

/**
 * This function returns a string that represent the current time.
 * For reasons detailed in a comment within this function, you must
 * free() the string that this function returns.
 */
char* timestamp()
{
    time_t current_time;    /* The current time. */
    char* stamp;            /* The time stamp. */
    char* stamp_cpy;        /* A Copy of the time stamp. */

    /* Obtaining the current time. */
    if ((current_time = time(NULL)) == ((time_t) - 1))
    {
        /* An error occured so we're printing an error message to and exiting
         * the program. */
        fprintf(stderr, 
                "ERROR: In function timestamp(): "
                "Calender time is not available\n");
        exit(EXIT_FAILURE);
    }

    /* Converting time to local time format. */
    if ((stamp = ctime(&current_time)) == NULL)
    {
        /* An error occured converting so we're printing an error message
         * and exiting the program. */
        fprintf(stderr, 
                "ERROR: In function timestamp(): "
                "Failure to convert the current time to a string.\n");
        exit(EXIT_FAILURE);
    }

    /* The string that ctime() returns is not one that should be be freed
     * with free() because the memory it uses was not allocated with malloc()
     * or a similar function. Because we are going to use sdelchar() to remove
     * the newline character that ctime() added to our timestamp, and
     * sdelchar() uses free() to remove chars from strings, we have to make
     * a copy of our stamp.
     * If this copy is not freed by the calling function, it will create a 
     * memory leak.
     */
    strfmt(&stamp_cpy, "%s", stamp);

    /* Removing the newline character that was added by ctime(). */
    sdelchar(&stamp_cpy, '\n');

    /* Returning the copy of the time stamp. */
    return stamp_cpy;
}

/******************************** In/Out *************************************/

/**
 * This function prints a prompt to the user, then assigns a string that is
 * input by the user to the string pointer provided to it.
 */
void scans(char** buf, char* prompt)
{
    char* buf_cpy;  /* A copy of the buffer. */
    char userin;    /* The user input. */

    /* Initialising the buffer to avoid invalid pointer error upon
     * initial call to free(). */
    *buf = (char*) malloc(sizeof(char));
    *buf[0] = '\0';
    strfmt(&buf_cpy, "%s", *buf);

    /* Going to a new line. */
    fprintf(stdout, "\n");
    
    do
    {
        /* Clearing the line. */
        clearfb();
        move_cursor(BEFORE, strlen(prompt) + strlen(*buf) + 1);

        /* Printing the prompt and any past user input. */
        fprintf(stdout, "%s%s\n", prompt, *buf);
        move_cursor(ABOVE, 1);
        move_cursor(AFTER, strlen(prompt) + strlen(*buf));

        /* Getting and processing user input. */
        switch (userin = scanc_nowait())
        {
            /* Backspace. */
            case (int) 127:
                /* Removing the last character in the buffer. */ 
                sdelelem(buf, strlen(*buf) - 1);
                
                /* Recreating the temporary copy of the buffer. */
                free(buf_cpy);
                strfmt(&buf_cpy, "%s", *buf);
                break;
            
            /* Enter. */
            case '\n':
                break;

            /* Anything else. */
            default:
                /* Recreating the buffer. */
                free(*buf);
                strfmt(buf, "%s%c", buf_cpy, userin);

                /* Recreating the temporary copy of the buffer. */
                free(buf_cpy);
                strfmt(&buf_cpy, "%s", *buf);
        }
    } while(userin != '\n');

    /* De-allocating temp variable memory. */
    free(buf_cpy);
}

/**
 * This function returns a char that was input by the user. It doesn't wait
 * for the user to press enter. (Not my code)
 */
char scanc_nowait() {
        char buf = 0;
        struct termios old = {0};
        if (tcgetattr(0, &old) < 0)
                perror("tcsetattr()");
        old.c_lflag &= ~ICANON;
        old.c_lflag &= ~ECHO;
        old.c_cc[VMIN] = 1;
        old.c_cc[VTIME] = 0;
        if (tcsetattr(0, TCSANOW, &old) < 0)
                perror("tcsetattr ICANON");
        if (read(0, &buf, 1) < 0)
                perror ("read()");
        old.c_lflag |= ICANON;
        old.c_lflag |= ECHO;
        if (tcsetattr(0, TCSADRAIN, &old) < 0)
                perror ("tcsetattr ~ICANON");
        return (buf);
}

/**
 * This function closes the file stream provided tp it. If there is an error,
 * it is printed on stderr and the program will exit.
 */
void closefs(FILE* fs)
{
    char* tstamp;   /* A time stamp. */

    /* Closing the file stream. */
    if (fclose(fs) == 0)
        return;
    
    /* An error occured so we are printing an error message. */
    fprintf(stderr,
            "[ %s ] ERROR: In function closefs: %s\n", 
            (tstamp = timestamp()), strerror(errno));

    /* De-allocating memory. */
    free(tstamp);

    /* Exiting the program. */
    exit(EXIT_FAILURE);
}

/**
 * This function opens a file that has a name that matches fname. It opens the
 * file in the mode specified by mode.
 * If there is an error it will be printed on stderr and the program 
 * is exited. If the file is successfully opened, this function
 * will return a pointer to the file stream.
 */
FILE* openfs(char* fname, char* mode)
{
    FILE* fs;       /* The pointer to the file stream. */
    char* tstamp;   /* A time stamp. */

    /* Opening the file. */
    if ((fs = fopen(fname, mode)) != NULL)
        return fs;

    /* An error occured so we're printing an error message. */
    fprintf(stderr, 
            "[ %s ] ERROR: In function openfs(): "
            "Could not open file %s: %s\n",
            (tstamp = timestamp()), fname, strerror(errno));

    /* De-allocating memory. */
    free(tstamp);

    /* Freeing memory. */
    exit(EXIT_FAILURE);
}

/**
 * This function assigns the next char in the file stream provided to it to
 * the buffer provided to it. It returns true on success or false if EOF is
 * reached. It will exit the program if an error occurs.
 */
bool readfsc(FILE* fs, char* buf)
{
    const bool SUCCESS = true;      /* Return value if success. */
    const bool END_OF_FILE = false; /* Return value if EOF. */
    char* tstamp;

    /* Getting the next char from the file stream and checking if it was
     * successfully read. */
    if ((*buf = fgetc(fs)) != EOF) 
        return SUCCESS;

    /* Checking if EOF was reached. */
    if (!ferror(fs)) 
        return END_OF_FILE;

    /* An error occurred so we're printing an error message. */
    fprintf(stderr,
            "[ %s ] ERROR: In function readfsc(): %s\n",
            (tstamp = timestamp()), strerror(errno));

    /* De-allocating memory. */
    free(tstamp);

    /* Exiting the program. */
    exit(EXIT_FAILURE);
}


/**
 * This function assigns the next line in the file stream provided to it to
 * the string provided to it. It returns true if the line was read successfully
 * or false if EOF was reached. If an error occurs the program will exit.
 * Make sure to free() the buffer when you're finished with it.
 */
bool readfsl(FILE* fs, char** buf)
{
    const bool SUCCESS = true;      /* Return value if success. */
    const bool END_OF_FILE = false; /* Return value if EOF. */
    size_t n;                       /* Allocated size of the buffer. */
    char* tstamp;                   /* A time stamp. */

    /* Initialising how big the buffer is. */
    n = 0;
    
    /* Reading the next line from the file stream and checking if it was
     * read successfully. */
    if (getline(buf, &n, fs) != -1)
        return SUCCESS;

    /* Checking if EOF was reached. */
    if (!ferror(fs))
        return END_OF_FILE;
            
    /* An error occurred so we are printing an error message. */
    fprintf(stdout,
            "[ %s ] ERROR: In function readfsl: %s\n",
            timestamp(), strerror(errno));

    /* De-allocating memory. */
    free(tstamp);

    /* Exiting the program. */
    exit(EXIT_FAILURE);
}

/**
 * This function writes the char provided to it to the file stream provided to
 * it.
 */
void writefsc(FILE* fs, char ch)
{
    /* Writing the char to the file stream. */
    fprintf(fs, "%c", ch); 
}

/**
 * This function writes the string provided to it to the file stream provided
 * to it.
 */
void writefss(FILE* fs, char* str)
{
    int c;  /* Index of the current char in the string. */

    /* Writing the string to the file stream. */
    for (c = 0; c < strlen(str); c++)
        writefsc(fs, str[c]);
}

/******************************** Strings ************************************/

/**
 * This function returns the number of bytes a string will need to be
 * allocated based on the variable argument list and a format string that are
 * provided to this function.
 */
size_t vbytesfmt(va_list lp, char* fmt)
{
    va_list lp_cpy; /* A Copy of the list of arguments. */
    size_t bytes;   /* The number of bytes the string needs. */

    /* Copying the argument list. */
    va_copy(lp_cpy, lp);

    /* Getting the number of bytes the string will need. Adding an extra
     * 1 char worth of bytes for the null character. */
    bytes = vsnprintf(NULL, 0, fmt, lp_cpy) + sizeof(char);

    /* Assuring a clean finish to the copy. */
    va_end(lp_cpy);

    /* Returning the number of bytes the string will need. */
    return bytes;
}

/**
 * This function dynamically allocates only the needed amount of memory to a
 * string based on the argument list, then concatenates the argument list into 
 * the supplied format and stores it in the supplied string pointer.
 */
void strfmt(char** sp, char *fmt, ...)
{
    va_list lp;     /* Pointer to the list of arguments. */
    size_t bytes;   /* The number of bytes the string needs. */

    /* Pointing to the first argument. */
    va_start(lp, fmt);

    /* Getting the number of bytes the string will need to be allocated. */
    bytes = vbytesfmt(lp, fmt);

    /* Allocating memory to the string. */
    *sp = (char*) malloc(bytes);

    /* Creating the string. */
    vsprintf(*sp, fmt, lp);

    /* Assuring a clean finish to the argument list. */
    va_end(lp);
}

/**
 * This function removes the char element from the string provided to it which
 * is at the element number/index provided to it.
 */
void sdelelem(char** sp, unsigned elem)
{
    char* to_elem;      /* Chars from start of string to element to delete. */
    char* from_elem;    /* Chars from element to delete to end of string. */
    unsigned c;         /* The current char in the string. */

    /* Allocating memory. */
    to_elem     = (char*) malloc(sizeof(char) * (elem + 1));
    from_elem   = (char*) malloc(sizeof(char) * (strlen(*sp) - elem));

    /* Storing the two sections of the string. */
    for (c = 0; c < strlen(*sp); c++)
    {
        if (c < elem)
            to_elem[c] = (*sp)[c];
        if (c > elem)
            from_elem[c] = (*sp)[c];
    }
    to_elem[elem] = '\0';
    from_elem[strlen(*sp) - elem - 1] = '\0';

    /* Recreating the string. */
    free(*sp);
    strfmt(sp, "%s%s", to_elem, from_elem);

    /* Cleaning up. */
    free(to_elem);
    free(from_elem);
}

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void sdelchar(char** sp, char remove)
{
    unsigned c;     /* Index of current char in the string. */

    /* Overwriting the unwanted characters. */
	for (c = 0; c < strlen(*sp); c++)
	{
        if ((*sp)[c] == remove)
        {
            sdelelem(sp, c);

            /* Decrementing the index so we will check the replacement 
             * character. */
            c--;
        }
    }
}

/******************************* Terminal ************************************/

/**
 * This function clears the entire terminal and positions the cursor at home.
 */
void clear()
{
    /* Clearing the terminal and putting the cursor at home. */
    system("tput clear");
}

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's beginning.
 */
void clearb()
{
    /* Clearing from the cursor to the beginning of the line. */
    system("tput el1");
}

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's end.
 */
void clearf()
{
    /* Clearing from the cursor to the end of the line. */
    system("tput el");
}

/**
 * This function clears the entire line that the terminal cursor is currently
 * on.
 */
void clearfb()
{
    /* Clearing the line that the terminal cursor is currently on. */
    clearf();
    clearb();
}

/**
 * This function returns the number of rows and columns of the terminal.
 */
vec2d get_res()
{
    vec2d res;      /* Storage for the rows and columns. */
    FILE* rfp;      /* File stream for the rows file. */
    FILE* cfp;      /* File stream for the columns file. */
    char rbuf[5];   /* The number of rows. */
    char cbuf[5];   /* The number of columns. */

    /* Creating a temporary directory to store the files. */
    system("if [ ! -d temp/ ]; then\nmkdir temp/\nfi");

    /* Writing the number of rows and columns to their files. */
    system("tput lines >> temp/screen_rows.txt");
    system("tput cols >> temp/screen_cols.txt");

    /* Opening the files. */
    rfp = openfs("temp/screen_rows.txt", "r");
    cfp = openfs("temp/screen_cols.txt", "r");

    /* Getting the number of rows and columns from the files. */
    fgets(rbuf, sizeof(rbuf), rfp);
    fgets(cbuf, sizeof(cbuf), cfp);

    /* Converting the number of rows and columns to integers. */
    res.x = atoi(cbuf); //strtol( cbuf, &end, 10 );
    res.y = atoi(rbuf); //strtol( rbuf, &end, 10 );

    /* Closing the files. */
    closefs(rfp);
    closefs(cfp);

    /* Deleting the files. */
    system("rm -rf temp");

    /* Returning the number of rows and columns that the terminal has. */
    return res;
}

/**
 * This function moves the terminal cursor a number of rows or columns
 * equal to the number provided to the function, and in a direction that is
 * also provided.
 */
void move_cursor(enum directions direction, unsigned int n)
{
    char* cmd; /* The command. */

    /* Creating the command. */
    switch (direction)
    {
        case ABOVE:
            strfmt(&cmd, "tput cuu %d", n);
            break;
        case BELOW:
            strfmt(&cmd, "tput cud %d", n);
            break;
        case BEFORE:
            strfmt(&cmd, "tput cub %d", n);
            break;
        case AFTER:
            strfmt(&cmd, "tput cuf %d", n);
            break;
    }

    /* Moving the cursor. */
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function prints the text file at the file path provided to it. It
 * prints the text file in the colours and mode that are provided to
 * the function.
 */
void print_fs_mod(char* filepath, vec2d origin, enum termcolours colour, 
                                                enum textmodes mode)
{
    FILE* fs;   /* Pointer to the file stream. */
    char* line; /* The text in the file. */

    /* Ensuring that the buffer is set to NULL. */
    line = NULL;
    
    /* Opening the file. */ 
    fs = openfs(filepath, "r");

    /* Setting the text mode and foreground colour. */
    text_mode(mode);
    text_fcol(colour);

    /* Reading the line from the file. */ 
    while (readfsl(fs, &line)) 
    {
        /* Drawing the line. */
        print_str(line, origin);

        /* Getting ready to draw the next line. */
        origin.y++;
        free(line);
        line = NULL;
    }

    /* Changing the text-mode and colour back to normal. */
    text_mode(NORMAL);

    /* Closing the file. */
    closefs(fs);
}

/**
 * This function prints the string provided to it at the position that is
 * also provided to the function.
 */
void print_str(char* str, vec2d pos)
{
    char* cmd;  // The command

    /* Printing the string. */
    put_cursor(pos.x, pos.y);
    strfmt(&cmd, "printf \"%s\"", str);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function prints the string provided to it at the position
 * that is also provided. It prints the string in the colours and in the
 * mode provided.
 */
void print_str_mod(char* str, vec2d pos, enum termcolours fcol,
                                         enum textmodes mode)
{
    /* Setting the text mode and foreground colour. */
    text_mode(mode);
    text_fcol(fcol);

    /* Printing the string. */
    print_str(str, pos);

    /* Reverting changes to the textcolours and mode. */
    text_mode(NORMAL);
}


/**
 * This function places the terminal at the row and column numbers
 * provided to it.
 */
void put_cursor(unsigned int col, unsigned int row)
{
    char* cmd;   /* The command. */

    /* Setting the cursor position. */
    strfmt(&cmd, "tput cup %d %d", row, col);
    system(cmd);

    /* Cleaning up. */
    free(cmd); 
}

/**
 * This function sets the background colour of the terminal cursor.
 */
void text_bcol(enum termcolours c)
{
    char* cmd;  /* The command. */

    /* Setting the background colour. */
    strfmt(&cmd, "tput setab %d", c);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void text_fcol(enum termcolours c)
{
    char* cmd;   /* The command. */

    /* Setting the colour. */
    strfmt(&cmd, "tput setaf %d", c);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function changes the terminal text-mode.
 */
void text_mode(enum textmodes m)
{
    /* Changing the terminal text-mode. */
    switch (m) 
    {
        case BOLD       : system( "tput bold" ); break;
        case NORMAL     : system( "tput sgr0" ); break;
        case BLINK      : system( "tput blink" ); break;
        case REVERSE    : system( "tput smso" ); break;
        case UNDERLINE  : system( "tput smul" ); break;
    }
}

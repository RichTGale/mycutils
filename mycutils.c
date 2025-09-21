#include "mycutils.h"

/******************************** Maths **************************************/

/**
 * This function maps value x to a value within a desired range.
 */
double map(double x, double in_min,  double in_max, 
                     double out_min, double out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}i

/******************************* In / Out ************************************/

/**
 * This function returns a new instance of the log data type.
 */
log* log_init(char* fname)
{
    log* l = (log*) malloc(sizeof(struct log_data));

    l->fs = openfs(fname, "a");
    l->out = &fsout;

    return l;
}

/**
 * This function destroys/deallocates memory from an instance of the log
 * data type.
 */
void log_term(log* l)
{
    closefs(l->fs);
    free(l);
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

    /* Opening the file. */
    if ((fs = fopen(fname, mode)) != NULL)
        return fs;

    /* An error occured so we're printing an error message. */
    fsout(
            stderr,
            "ERROR: In function openfs(): "
            "Could not open file %s: %s\n",
            fname, strerror(errno));

    /* Freeing memory. */
    exit(EXIT_FAILURE);
}

/**
 * This function closes the file stream provided tp it. If there is an error,
 * it is printed on stderr and the program will exit.
 */
void closefs(FILE* fs)
{
    /* Closing the file stream. */
    if (fclose(fs) == 0)
        return;
    
    /* An error occured so we are printing an error message. */
    fsout(
            stderr,
	        "ERROR: In function closefs: %s\n", 
	         strerror(errno));

    /* Exiting the program. */
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

    /* Getting the next char from the file stream and checking if it was
     * successfully read. */
    if ((*buf = fgetc(fs)) != EOF) 
        return SUCCESS;

    /* Checking if EOF was reached. */
    if (!ferror(fs)) 
        return END_OF_FILE;

    /* An error occurred so we're printing an error message. */
    fsout(
            stderr,
            "ERROR: In function readfsc(): %s\n",
            strerror(errno));

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
    fsout(
            stdout,
            "ERROR: In function readfsl: %s\n",
            strerror(errno));

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
    fsout(fs, "%c", ch); 
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
char* strfmt(char* buf, char *fmt, ...)
{
    va_list lp;     /* Pointer to the list of arguments. */
    size_t bytes;   /* The number of bytes the string needs. */

    /* Pointing to the first argument. */
    va_start(lp, fmt);

    /* Getting the number of bytes the string will need to be allocated. */
    bytes = vbytesfmt(lp, fmt);

    /* Allocating memory to the string. */
    buf = (char*) malloc(bytes);

    /* Creating the string. */
    vsprintf(buf, fmt, lp);

    /* Assuring a clean finish to the argument list. */
    va_end(lp);

    return buf;
}

/**
 * This function removes the char element from the string provided to it which
 * is at the element number/index provided to it.
 */
char* sdelelem(char* sp, unsigned elem)
{
    char* to_elem;      /* Chars from start of string to element to delete. */
    char* from_elem;    /* Chars from element to delete to end of string. */
    unsigned c;         /* The current char in the string. */

    /* Allocating memory. */
    to_elem     = (char*) malloc(sizeof(char) * (elem + 1));
    from_elem   = (char*) malloc(sizeof(char) * (strlen(sp) - elem));

    /* Storing the two sections of the string. */
    for (c = 0; c < strlen(sp); c++)
    {
        if (c < elem)
            to_elem[c] = sp[c];
        if (c > elem)
            from_elem[c] = sp[c];
    }
    to_elem[elem] = '\0';
    from_elem[strlen(sp) - elem - 1] = '\0';

    /* Recreating the string. */
    free(sp);
    sp = strfmt(sp, "%s%s", to_elem, from_elem);

    /* Cleaning up. */
    free(to_elem);
    free(from_elem);

    return sp;
}

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void sdelchar(char* sp, char remove)
{
    unsigned c;     /* Index of current char in the string. */

    /* Overwriting the unwanted characters. */
	for (c = 0; c < strlen(sp); c++)
	{
        if (sp[c] == remove)
        {
            sp = sdelelem(sp, c);

            /* Decrementing the index so we will check the replacement 
             * character. */
            c--;
        }
    }
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
        fsout(
                stderr, 
                "ERROR: In function timestamp(): "
                "Calender time is not available\n");

        exit(EXIT_FAILURE);
    }

    /* Converting time to local time format. */
    if ((stamp = ctime(&current_time)) == NULL)
    {
        /* An error occured converting so we're printing an error message
         * and exiting the program. */
        fsout(
                stderr,
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
    stamp_cpy = strfmt(stamp_cpy, "%s", stamp);

    /* Removing the newline character that was added by ctime(). */
    sdelchar(stamp_cpy, '\n');

    /* Returning the copy of the time stamp. */
    return stamp_cpy;
}

/**
 * This function outputs to a filestream.
 * It dynamically allocates the neccessary amount of memory to an internal
 * buffer that is based on the format string and argument list
 * parameters, then outputs the buffer to the filestream parameter.
 */
void fsout(FILE* fs, char *fmt, ...)
{
    char* _timestamp;
    char* buf;
    va_list lp;     /* Pointer to the list of arguments. */
    size_t bytes;   /* The number of bytes the string needs. */

    /* Pointing to the first argument. */
    va_start(lp, fmt);

    /* Getting the number of bytes the string will need to be allocated. */
    bytes = vbytesfmt(lp, fmt);

    /* Allocating memory to the string. */
    buf = (char*) malloc(bytes);

    /* Creating the string. */
    vsprintf(buf, fmt, lp);

    /* Assuring a clean finish to the argument list. */
    va_end(lp);

    _timestamp = timestamp();

    fprintf(fs, "[ %s ] %s", _timestamp, buf);

    free(_timestamp);
    free(buf);
}

/******************************Terminal***********************************/

void exec(char* cmd)
{
    system(cmd);
    free(cmd);
}

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
cart2d get_res()
{
    cart2d res;      /* Storage for the rows and columns. */
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
            exec(strfmt(cmd, "tput cuu %d", n));
            break;
        case BELOW:
            exec(strfmt(cmd, "tput cud %d", n));
            break;
        case BEFORE:
            exec(strfmt(cmd, "tput cub %d", n));
            break;
        case AFTER:
            exec(strfmt(cmd, "tput cuf %d", n));
            break;
    }
}

/**
 * This function prints the text file at the file path provided to it. It
 * prints the text file in the colours and mode that are provided to
 * the function.
 */
void print_fs_mod(char* filepath, cart2d origin, enum termcolours colour,
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
void print_str(char* str, cart2d pos)
{
    char* cmd;  // The command

    /* Printing the string. */
    put_cursor(pos.x, pos.y);
    exec(strfmt(cmd, "printf \"%s\"", str));
}

/**
 * This function prints the string provided to it at the position
 * that is also provided. It prints the string in the colours and in the
 * mode provided.
 */
void print_str_mod(char* str, cart2d pos, enum termcolours fcol,
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
    exec(strfmt(cmd, "tput cup %d %d", row, col));
}

/**
 * This function sets the background colour of the terminal cursor.
 */
void text_bcol(enum termcolours c)
{
    char* cmd;  /* The command. */

    /* Setting the background colour. */
    exec(strfmt(cmd, "tput setab %d", c));
}

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void text_fcol(enum termcolours c)
{
    char* cmd;   /* The command. */

    /* Setting the colour. */
    exec(strfmt(cmd, "tput setaf %d", c));
}

/**
 * This function changes the terminal text-mode.
 */
void text_mode(enum textmodes m)
{
    /* Changing the terminal text-mode. */
    switch (m)
    {
        case BOLD       : system( "tput bold"  ); break;
        case NORMAL     : system( "tput sgr0"  ); break;
        case BLINK      : system( "tput blink" ); break;
        case REVERSE    : system( "tput smso"  ); break;
        case UNDERLINE  : system( "tput smul"  ); break;
    }
}

/**
 * mycutils.c
 *
 * This file contains the definitions of various utility functions.
 *
 * Author: Richard Gale
 * Version: 16th July, 2023
 */

#include "mycutils.h"

/********************************* Time **************************************/

/**
 * This function returns true if a number of nano-seconds equal to or greater
 * than wait_time has elapsed since start.
 */
bool check_timer(struct timespec start, uint64_t wait_time)
{
    struct timespec current;    /* The current time. */
    struct timespec elapsed;    /* The time elapsed since start. */

    /* Obtaining the current time. */
    clock_gettime(CLOCK_REALTIME, &current);

    /* Calculating the elapsed time. */
    elapsed.tv_sec = current.tv_sec - start.tv_sec;
    elapsed.tv_nsec = current.tv_nsec - start.tv_nsec;

    /* Checking whether the time hasn't elapsed. */
    if ((elapsed.tv_sec * NANOS_PER_SEC) + elapsed.tv_nsec < wait_time)
        return false;

    /* The time has elapsed. */
    return true;
}

/**
 * This function obtains the current time and stores it in the timespec
 * that was provided to it.
 */
void start_timer(struct timespec* ts)
{
    /* Obtaining the current time.*/
    if ((clock_gettime(CLOCK_REALTIME, ts)) != -1)
        return;
        
    /* An error occured so we are printing it to stderr and exiting the
     * program. */
    fprintf(stderr, 
            "[ %s ] ERROR: in function start_timer(): %s\n",
            timestamp(), strerror(errno));
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
        /* There was an error obtaining the time so we're printing 
         * a message to stderr and exiting the program. */
        fprintf(stderr, 
                "ERROR: In function timestamp(): "
                "Calender time is not available\n");
        exit(EXIT_FAILURE);
    }

    /* Converting time to local time format. */
    if (ctime_r(&current_time, stamp) == NULL)
    {
        /* There was an error converting the time to a string so we're
         * printing a message to stderr and exiting the program. */
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
    sfmt(&stamp_cpy, "%s", stamp);

    /* Removing the newline character that was added by ctime(). */
    sdelchar(&stamp_cpy, '\n');

    /* Returning the copy of the time stamp. */
    return stamp_cpy;
}

/******************************** In/Out *************************************/

/**
 * This function prints a string based oin the format string and argument
 * list provided to it. It outputs the string to the file stream that is
 * provided to the function. It also clears the current line of terminal that
 * the cursor is on before printing on that line. 
 */
void fprintf_clear(FILE* fs, char* fmt, ...)
{
    va_list lptr;       /* Pointer to the list of arguments. */
    va_list lptr_cpy;   /* A Copy of the list of arguments. */
    size_t bytes;       /* The number of bytes the string needs. */
    char* prompt;       /* The prompt. */

    /* Pointing to the first argument. */
    va_start(lptr, fmt);

    /* Copying the argument list. */
    va_copy(lptr_cpy, lptr);

    /* Getting the number of bytes the string will need. Adding an extra
     * 1 char worth of bytes for the null character. */
    bytes = vsnprintf(NULL, 0, fmt, lptr_cpy) + sizeof(char);
	
    /* Assuring a clean finish to the copy. */
    va_end(lptr_cpy);

    /* Allocating memory to the string. */
    prompt = (char*) malloc(bytes);

    /* Creating the string. */
    vsnprintf(prompt, bytes, fmt, lptr);

    /* Assuring a clean finish to the argument list. */
    va_end(lptr);

    /* Clearing the current line that the terminal cursor is on. */
    termclearfb();

    /* Moving the terminal cursor to the beginning of the current line it is
     * on */
    cursmv(strlen(prompt) + 1, LEFT);

    /* Printing the prompt. */
    fprintf(fs, "%s\n", prompt);

    /* Moving the cursor to the end of the line. */
    cursmv(1, UP);
    cursmv(strlen(prompt), RIGHT);

    /* De-allocating memory from the prompt. */
    free(prompt);
}

/**
 * This function prints a prompt to the user, then assigns a string that is
 * input by the user to the string pointer provided to it.
 */
void scans(char** buf, char* prompt)
{
    char* btemp;    /* Temp storage for the buffer. */
    char userin;    /* The user input. */

    /* Arbitrarily initialising to avoid invalid pointer error upon
     * initial call to free(). */
    *buf = (char*) malloc(sizeof(char));
    btemp = (char*) malloc(sizeof(char));
    btemp[0] = '\0';

    do
    {
        /* Printing the prompt and any past user input. */
        fprintf_clear(stdout, "%s%s", prompt, btemp);

        /* Getting and processing user input. */
        switch (userin = scanc_nowait())
        {
            /* Backspace. */
            case (int) 127:
                /* Removing the last character in the buffer. */ 
                sdelelem(&btemp, strlen(btemp) - 1);
                break;
            
            /* Enter. */
            case '\n':
                break;

            /* Anything else. */
            default:
                /* Recreating the buffer. */
                free(*buf);
                sfmt(buf, "%s%c", btemp, userin);

                /* Recreating the temporary copy of the buffer. */
                free(btemp);
                sfmt(&btemp, "%s", *buf);
        }
    } while(userin != '\n');

    /* De-allocating temp variable memory. */
    free(btemp);
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
    /* Closing the file stream. */
    if (fclose(fs) == 0)
        return;
    
    /* There was an error closing the file stream so we are printing it
     * on stderr and exiting the program. */
    fprintf(stderr,
            "[ %s ] ERROR: In function closefs: %s\n", 
            timestamp(), strerror(errno));
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
    FILE* fs;   /* The pointer to the file stream. */

    /* Opening the file. */
    if ((fs = fopen(fname, mode)) != NULL)
        return fs;

    /* There was an error opening the file so wea re printing the error to
     * stderr and exiting the program. */
    fprintf(stderr, 
            "[ %s ] ERROR: In function openfs(): "
            "Could not open file %s: %s\n",
            timestamp(), fname, strerror(errno));
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

    /* An error occurred so we are printing an error message and exiting 
     * the program. */
    fprintf(stderr,
            "[ %s ] ERROR: In function readfsc(): %s\n",
            timestamp(), strerror(errno));
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
            
    /* An error occurred so we are printing an error message and exiting
     * the program. */
    fprintf(stdout,
            "[ %s ] ERROR: In function readfsl: %s\n",
            timestamp(), strerror(errno));
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
 * This function dynamically allocates only the needed amount of memory to a
 * string based on the argument list, then concatenates the argument list into 
 * the supplied format and stores it in the supplied string pointer.
 */
void sfmt(char** sp, char *fmt, ...)
{
    va_list lp;     /* Pointer to the list of arguments. */
    va_list lp_cpy; /* A Copy of the list of arguments. */
    size_t bytes;   /* The number of bytes the string needs. */

    /* Pointing to the first argument. */
    va_start(lp, fmt);

    /* Copying the argument list. */
    va_copy(lp_cpy, lp);

    /* Getting the number of bytes the string will need. Adding an extra
     * 1 char worth of bytes for the null character. */
    bytes = vsnprintf(NULL, 0, fmt, lp_cpy) + sizeof(char);

    /* Assuring a clean finish to the copy. */
    va_end(lp_cpy);

    /* Allocating memory to the string. */
    *sp = (char*) malloc(bytes);

    /* Creating the string. */
    vsnprintf(*sp, bytes, fmt, lp);

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
    sfmt(sp, "%s%s", to_elem, from_elem);

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
 * This function sets the background colour of the terminal cursor.
 */
void curscolb(enum termcolours c)
{
    char* cmd;  /* The command. */

    /* Setting the background colour. */
    sfmt(&cmd, "tput setab %d", c);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void curscolf(enum termcolours c)
{
    char* cmd;   /* The command. */

    /* Setting the colour. */
    sfmt(&cmd, "tput setaf %d", c);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function moves the terminal cursor a number of rows or columns
 * equal to the number provided to the function, and in a direction that is
 * also provided.
 */
void cursmv(unsigned int n, enum directions direction)
{
    char* cmd; /* The command. */

    /* Creating the command. */
    switch (direction)
    {
        case UP:
            sfmt(&cmd, "tput cuu %d", n);
            break;
        case DOWN:
            sfmt(&cmd, "tput cud %d", n);
            break;
        case LEFT:
            sfmt(&cmd, "tput cub %d", n);
            break;
        case RIGHT:
            sfmt(&cmd, "tput cuf %d", n);
            break;
    }

    /* Moving the cursor. */
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function places the terminal at the row and column numbers
 * provided to it.
 */
void cursput(unsigned int col, unsigned int row)
{
    char* cmd;   /* The command. */

    /* Setting the cursor position. */
    sfmt(&cmd, "tput cup %d %d", row, col);
    system(cmd);

    /* Cleaning up. */
    free(cmd); 
}

/**
 * This function clears the entire terminal and positions the cursor at home.
 */
void termclear()
{
    /* Clearing the terminal and putting the cursor at home. */
    system("tput clear");
}

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's beginning.
 */
void termclearb()
{
    /* Clearing from the cursor to the begining of the line. */
    system("tput el1");
}


/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's end.
 */
void termclearf()
{
    system("tput el");
}

/**
 * This function clears the entire line that the terminal cursor is currently
 * on.
 */
void termclearfb()
{
    /* Clearing the line that the terminal cursor is currently on. */
    termclearf();
    termclearb();
}

/**
 * This function draws in the terminal based on the contents of a file.
 */
void termdrawfs(char* filepath, vec2d origin, vec2d bounds)
{
    FILE* fs;   /* Pointer to the file stream. */
    char* line; /* The text in the file. */

    /* Ensuring that the buffer is set to NULL. */
    line = NULL;
    
    /* Opening the file. */ 
    fs = openfs(filepath, "r");
   
    /* Reading the line from the file. */ 
	while (readfsl(fs, &line)) 
    {
        /* Drawing the line. */
        termdrawl(line, strlen(line), origin, bounds);

        /* Getting ready to draw the next line. */
        origin.y++;
        free(line);
        line = NULL;
    }

    /* Closing the file. */
    closefs(fs);
}

/**
 * This function draw a single row of an art file.
 */
void termdrawl(char* text, size_t text_len, vec2d origin, vec2d bounds)
{
    int c;  // The current column of the row

    /* Moving the cursor to the origin. */
    cursput(origin.x, origin.y);
    
    /* Drawing the row. */
    for (c = 0; c < text_len && c < bounds.x; c++)
    {
        /* Checking if there should be something drawn in theis column. */
        if (text[c] == '1')
        {
            /* Drawing a filled space. */
            curscolb(WHITE);
            system("printf \" \"");
        }
        else
        {
            /* Drawing an empty space. */
            system("tput cuf1");
        }
    }
    textmode(NORMAL);
}

/**
 * This function draws a string in the terminal.
 * The art files need to exist for each character.
 */
void termdraws(char* str, vec2d origin, vec2d bounds)
{
    char* filepath; // Path of the current file
    int c;          // Current letter in the string

    /* Drawing the string. */
    for (c = 0; c < strlen(str); c++)
    {
        sfmt(&filepath, "./art/%c.txt", str[c]);
        termdrawfs(filepath, origin, bounds);
        origin.x += CHAR_WIDTH;
        free(filepath);
    }
}

/**
 * This function returns the number of rows and columns of the terminal.
 */
vec2d termres()
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
 * This function changes the terminal text-mode.
 */
void textmode(enum textmodes m)
{
    switch (m) 
    {
        case BOLD       : system( "tput bold" ); break;
        case NORMAL     : system( "tput sgr0" ); break;
        case BLINK      : system( "tput blink" ); break;
        case REVERSE    : system( "tput smso" ); break;
        case UNDERLINE  : system( "tput smul" ); break;
    }
}

/**
 * This function prints the string provided to it into the the terminal
 * at the location specified by the vec2d that is also provided to the
 * function.
 */
void termprint(char* str, vec2d origin)
{
    char* cmd;  // The command

    /* Printing the string. */
    cursput(origin.x, origin.y);
    sfmt(&cmd, "printf \"%s\"", str);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

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
    struct timespec current;    // The current time
    struct timespec elapsed;    // The time elapsed since start
    bool has_elapsed = false;   // Whether the time has elapsed

    /* Obtaining the current time. */
    clock_gettime(CLOCK_REALTIME, &current);

    /* Calculating the elapsed time. */
    elapsed.tv_sec = current.tv_sec - start.tv_sec;
    elapsed.tv_nsec = current.tv_nsec - start.tv_nsec;

    /* Checking whether the time has elapsed. */
    if ((elapsed.tv_sec * NANOS_PER_SEC) + elapsed.tv_nsec >= wait_time)
    {
        has_elapsed = true; // The designated time has elapsed.
    }

    /* Returning whether the designated time has elapsed. */
    return has_elapsed;
}

/**
 * This function obtains the current time and stores it in the timespec
 * that was provided to it.
 */
void start_timer(struct timespec* ts)
{
    /* Obtaining the current time.*/
    if ((clock_gettime(CLOCK_REALTIME, ts)) == -1)
    {
        /* An error occured so we are printing it to stderr. */
        fprintf(stderr, 
                "[ %s ] ERROR: in function start_timer(): %s\n",
                timestamp(), strerror(errno));
    }
}

/**
 * This function returns a string that represent the current time.
 */
char* timestamp()
{
    time_t current_time;    // The current time
    char* stamp;            // The time stamp

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
    if ((stamp = ctime(&current_time)) == NULL)
    {
        /* There was an error converting the time to a string so we're
         * printing a message to stderr and exiting the program. */
        fprintf(stderr, 
                "ERROR: In function timestamp(): "
                "Failure to convert the current time to a string.\n");
        exit(EXIT_FAILURE);
    }

    /* Removing the newline character that was added by ctime(). */
    stringrmc(&stamp, '\n');

    /* Returning the time stamp. */
    return stamp;
}

/******************************** In/Out *************************************/

/**
 * This function prints a prompt to the user that is based on the format string
 * and argument list provided to it.
 */
void promptf(char* fmt, ...)
{
    va_list lptr;       // Pointer to the list of arguments
    va_list lptr_cpy;   // A Copy of the list of arguments
    size_t bytes;       // The number of bytes the string needs
    char* prompt;       // The prompt

    /* Pointing to the first argument. */
    va_start(lptr, fmt);

    /* Copying the argument list. */
    va_copy(lptr_cpy, lptr);

    /* Getting the number of bytes the string will need. Adding
     * 1 for the null byte. */
    bytes = vsnprintf(NULL, 0, fmt, lptr_cpy) + 1;

    /* Assuring a clean finish to the copy. */
    va_end(lptr_cpy);

    /* Allocating memory to the string. */
    prompt = (char*) malloc(bytes);

    /* Creating the string. */
    vsnprintf(prompt, bytes, fmt, lptr);

    /* Assuring a clean finish to the argument list. */
    va_end(lptr);

    /* Clearing the line the prompt is on in the terminal. */
    termclearl();
    cursputb(strlen(prompt) + 1);

    /* Printing the prompt. */
    fprintf(stdout, "%s\n", prompt);

    /* Moving cursor to the end of the line. */
    cursputu(1);
    cursputf(strlen(prompt));

    /* De-allocating memory from the prompt. */
    free(prompt);
}

/**
 * This function asks the user to input a char in response to a prompt supplied
 * to it, then stores it in the supplied char pointer. 
 */
void scans(char** buf, char* prompt)
{
    char* btemp;    // Temp storage for the buffer
    char userin;    // The user input

    /* Arbitrarily initialising to avoid crash later. */
    *buf = (char*) malloc(sizeof(char));
    btemp = (char*) malloc(sizeof(char));

    do
    {
        /* Printing a prompt to the user. */
        promptf("%s%s", prompt, btemp);

        /* Getting and processing user input. */
        switch (userin = scanc_nowait())
        {
            /* Backspace. */
            case (int) 127:
                /* Removing the last character in the buffer. */ 
                stringrmlast(&btemp);
                break;
            
            /* Enter. */
            case '\n':
                break;

            /* Anything else. */
            default:
	            /* De-allocating memory from the buffer. */
	            free(*buf);
	
	            /* Adding the latest user input to the buffer. */
	            stringf(buf, "%s%c", btemp, userin);
	
	            /* Storing the buffer temporarily. */
	            free(btemp);
	            btemp = (char*) malloc(sizeof(char) * strlen(*buf));
	            strcpy(btemp, *buf);
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
void closefs(FILE* fp)
{
    /* Closing the file stream. */
    if (fclose(fp) != 0)
    {
        /* There was an error closing the file stream so we are printing it
         * on stderr and exiting the program. */
        fprintf(stderr,
                "[ %s ] ERROR: In function close_file: %s\n", 
                timestamp(), strerror(errno));
        exit(EXIT_FAILURE);
    }
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
    FILE* fp;   // The pointer to the file stream.

    /* Opening the file. */
    if ((fp = fopen(fname, mode)) == NULL)
    {
        /* There was an error opening the file so wea re printing the error to
         * stderr and exiting the program. */
        fprintf(stderr, 
                "[ %s ] ERROR: In function open_file(): "
                "Could not open file %s: %s\n",
                timestamp(), fname, strerror(errno));
        exit(EXIT_FAILURE);
    }

    /* Returning the pointer to the file stream. */
    return fp;
}

/**
 * This function assigns the next char in the file stream provided to it to
 * the buffer provided to it. It returns true on success or false if EOF is
 * reached. It will exit the program if an error occurs.
 */
bool readfsc(FILE* fstreamp, char* buf)
{
    const bool SUCCESS = 1;     // Return value if success
    const bool END_OF_FILE = 0; // Return value if EOF
    bool status;                // Whether the char was read successfully

    /* Presuming the character was read successfully. */
    status = SUCCESS;

    /* Reading the char. */
    if ((*buf = fgetc(fstreamp)) == EOF)
    {
        /* Checking if an error occured. */
        if (ferror(fstreamp))
        {
            /* Printing an error message and exiting the program. */
            fprintf(stderr,
                    "[ %s ] ERROR: In function read_filec(): %s\n",
                    timestamp(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        /* EOF was reached. */
        status = END_OF_FILE;
    }

    /* Returning whether the char was read successfully. */
    return status;
}


/**
 * This function assigns the next line in the file stream provided to it to
 * the string provided to it. It returns true if the line was read successfully
 * or false if EOF was reached. If an error occurs the program will exit.
 * Make sure to free() the buffer when you're finished with it.
 */
bool readfsl(FILE* fstreamp, char** buf)
{
    const bool SUCCESS = 1;     // Return value if success
    const bool END_OF_FILE = 0; // Return value if EOF
    bool status;                // Whether the line was read successfully
    size_t n;                   // Allocated size of the buffer

    /* Initialising how big the buffer is. */
    n = 0;

    /* Presuming the character was read successfully. */
    status = SUCCESS;
    
    /* Reading the next line from the file. */
    if (getline(buf, &n, fstreamp) == -1)
    {
        /* Checking if an error occured. */
        if (ferror(fstreamp))
        {
            /* Printing an error message and exiting the program. */
            fprintf(stdout,
                    "[ %s ] ERROR: In function read_fileln: %s\n",
                    timestamp(), strerror(errno));
            exit(EXIT_FAILURE);
        }
        /* EOF was reached. */
        status = END_OF_FILE;
    }

    /* Returning whether the line was read successfully. */
    return status;
}

/**
 * This function writes the char provided to it to the file stream provided to
 * it.
 */
void writefsc(FILE* fstreamp, char ch)
{
    /* Writing the char to the file stream. */
    fprintf(fstreamp, "%c", ch); 
}

/**
 * This function writes the string provided to it to the file steam provided
 * to it.
 */
void writefss(FILE* fstreamp, char* str)
{
    int c;  // Index of the current char in the string

    /* Writing the string to the file stream. */
    for (c = 0; c < strlen(str); c++)
        writefsc(fstreamp, str[c]);
}

/******************************** Strings ************************************/

/**
 * This function dynamically allocates only the needed amount of memory to a
 * string based on the argument list, then concatenates the argument list into 
 * the supplied format and stores it in the supplied string pointer.
 */
void stringf(char** sptr, char *fmt, ...)
{
    va_list lptr;       // Pointer to the list of arguments
    va_list lptr_cpy;   // A Copy of the list of arguments
    size_t bytes;       // The number of bytes the string needs

    /* Pointing to the first argument. */
    va_start(lptr, fmt);

    /* Copying the argument list. */
    va_copy(lptr_cpy, lptr);

    /* Getting the number of bytes the string will need. Adding
     * 1 for the null byte. */
    bytes = vsnprintf(NULL, 0, fmt, lptr_cpy) + 1;

    /* Assuring a clean finish to the copy. */
    va_end(lptr_cpy);

    /* Allocating memory to the string. */
    *sptr = (char*) malloc(bytes);

    /* Creating the string. */
    vsnprintf(*sptr, bytes, fmt, lptr);

    /* Assuring a clean finish to the argument list. */
    va_end(lptr);
}

/**
 * This function removes all cases of the provided char from the string at the
 * provided pointer.
 */
void stringrmc(char** str, char remove)
{
    int len = strlen(*str);   // The original length of the string.
    int total_chars = strlen(*str);   // The current length of the string.
    int i; // An indexer
    char* src; // The address of where to start moving the memory.
    char* dst; // The address of where to move the memory to.

    /* Overwriting the unwanted character. */
    for (i = 0; i < len; i++)
    {
        if ((*str)[i] == remove)
        {
            /* Setting the source and destinations points for moving. */
            src = &((*str)[i + 1]);
            dst = &((*str)[i]);

            /* Overwriting an unwanted character. */
            memmove(dst, src, 
                    (sizeof(char) * strlen(*str)) - (sizeof(char) * i));

            /* Decrementing the index so we will check the replacement 
             * character. */
            i--;

            /* Recording the new length of the string. */
            total_chars--;
        }
    }

    /* Designating the end of the string. */
    (*str)[total_chars] = '\0';
}

/**
 * This function removes the last character from a string before the null
 * character.
 */
void stringrmlast(char** s)
{
    char* temps;
    int c;

    /* Checking if string isn't empty. */
    if (strlen(*s) > 0)
    {
	    temps = (char*) malloc(sizeof(char) * strlen(*s));
	    strcpy(temps, *s);
	    free(*s);
	    *s = (char*) malloc(sizeof(char) * strlen(temps) - sizeof(char));
	    for (c = 0; c < strlen(temps) - 1; c++)
	    {
	        (*s)[c] = temps[c];
	    }
	    (*s)[c] = '\0';
	    free(temps);
    }
}

/******************************* Terminal ************************************/

/**
 * This function sets the background colour of the terminal cursor.
 */
void curscolb(enum termcolours c)
{
    char* cmd;  // The command

    /* Creating the command. */
    stringf(&cmd, "tput setab %d", c);

    /* Setting the background colour. */
    system(cmd);

    /* De-allocating memory. */
    free(cmd);
}

/**
 * This function sets the foreground colour of the temrinal cursor.
 */
void curscolf(enum termcolours c)
{
    char* cmd;   // The command

    /* Creating the command. */
    stringf(&cmd, "tput setaf %d", c);
    
    /* Setting the foreground colour. */
    system(cmd);

    /* De-allocating memory. */
    free(cmd);
}

/**
 * This function places the terminal at the row and column numbers
 * provided to it.
 */
void cursput(unsigned int col, unsigned int row)
{
    char* cmd;   // The command

    /* Creating the command. */
    stringf(&cmd, "tput cup %d %d", col, row);

    /* Setting the cursor position. */
    system(cmd);

    /* De-allocating memory. */
    free(cmd); 
}

/**
 * This function moves the cursor left by the number of columns provided
 * to it.
 */
void cursputb(unsigned int ncols)
{
    char* cmd;  // The command

    /* Moving the cursor. */
    stringf(&cmd, "tput cub %d", ncols);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function moves the cursor right by the number of columns provided
 * to it.
 */
void cursputf(unsigned int ncols)
{
    char* cmd;  // The command

    /* Moving the cursor. */
    stringf(&cmd, "tput cuf %d", ncols);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function moves the cursor up by the number of rows provided
 * to it.
 */
void cursputu(unsigned int nrows)
{
    char* cmd;  // The command

    /* Moving the cursor. */
    stringf(&cmd, "tput cuu %d", nrows);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

/**
 * This function clears the terminal.
 */
void termclear()
{
    /* Clearing the terminal. */
    system( "tput clear" );
}

/**
 * This function clears the current line the terminal cursor is on from
 * the position of the cursor to the line's beginning.
 */
void termclearl()
{
    system("tput el1");
}

/**
 * This function draws in the terminal base on the contents of a file.
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
        stringf(&filepath, "../../art/%c.txt", str[c]);
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
    res.x = atoi(rbuf); //strtol( cbuf, &end, 10 );
    res.y = atoi(cbuf); //strtol( rbuf, &end, 10 );

    /* Closing the files. */
    closefs(rfp);
    closefs(cfp);

    /* Deleting the files. */
    system("rm temp/screen_rows.txt");
    system("rm temp/screen_cols.txt");

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
    stringf(&cmd, "printf \"%s\"", str);
    system(cmd);

    /* Cleaning up. */
    free(cmd);
}

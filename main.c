/**
 * main.c
 *
 * This file gives examples of using the functions in the mycutils library
 *
 * Version: 1.0.1
 * Author: Richard Gale
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "mycutils.h"

int main()
{
    FILE* fs;                   /* File stream. */
    struct timespec my_timer;   /* Stores a time. */
    uint64_t nanos_per_frame;   /* The number of nanoseconds per frame. */
    bool is_running;            /* Whether the program is running. */
    char* filename;             /* Name of the file. */
    char* filetext;             /* Text to write to a file. */
    char* tstamp;               /* Timestamp we need to free(). */
    char* userin;               /* User input. */
    char* prompt;               /* User prompt. */
    int framecount;             /* Counts how many frames have happened. */

    /* Sixty frame per second. */
    nanos_per_frame = NANOS_PER_SEC / 60;

    /* Getting a filename from the user. */
    scans(&userin, "Write a name for the file: ");

    /* Creating a file name. */
    strfmt(&filename, "%s", userin);
    free(userin); 
    fs = openfs(filename, "w");

    /* Program will loop indefinitely. */
    is_running = true;

    /* Getting the start time. */
    start_timer(&my_timer);

    /* No frames have past yet. */
    framecount = 0;

    /* Running the loop. */ 
    while (is_running)
    {
        /* Checking if it's time to run a frame. */
        if (check_timer(my_timer, nanos_per_frame))
        {
            /* Recording this frame. */
            framecount++;

            /* Creating the text to write to the file. */
            strfmt(&filetext, "Frame number %d at %s\n", 
                    framecount, (tstamp = timestamp()));

            /* Writing the text to the file. */
            writefss(fs, filetext);

            /* Printing the text. */
            fprintf(stdout, "%s", filetext);

            /* Freeing memory. */
            free(filetext);
            free(tstamp); /* See timestamp() for details on freeing this. */

            /* Checking if we should end the loop. */
            if (framecount == 5)
                is_running = false;

            /* Restarting the timer. */
            start_timer(&my_timer); 
        }
    } 

    /* Closing the file. */
    closefs(fs);

    /* Printing message to user. */
    fprintf(stdout, "Please review file: %s\n", filename);

    /* Freeing memory. */
    free(filename);
   
    /* Exiting the program. */ 
    exit(EXIT_SUCCESS);
}

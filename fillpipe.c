/* Watches specified directory for files,
   appends files to fifo, moves processed
   files to subdir done/ */

#include "recat.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define FIFO_NAME "american_maid"

int main(int argc, char **argv) {
    int i, fd_out;
    DIR * d;
    char * dir_name;

    char done_dir[] = "done";

    if(argc != 2) {
        fprintf(stderr, "Invalid directory\n");
        return 1;
    }

    dir_name = argv[1];

    mknod(FIFO_NAME, S_IFIFO | 0666, 0);

    //printf("waiting for readers...\n");
    fd_out = open(FIFO_NAME, O_WRONLY | O_APPEND); //removing o_append for testing
    //printf("got a reader\n");

    if(!(d = opendir(dir_name))) {
        fprintf(stderr, "Could not open directory '%s': %s\n", dir_name, strerror(errno));
        return 1;
    }

    if(chdir(dir_name) != 0) {
        fprintf(stderr, "Could not cd\n");
        return 1;
    }

    mkdir(done_dir, S_IRWXU | S_IRWXG);

    while(1) {
        struct dirent * entry;
        char * entry_name;
        char * new_entry_name;
        int buffsize;

        entry = readdir(d);
        if(!entry) {
            rewinddir(d);
            sleep(1);
            //break;
        } else {
            entry_name = entry->d_name;

            if(strcmp(entry_name, ".") != 0
              && strcmp(entry_name, "..") != 0
              && strcmp(entry_name, done_dir) != 0) {

                buffsize = strlen(done_dir) + 1 + strlen(entry_name) + 1;
                new_entry_name = malloc(buffsize);

                snprintf(new_entry_name, buffsize, "%s/%s", done_dir, entry_name);
                printf("Wrote %s: %d bytes\n", new_entry_name, buffsize);

                if(catio(entry_name, fd_out) != 0) {
                    fprintf(stderr, "Write error\n");
                }

                if(rename(entry_name, new_entry_name) != 0) {
                    fprintf(stderr, "Could not move %s\n", new_entry_name);
                }
                free(new_entry_name);
                //sleep(10); //testing

            }

        }
    }

    if(closedir(d) != 0) {
        fprintf(stderr, "Could not close '%s': %s\n", dir_name, strerror(errno));
        return 1;
    }
    close(fd_out);
    return 0;

}


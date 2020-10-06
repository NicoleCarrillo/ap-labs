#define _XOPEN_SOURCE 500   /* See feature_test_macros(7) */
#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#include <sys/inotify.h>
#include <stdio.h>
#include "logger.h"
#include <limits.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <ftw.h>
int inotifyFd, wd;
int status;
struct stat st_buf;
struct inotify_event *event;

static int display_info(const char *fpath, const struct stat *sb,int tflag, struct FTW *ftwbuf){
    if(tflag==FTW_D){
        wd = inotify_add_watch(inotifyFd, fpath, IN_CREATE | IN_DELETE | IN_MOVE_SELF | IN_MOVED_TO);
        if (wd == -1)
            errorf("INOTIFY NOT WORKING");
    }
    return 0;
}

static void displayInotifyEvent(struct inotify_event *i)
{
    if (i->mask & IN_CREATE){
        infof("CREATED %s\n",i->name);
    }        
    if (i->mask & IN_DELETE){
        infof("DELETED %s\n",i->name);
    }       
    if (i->mask & IN_MOVED_FROM){
        infof("RENAMING %s ",i->name);
    }   
    if (i->mask & IN_MOVED_TO){
        infof("TO %s\n", i->name);
    }

}

int main(int argc, char *argv[]){
    int j;
    char buf[BUF_LEN] __attribute__ ((aligned(8)));
    ssize_t numRead;
    char *p;
   
    if (argc < 2 || strcmp(argv[1], "--help") == 0)     /* Error handling */
        errorf("%s\n","pathname...", argv[0]);
    
    inotifyFd = inotify_init();                         /* Create inotify instance */
    
    if (inotifyFd == -1)                                /* Error handling */
        errorf("%s\n","inotify_init");   


    status = stat (argv[1], &st_buf);

    if (status != 0) {
        errorf ("%d\n","Error,= name handling ");
        return 0;
    }



    int flags = 0;                                      /* To obtain a list of all of these subdirectories, you will need to make use of nftw(). */
    if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;
    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags)== -1) {
        errorf("%s\n","nftw");
    }



    for (;;) {                                          /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            panicf("%s\n","read() from inotify fd returned 0!");
        if (numRead == -1)
            errorf("%s\n","read");
        for (p = buf; p < buf + numRead; ) {            /* Process all of the events in buffer returned by read() */
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);
            p += sizeof(struct inotify_event) + event->len;
        }
    }   
    return 0;
}
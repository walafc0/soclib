#include <pthread.h>
#include <stdint.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "fb_controller.h"

static uint8_t *FrameBuffer;
static int inited = 0;
static int map_fd;
static pid_t screen_pid;
static uint32_t width, height;
static int control_fd;

static const int bpp = 32;

void* fb_init(int32_t w, int32_t h)
{
	char tmpname[] = "/tmp/fb_screen.XXXXXX";

    if ( inited )
        return;
    inited = 1;

	width = w;
	height = h;
    
	{
		mkstemp(tmpname);

		map_fd = open(tmpname, O_RDWR|O_CREAT|O_TRUNC, 0644);
		if ( map_fd < 0 ) {
			perror("open");
			exit(1);
		}

		lseek(map_fd, width*height*2, SEEK_SET);
		write(map_fd, "", 1);
		lseek(map_fd, 0, SEEK_SET);
	
		FrameBuffer = (uint8_t*)mmap(0, width*height*2, PROT_WRITE|PROT_READ, MAP_FILE|MAP_SHARED, map_fd, 0);
		if ( FrameBuffer == ((uint8_t *)-1) ) {
			perror("mmap");
			exit(1);
		}
	
		memset(FrameBuffer, 128, width*height*2);
	}
	
	{
		char wstr[8];
		char hstr[8];
		char *args[] = {"soclib-fb", wstr, hstr, "420", tmpname, NULL};
		int fds[2];

		pipe(fds);
		control_fd = fds[1];

		snprintf(wstr, sizeof(wstr), "%u", width);
		snprintf(hstr, sizeof(hstr), "%u", height);

		screen_pid = fork();
		if ( !screen_pid ) {
			close(fds[1]);
			close(0);
			dup2(fds[0], 0);
			execvp("soclib-fb", &args[0]);
			perror("soclib-fb");
			kill(getppid(), SIGKILL);
		} else {
			close(fds[0]);
		}
	}
	atexit(fb_cleanup);
	return FrameBuffer;
}

void fb_update(void)
{
	write(control_fd, "", 1);
}

void fb_cleanup(void)
{
	close(control_fd);
	kill(screen_pid, SIGKILL);
}

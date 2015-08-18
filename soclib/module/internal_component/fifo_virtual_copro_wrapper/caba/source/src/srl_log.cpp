#include "../include/srl_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <string.h>
#include <fstream>
#include <iostream>
//#include <unistd.h>

namespace dsx { namespace caba {

/* SRL LOG */
int outfd;
int verbosity;

int srl_log_open() {
    const char * verbosities[VERB_MAX+1] = {
        "NONE",
        "TRACE",
        "DEBUG",
         NULL,
    };
    const char * env_verb = getenv("SRL_VERBOSITY");
    const char * verb = env_verb ? env_verb : "NONE" ;

    int i;
    for (i = 0; verbosities[i]; ++i) {
        if (!strcasecmp(verbosities[i], verb)) {
            verbosity = i;
            break;
        }
    }
    outfd = open( SRL_TRACE_FILE, O_WRONLY|O_CREAT|O_TRUNC, 0644 );
    if (outfd < 0) {
        perror("Cant open SRL trace file");
        outfd = 0;
        return -1;
    }
    srl_log(TRACE, "SRL started\n");
    return 0;
}


void _srl_log(int level, const char * msg) {
    if (level > verbosity) {
        return;
    }
    write(2, msg, strlen(msg));
    write(outfd, msg, strlen(msg));
}


void _srl_log_printf(int level, const char * fmt, ...) {
    va_list args;
    char buffer[LOG_BUFSZ];
    
    if (level > verbosity) {
        return;
    }
    va_start(args, fmt);
    vsnprintf(buffer, LOG_BUFSZ, fmt, args);
    va_end(args);
    _srl_log(level, buffer);
}

}}

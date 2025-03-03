/*
Copyright (C) 2000-2005 Silicon Graphics, Inc. All Rights Reserved.
Portions Copyright (C) 2008-2021 David Anderson.  All Rights Reserved.
Redistribution and use in source and binary forms, with
or without modification, are permitted provided that the
following conditions are met:

    Redistributions of source code must retain the above
    copyright notice, this list of conditions and the following
    disclaimer.

    Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials
    provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Usage:  ./test_errmsg_list.c -f /path.../libdwarf.h */

#include "config.h"
#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>  /* For exit(), strtoul() declaration etc. */
#endif
#if defined(_WIN32) && defined(HAVE_STDAFX_H)
#include "stdafx.h"
#endif /* HAVE_STDAFX_H */
#ifdef HAVE_STRING_H
#include <string.h>  /* strcpy() strlen() */
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#include "libdwarf_private.h"
#include "dwarf.h"
#include "libdwarf.h"
#include "dwarf_base_types.h"
#include "dwarf_opaque.h"
#include "dwarf_errmsg_list.h"

/* This is just to help localize the error. */
static void
printone(int i)
{
    int arraysize = sizeof(_dwarf_errmsgs) / sizeof(char *);
    if ( i >= arraysize) {
        printf("%d is outside the array! Missing something!\n",i);
    } else {
        printf("%d is: %s\n",i,_dwarf_errmsgs[i]);
    }
}

/* Arbitrary. A much smaller max length value would work. */
#define MAX_NUM_LENGTH 12

/* return TRUE on error */
static int
check_errnum_mismatches(unsigned i)
{
    unsigned nextstop = 0;
    const char *sp = _dwarf_errmsgs[i];
    const char *cp = sp;
    unsigned innit = FALSE;
    unsigned prevchar = 0;
    unsigned value = 0;

    for ( ; *cp; cp++) {
        unsigned c = 0;
        c = 0xff & *cp;
        if ( c >= '0' && c <= '9' && !innit
            && prevchar != '(') {
            /* Skip. number part of macro name. */
            prevchar = c;
            continue;
        }
        if ( c >= '0' && c <= '9') {
            value = value * 10;
            value += (c - '0');
            nextstop++;
            if (nextstop > MAX_NUM_LENGTH) {
                break;
            }
            innit = TRUE;
        } else {
            if (innit) {
                break;
            }
            prevchar= c;
        }
    }
    if (innit) {
        if (i != value) {
            return TRUE;
        }
        return FALSE;
    }
    /* There is no number to check. Ignore it. */
    printf("mismatch value %d has no errnum to check %s\n",
        i,_dwarf_errmsgs[i]);
    return TRUE;
}

/* We don't allow arbitrary DW_DLE line length. */
#define MAXDEFINELINE 200

static int
splmatches(char *base, unsigned baselen,char *test)
{
    if (baselen != strlen(test) ) {
        return FALSE;
    }
    for ( ; *test; ++test,++base) {
        if (*test != *base) {
            return FALSE;
        }
    }
    return TRUE;
}

static void
check_dle_list(const char *path)
{
    /*  The format should be
        #define<space>name<spaces>number<spaces>optional-c-comment
        and we are intentionally quite rigid about it all except
        that the number of spaces before any comment is allowed. */
    char buffer[MAXDEFINELINE];
    unsigned linenum = 0;
    unsigned cur_dle_line = 0;
    unsigned long prevdefval = 0;
    unsigned foundlast = 0;
    unsigned foundlouser = 0;
    FILE*fin = 0;

    fin = fopen(path, "r");
    if (!fin) {
        printf("Unable to open define list to read %s\n",path);
        exit(1);
    }
    for ( ;;++linenum) {
        char *line = 0;
        unsigned linelen = 0;
        char *  curdefname = 0;
        char *  pastname = 0;
        unsigned curdefname_len = 0;
        char *numstart = 0;
        char * endptr = 0;
        unsigned long v = 0;

        line = fgets(buffer,MAXDEFINELINE,fin);
        if (!line) {
            break;
        }
        linelen = strlen(line);
        if (linelen >= (unsigned)(MAXDEFINELINE-1)) {
            printf("define line %u is too long!\n",linenum);
            exit(1);
        }
        if (strncmp(line,"#define DW_DLE_",15)) {
            /* Skip the non- DW_DLE_ lines */
            continue;
        }
        curdefname = line+8;
        /* ASSERT: line ends with NUL byte. */
        for ( ; ; curdefname_len++) {
            if (foundlouser) {
                printf("define line %u has  stuff after "
                    "DW_DLE_LO_USER!\n",
                    linenum);
                exit(1);
            }
            pastname = curdefname +curdefname_len;
            if (!*pastname) {
                /* At end of line. Missing value. */
                printf("define line %u of %s: has no number value!\n",
                    linenum,path);
                exit(1);
            }
            if (*pastname == ' ') {
                /* Ok. Now look for value. */
                numstart = pastname + 1;
                break;
            }
        }
        /* strtoul skips leading whitespace. */
        v = strtoul(numstart,&endptr,0);
        /*  This test is a bit odd. But is valid till
            we decide it is inappropriate. */
        if (v > DW_DLE_LO_USER) {
            printf("define line %u: number value unreasonable. %lu\n",
                linenum,v);
            exit(1);
        }
        if (v == 0 && endptr == numstart) {
            printf("define line %u of %s: number value missing.\n",
                linenum,path);
            printf("Leaving a space as in #define A B 3"
                " in libdwarf.h.in will cause this.\n");
            exit(1);
        }
        if (*endptr != ' ' && *endptr != '\n') {
            printf("define line %u: number value terminates oddly\n",
                linenum);
            exit(1);
        }
        if (splmatches(curdefname,curdefname_len,"DW_DLE_LAST")) {
            if (foundlast) {
                printf("duplicated DW_DLE_LAST! line %u\n",linenum);
                exit(1);
            }
            foundlast = 1;
            if (v != prevdefval) {
                printf("Invalid: Last value mismatch! %lu vs %lu\n",
                    v,prevdefval);
            }
        } else if (splmatches(curdefname,curdefname_len,
            "DW_DLE_LO_USER")) {
            if (!foundlast) {
                printf("error:expected DW_DLE_LO_USER after LAST! "
                    "line %u\n", linenum);
                exit(1);
            }
            if (foundlouser) {
                printf("Error:duplicated DW_DLE_LO_USER! line %u\n",
                    linenum);
                exit(1);
            }
            foundlouser = 1;
            continue;
        } else {
            if (cur_dle_line > 0) {
                if (v != prevdefval+1) {
                    printf("Invalid: Missing value! %lu vs %lu\n",
                        prevdefval,v);
                    exit(1);
                }
            }
            prevdefval = v;
        }
        ++cur_dle_line;
        /* Ignoring rest of line for now. */
    }
    fclose(fin);
}

char pathbuf[2000];
static void
safe_strcpy(char *targ,char *src,unsigned targlen, unsigned srclen)
{
    if (srclen > targlen) {
        printf("Target name does not fit in buffer.\n"
            "In test_errmsg_list.c increas buffer size "
            " from %u \n",(unsigned int)sizeof(pathbuf));
        exit(1);
    }
    strcpy(targ,src);
}

/*   ./test_errmsg_list.c -f /path.../libdwarf.h */
int
main(int argc, char **argv)
{
    unsigned arraysize = sizeof(_dwarf_errmsgs) / sizeof(char *);
    unsigned i = 0;
    char *path = 0;
    unsigned len = 0;
    const char *libpath="/src/lib/libdwarf/libdwarf.h";
    pathbuf[0] = 0;

    if (argc > 1) {
        if (argc != 3) {
            printf("Expected -f <filename> of libdwarf.h\n");
            exit(1);
        }
        if (strcmp(argv[1],"-f")) {
            printf("Expected -f\n");
            exit(1);
        }
        path=argv[2];
     } else {
        /* env var should be set with base path of code */
        path = getenv("DWTOPSRCDIR");
        if (!path) {
            printf("Expected environment variable "
                " DWTOPSRCDIR with path of "
                "base directory (usually called 'code')\n");
            exit(1);
        }
        len = strlen(path);
        safe_strcpy(pathbuf,path,sizeof(pathbuf),len);
        safe_strcpy(pathbuf+len,(char *)libpath,
             sizeof(pathbuf) -len -1,(unsigned)strlen(libpath));
        path = pathbuf;
    }
    check_dle_list(path);

    if (arraysize != (DW_DLE_LAST + 1)) {
        printf("Missing or extra entry in dwarf error strings array"
            " %u expected DW_DLE_LAST+1 %d\n",
            arraysize, DW_DLE_LAST+1);
        printone(1);
        printone(100);
        printone(200);
        printone(250);
        printone(260);
        printone(262);
        printone(263);
        printone(264);
        printone(265);
        printone(273);
        printone(274);
        printone(275);
        printone(300);
        printone(328);
        exit(1);
    }
    for ( i = 0; i <= DW_DLE_LAST; ++i) {
        if (check_errnum_mismatches(i)) {
            printf("mismatch value %d is: %s\n",i,_dwarf_errmsgs[i]);
            exit(1);
        }
    }
    /* OK. */
    return 0;
}

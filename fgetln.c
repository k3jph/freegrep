
/* fgetline:  Read one line of input and return a pointer to
   that line.  Necessary space is obtained from malloc.
   (char *) NULL is returned on EOF.

    Andy Dougherty              doughera@lafcol.lafayette.edu
    Dept. of Physics
    Lafayette College, Easton PA 18042

    Successive calls to fgetline() overwrite the original buffer.
    If you want to preserve the data, you must do something like
    the following (the +1's are for '\0' characters):

        tmp = fgetline(fp);
        ntmp = Ealloc(strlen(tmp)+1, sizeof(char));
        strncpy(ntmp, tmp, strlen(tmp)+1);

    A line is defined as all characters up to (and including) the next
    newline character or end of file.
    The string is terminated by a NULL character.

 * Version 1.1   A. Dougherty  2/7/94
   Don't call realloc() just to save a few bytes.
   Check return value from realloc().  (NULL is allowed under POSIX,
   though I've never hit it.)

 * Version 1.0  A. Dougherty  2/27/91

 This fgetline implementation was written by Andrew Dougherty
 <doughera@lafayette.edu>.  I hearby place it in the public domain.
 As a courtesy, please leave my name attached to the source.

 This code comes with no warranty whatsoever, and I take no
 responsibility for any consequences of its use.
*/

/* Algorithm:  A local static buffer "buf" is maintained.  The current
   length (space available) is in the static variable "avail".
   Read in characters into this buffer.  If need more space, call
   malloc().

   Aside:  We have computed strlen(buf) in this function.  It
   seems a shame to throw it away.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define LINELEN 128 /* A decent guess that should only rarely be
                        overwritten.
                    */
#define OK_TO_WASTE 512 /* Don't bother trying to realloc() back to
                           a smaller buffer if you'd only end up
                           wasting OK_TO_WASTE bytes.
                        */

void *Emalloc(size_t len) /* David */
{
        char *p;

        p=malloc(len);
        if (p == NULL) {
                perror("out of memory (Emalloc)");
                exit(2);

        }
        return p;
}

void *Erealloc(char *p, size_t len) /* David */
{
        p=realloc(p, len);
        if (p == NULL) {
                perror("out of memory (Erealloc)");
                exit(2);

        }
        return p;
}

char *
fgetln(FILE *fp, size_t *length)
  {
    static char *buf = NULL;
    static size_t avail = 0;
    int c;
    char *p; /* Temporary used for reducing length. */
    int len;

    if (avail == 0)
      {
        buf = (char *) Emalloc(LINELEN * sizeof(char));
        avail = LINELEN;
      }

    len = 0; /* Current length */

    while ((c=getc(fp)) != EOF)
      {
        if (len >= avail) /* Need to ask for space */
          {
            avail += LINELEN;  /* Maybe avail *= 2 would be better */
            buf = (char *) Erealloc((void *) buf, avail * sizeof(char));
          }
        buf[len] = c;
        len++;
        if (c == '\n')
            break;
      }

    if (c == EOF && len == 0)
        return (char *) NULL;

    /* Add terminating '\0' character */
    if (len >= avail)  /* Need 1 more space */
        buf = (char *) Erealloc((void *) buf, (len+1) * sizeof(char));
    buf[len] = '\0';

    /* Should we bother to try reclaiming memory?  (Otherwise, this
       function will hold onto enough memory to hold the longest
       line for the entire duration of the program.)
    */
    if (avail - len  > OK_TO_WASTE)
      {
        p = (char *) Erealloc((void *) buf, (len+1) * sizeof(char));
        if (p != NULL)
          {
            buf = p;
            avail = len + 1;
          }
      }
    *length=len-1;
    return buf;
  }

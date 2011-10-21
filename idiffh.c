/* Ian Cottam, Manchester, May/June/July 2008; Donated to the Public Domain. No warranty of any kind. */



/*
 * Written to the C99 standard; there should not be anything UNIX
 * specific. But, if you -DUNIX when you compile, it will catch SIGINT
 * and delete the temporary files. I have tried a previous version on
 * Windows (mingw and lcc compilers), Linux (gcc) and Mac OS X (gcc,
 * icc) where it was developed. See below for Windows caveat.
 *
 * If you are stuck with an older C compiler or run-time library,
 * there are two things that may need changing:
 * (a) "%zu" formats may not be supported, change to "%lu" or "%llu"
 *     and cast the size_t values; and
 * (b) declarations and statements are mixed up (like C++), create
 *     extra blocks { ... } or move the declarations.
 * The first of these is likely with any Windows-based compiler (for
 * some reason); the second is likely to be OK.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#ifdef FGETLN
#include <bsd/stdio.h>
#endif

typedef int CompareFunc_t(const char *s1, const char * s2);
typedef CompareFunc_t *PtrCompareFunc_t;
/* normal compare is just builtin strcmp */
static  PtrCompareFunc_t comparefunction= strcmp;

static int  wcompare(const char *s1, const char * s2);
static int  bcompare(const char *s1, const char * s2);
static char *fgetline(FILE*);
static void exception(int failure, const char *fmt, ...);
static int  checkinsertdelete(const size_t bound, char *line1, char *line2,
			      FILE *filen, char *direction, size_t *count,
			      FILE *filem);
static void printtempfile(char *ed, size_t count1, size_t count2,
			  size_t f2count1, size_t f2count2);
static int  synconnextline(FILE *file1, FILE *file2);
static void handlechangeline(size_t lcount, size_t f2lcount,
			     char *line1, char *line2);
static void outputremainder(FILE *file1, FILE *file2, char *line1, char *line2,
			    size_t savedlcount, size_t lcount,
			    size_t savedf2lcount, size_t f2lcount);

/* flags */
static int bflag= 0;    /* -b */
static int wflag= 0;    /* -w */

/* holds the possible output for an insertion or deletion */
static FILE *temp;
/* holds c lines from file1 */
static FILE *tempc1;
/* holds c lines from file2 */
static FILE *tempc2;

/* line count for file1 */
static size_t lcount= 0;
/* line count for file2 */
static size_t f2lcount= 0;

/* temp file names */
static char filetempname1 [L_tmpnam];
static char cfile1tempname[L_tmpnam];
static char cfile2tempname[L_tmpnam];

/* Usage */
static const char *usage = "usage: idiffh [-b|-w] [-zNumber] file1 file2";
static const char *usage1 =
  "usage: idiffh [-b|-w] [-zNumber] file1 file2\n"
  "options:\n"
  "-b = ignore differences in amount of white space when comparing lines\n"
  "-w = ignore all white space when comparing lines\n"
  "-zNUmber = set the lookahead limit to Number (defaults to 100 lines).\n"
  "           100=<Number=<1000\n"
  "(The -z is more for development, but the odd user may find it helpful.)\n"
  "If one looks at the GNU diff documentation, there isn't a very formal\n"
  "definition for -b and -w, so you may see some behaviour that differs a\n"
  "little from diff. I'm simply not sure which is correct. Treat them as a\n"
  "help, when needed, not a necessity.\n"
  "\n"
  "idiffh is a diffh that may be useful for enormous text files and,\n"
  "perhaps, few differences. I.e., files that, e.g., GNU diff cannot cope\n"
  "with. Note that you are just lucky if you get minimal differences; the\n"
  "simple heuristics used do not guarantee such -- the price you pay for\n"
  "being able to compare enormous files of any length.\n"
  "\n"
  "There are no arbitrary limits, such as a maximum line length or file\n"
  "size, but there is a necessary limit on the \"diff size\", i.e., the\n"
  "amount of lines to readahead (to play with it: supply as -zNumber).\n"
  "The 'best' value for the readahead bound depends on the input files. It\n"
  "defaults to 100 lines, which should be reasonable in many cases. Text\n"
  "files are assumed.\n";

/* UNIX like systems can clean up temp files on control-c */
#ifdef UNIX
#include <signal.h>
static void catchint(int signum) { remove(filetempname1); remove(cfile1tempname); remove(cfile2tempname); exit(signum?2:2); }
#define controlc()  signal(SIGINT, catchint)
#else
#define controlc()
#endif





int main(int argc, char **argv)
{
  size_t savedlcount= 0;	/* for restoring lcount if we backtrack */
  size_t savedf2lcount= 0;	/* for restoring f2lcount if we backtrack */
  char *line1, *line2;		/* current lines from file1 and file2 */
  fpos_t backtrackpoint;	/* file1 or file2 point to backtrack to */
  int rv= 0;			/* 0 OK and no diffs, 1 OK and diffs, 2 trouble */
  int ex;			/* for checking return codes */
  unsigned int bounded= 100;	/* bound for readahead - may be supplied via -zNumber */
  unsigned int f1= 0, f2= 0;	/* argv[f1] is first filename, argv[f2] is second */

  errno= 0;
  { /* Process arguments */
    int i;
    exception(argc < 3, usage1);
    for (i= 1; i != argc; ++i) {
      if (argv[i][0] == '-') {
	/* handle optional switches */
	switch (argv[i][1]) {
	default:
	  exception(1, usage);
	  break;
	case 0:
	  exception(1, "sorry, standard input can't be used because we need random file access");
	  break;
	case 'b':
	  exception(strcmp(argv[i], "-b") != 0, usage);
	  bflag= 1;
	  comparefunction= bcompare;
	  break;
	case 'w':
	  exception(strcmp(argv[i], "-w") != 0, usage);
	  wflag= 1;
	  comparefunction= wcompare;
	  break;
	case 'z': {
	  unsigned int j;
	  char *limit= &argv[i][2];
	  unsigned int len= strlen(limit);
	  for (j= 0; j != len; ++j)
	    exception(!isdigit(limit[j]),
		      "non digit supplied in -zNumber: %s",
		      argv[i]);
	  ex= sscanf(limit, "%u", &bounded);
	  exception(ex == EOF, usage);
	  exception(bounded < 100 || bounded > 1000,
		    "limit to readahead supplied with -zNumber cannot be <100 or >1000");
	  break;
	 }
	}
      } else {
	/* should be exactly two filenames or error */
	if (f1 == 0) f1= i; else if (f2 == 0) f2= i; else exception(1, usage);
      }
    }
    /* very silly to supply both -b and -w */
    exception(bflag && wflag, usage);
    /* must have seen precisely two filenames */
    exception(f2 == 0, usage);
  }

  FILE *file1= fopen(argv[f1], "r"); exception(file1 == 0, "can't open %s", argv[f1]);
  FILE *file2= fopen(argv[f2], "r"); exception(file2 == 0, "can't open %s", argv[f2]);

  /* create temporary file names */
  tmpnam(filetempname1); tmpnam(cfile1tempname); tmpnam(cfile2tempname);

  /* clean up on interrupt if UNIX-like */
  controlc();

  /* print any line diffs until one or both files exhausted */
  while (line1= fgetline(file1), line2= fgetline(file2), (line1 != 0 && line2 != 0)) { /* main loop */
    ++lcount, ++f2lcount;

    if ((*comparefunction)(line1, line2) != 0) { /* i.e. the lines differ */
      rv= 1;
      /* remember file2 position before we check for insert */
      savedf2lcount= f2lcount;
      ex= fgetpos(file2, &backtrackpoint);     exception(ex != 0, "failed to set backtrack position in %s", argv[f2]);
      if (checkinsertdelete(bounded, line1, line2, file2, ">", &f2lcount, file1))
         printtempfile("a", lcount-1, lcount-1, savedf2lcount, f2lcount-1);
      else {
	/* restore file2 pos */
	ex= fsetpos(file2, &backtrackpoint);   exception(ex != 0, "failed to restore backtrack position in %s", argv[f2]); 
        f2lcount= savedf2lcount;
	/* remember file1 position before we check for deletion */
	savedlcount= lcount; 
	ex= fgetpos(file1, &backtrackpoint);   exception(ex != 0, "failed to set backtrack position in %s", argv[f1]);
	if (checkinsertdelete(bounded, line1, line2, file1, "<", &lcount, file2))
           printtempfile("d", savedlcount, lcount-1, savedf2lcount-1, f2lcount-1);
	else  {
	  /* neither insertion nor deletion as far as we can tell with our given readahead; restore file1 pos */
	  ex= fsetpos(file1, &backtrackpoint); exception(ex != 0, "failed to restore backtrack position in %s", argv[f1]);
	  lcount= savedlcount;
	  /* just declare it a changed line */
	  handlechangeline(lcount, f2lcount, line1, line2);
	}
      }
    }
    free(line1); free(line2);
  }

  exception(line1 == 0 && !feof(file1), "sorry, looks like out of memory or error reading %s", argv[f1]);
  exception(line2 == 0 && !feof(file2), "sorry, looks like out of memory or error reading %s", argv[f2]);

  /* flush any remaining c lines */
  handlechangeline(0, 0, "", "");  

  /* remaining lines if any - to be added or deleted at end */
  if (line1 != 0 || line2 != 0) { rv= 1; outputremainder(file1, file2, line1, line2, savedlcount, lcount, savedf2lcount, f2lcount); }

  remove(filetempname1); remove(cfile1tempname); remove(cfile2tempname);
  return rv; 
}



/* Auxiliary functions */


/*
 * One file has more lines than the other - output a or d lines as appropriate
 */
static void outputremainder(FILE *file1, FILE *file2, char *line1, char *line2, size_t savedlcount, size_t lcount, size_t savedf2lcount, size_t f2lcount)
{
  char *line= (line1 == 0) ? line2 : line1; int ex;
  errno= 0;
  temp= fopen(filetempname1, "w"); exception(temp == 0, "can't open temporary file for writing: %s", filetempname1);
  fputs(line, temp); free(line);
  if (line1 == 0) { /* means it is an "a" of lines from file2; need to buffer up lines to count them */
    savedf2lcount= f2lcount;
    while (line= fgetline(file2), line != 0) { ++f2lcount; fputs(line, temp); free(line); }
  } else {          /* means it is a  "d" of lines from file1; need to buffer up lines to count them */
    savedlcount= lcount;
    while (line= fgetline(file1), line != 0) { ++lcount; fputs(line, temp); free(line); }
  }
  exception(!feof(line1 == 0 ? file2 : file1), "sorry, looks like out of memory or error reading extra lines");
  exception(errno != 0, "sorry looks like I/O failure while attempting to process extra lines");
  ex= fclose(temp);  exception(ex != 0, "failed to close temporary file successfully: %s", filetempname1);
  if (line1 == 0) printtempfile("a", lcount, lcount, savedf2lcount+1, f2lcount+1); else printtempfile("d", savedlcount+1, lcount+1, f2lcount, f2lcount);
}


/*
 * batch up consecutive c lines
 * if consecutive, add to temp files
 * if not consecutive, flush current and restart batch
 * if called as handlechangeline(0,0,"","") it is just to flush the batch
 */
static void handlechangeline(size_t lcount, size_t f2lcount, char *line1, char *line2)
{
  static size_t firstl1= 0, firstl2= 0, lastl1= 0, lastl2= 0; int ex;
  errno= 0;
  /* case of first time through and not a flush call */
  if (firstl1 == 0 && lcount != 0) {
    firstl1= lcount; lastl1= lcount; firstl2= f2lcount; lastl2= f2lcount;
    tempc1= fopen(cfile1tempname, "w+"); exception(tempc1 == 0, "failed to open temp file: %s", cfile1tempname);
    tempc2= fopen(cfile2tempname, "w+"); exception(tempc2 == 0, "failed to open temp file: %s", cfile2tempname);
    fprintf(tempc1, "< %s", line1); fprintf(tempc2, "> %s", line2);
    exception(errno != 0, "sorry looks like I/O failure handling a c line");
    return;
  }
  /* case of consecutive c lines */
  if (lastl1+1 == lcount) {
    ++lastl1; ++lastl2;
    fprintf(tempc1, "< %s", line1); fprintf(tempc2, "> %s", line2);
    exception(errno != 0, "sorry looks like I/O failure handling a c line");
    return;
  }
  /* case of non consecutive c line, so there may be some to print */
  /* guard below is for when we call, to flush, with no ourstanding c lines */
  if (firstl1 != 0)  {
      char *l1, *l2;
      if (firstl1 == lastl1) printf("%zuc%zu\n", firstl1, firstl2); else printf("%zu,%zuc%zu,%zu\n", firstl1, lastl1, firstl2, lastl2);
      rewind(tempc1);
      while (l1= fgetline(tempc1), l1 != 0) { fputs(l1, stdout); free(l1); }
      exception(!feof(tempc1), "error reading temp file: %s", cfile1tempname);
      fputs("---\n", stdout);
      rewind(tempc2);
      while (l2= fgetline(tempc2), l2 != 0) { fputs(l2, stdout); free(l2); }
      exception(errno != 0, "sorry looks like I/O failure handling a c line");
      exception(!feof(tempc2), "error reading temp file: %s", cfile2tempname);
      ex= fclose(tempc1);  exception(ex != 0, "failed to close temporary file successfully: %s", cfile1tempname);
      ex= fclose(tempc2);  exception(ex != 0, "failed to close temporary file successfully: %s", cfile2tempname);
      firstl1= 0; firstl2= 0; lastl1= 0; lastl2= 0;
      /* recurse to first case  */
      handlechangeline(lcount, f2lcount, line1, line2);
   }
}


/*
 * returns 1 iff an insertion or deletion, 0 otherwise
 * give up after bound readaheads
 */
static int checkinsertdelete(const size_t bound, char *line1, char *line2, FILE *filen, char *direction, size_t *count, FILE *filem)
{
  size_t readahead= 0; int foundline= 1; char *line; int status= 0; int ex;
  errno= 0;
  /* open tmp file to write possible output too */
  temp= fopen(filetempname1, "w"); exception(temp == 0, "can't open temporary file for writing: %s", filetempname1);
  fputs(direction[0] == '>' ? line2 : line1, temp);
  do {
    /* try reading ahead to find a line that does match in file-n */
    while (++readahead, 
          line= fgetline(filen), 
          line != 0 && (foundline= comparefunction((direction[0] == '>') ? line1 : line2, line), foundline != 0) && readahead != bound) {
      *count += 1;
      fputs(line, temp);
      free(line);
    }
    exception(line == 0 && !feof(filen), "sorry, looks like out of memory or error reading file");
    if (readahead == bound && foundline != 0 && line != 0) fputs(line, temp);
    if (line != 0) {
      *count += 1;
      free(line);
      if (foundline == 0 && synconnextline(filen, filem)) { /* peek ahead another line to double check sync point */
	status= 1;
	readahead= bound;
      }
    }
  } while (readahead < bound && line != 0);
  ex= fclose(temp);  exception(ex != 0, "failed to close temporary file successfully: %s", filetempname1);
  exception(errno != 0, "sorry looks like I/O failure processing a/d chunk");
  return status;
 }


/* 
 * return 1 iff we can also sync the files
 * on the next line after already finding
 * one match (with checkinsertdelete); 0 otherwise
 */
static int synconnextline(FILE *file1, FILE *file2)
{
  char *line1, *line2;
  fpos_t backtrackpoint1, backtrackpoint2;
  int status= 0, ex;
  ex= fgetpos(file1, &backtrackpoint1);   exception(ex != 0, "failed to set backtrack point");
  ex= fgetpos(file2, &backtrackpoint2);   exception(ex != 0, "failed to set backtrack point");
  line1= fgetline(file1); line2= fgetline(file2);
  if (line1 != 0 && line2 != 0) status= comparefunction(line1, line2) == 0; else status= 1;
  if (line1 != 0) free(line1);
  if (line2 != 0) free(line2);
  ex= fsetpos(file1, &backtrackpoint1);   exception(ex != 0, "failed to reset backtrack point");
  ex= fsetpos(file2, &backtrackpoint2);   exception(ex != 0, "failed to reset backtrack point");
  return status;
}


/*
 * print the tempfile to stdout
 */
static void printtempfile(char *ed, size_t count1, size_t count2, size_t f2count1, size_t f2count2)
{
  char *line; int ex;
  handlechangeline(0, 0, "", "");  /* first flush any previous c lines */
  errno= 0;
  temp= fopen(filetempname1, "r"); exception(temp == 0, "error opening temporary file for reading: %s", filetempname1);
  if (count1 != count2)  printf("%zu,%zu%s", count1, count2, ed); else printf("%zu%s", count1, ed);
  if (f2count1 != f2count2) printf("%zu,%zu\n", f2count1, f2count2); else printf("%zu\n", f2count1);
  while (line= fgetline(temp), line != 0) { printf("%s%s", (ed[0] == 'a') ? "> " : "< ", line); free(line); }
  exception(errno != 0, "sorry looks like I/O failure attempting to print a/d chunk");
  exception(!feof(temp), "out of memory or error reading temporary file: %s", filetempname1);
  ex= fclose(temp);  exception(ex != 0, "failed to close temporary file successfully: %s", filetempname1);
}


/*
 * Prints error message, tidies up and exits
 */
static void exception(int failure, const char *fmt, ...)
{
  va_list args;
  if (!failure) return;
  if (errno != 0) perror("idiffh (library routine reporting error)");
  va_start(args, fmt);
  fprintf(stderr, "idiffh: ");
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
  remove(filetempname1);
  remove(cfile1tempname);
  remove(cfile2tempname);
  exit(2);
}


/*
 * Compare two strings for equality ignoring white space
 * returns 0 iff match.
 * Mimic diff -w.
 */
static int wcompare(const char *s1, const char * s2)
{
  /* we assume most lines are equal, so we use strcmp first */
  if (strcmp(s1, s2) == 0) return 0;
  /* lines differ, so we have to look at white space */
  size_t i= 0, j= 0;
  for (;;) {
    while(isspace(s1[i])) ++i;
    while(isspace(s2[j])) ++j;
    if (s1[i] != s2[j]) return 1;
    if (s1[i] == '\0' || s2[j] == '\0') break;
    ++i, ++j;
  }
  return !(s1[i] == s2[j]);
}


/*
 * Compare two strings for equality ignoring differences in white space
 * returns 0 iff match.
 * Mimic diff -b.
 */
static int bcompare(const char *s1, const char * s2)
{ 
  /* we assume most lines are equal, so we use strcmp first */
  if (strcmp(s1, s2) == 0) return 0;
  /* lines differ, so we have to look at white space */
  size_t i= 0, j= 0;
  for (;;) {
    if (isspace(s1[i])) {
      if (!isspace(s2[j])) return 1;
      ++i, ++j;
      while(isspace(s1[i])) ++i;
      while(isspace(s2[j])) ++j;
    }
    if (s1[i] != s2[j]) return 1;
    if (s1[i] == '\0' || s2[j] == '\0') break;
    ++i, ++j;
  }
  return !(s1[i] == s2[j]);
}


/*
 *  returns a pointer to the start of a line read
 *  from f, or the null pointer on any error or eof.
 *  Like fgets(), you may well want to get rid of the
 *  \n at the end (if any), and check feof(f)! This is
 *  not a wonderful interface, but it mimics fgets().
 *  Caller should free memory.
 */

#ifdef FGETLN
/* May well be the case that only BSD based systems (including Mac OS X) have fgetln() */

static char *fgetline(FILE *f)
{
  size_t len;
  char *line= fgetln(f, &len);
  if (line == 0) return 0;
  char *buf= malloc(len+1);
  if (buf == 0) return 0;
  memcpy(buf, line, len);
  buf[len]= '\0';
  return buf;
}
#endif

#ifndef FGETLN
# define FGETLINE_BUFFSIZE 256
static char *fgetline(FILE *f)
{
  int ch;
  size_t length= 0;
  size_t buffer_length= FGETLINE_BUFFSIZE;
  char *new, *buffer= malloc(buffer_length);
  if (buffer == 0) return 0;
  while (ch= getc(f), ch != EOF) {
    buffer[length]= (char)ch;
    ++length;
    if (length == buffer_length) {
      buffer_length += FGETLINE_BUFFSIZE;
      if (new= realloc(buffer, buffer_length), new == 0) { free(buffer); return 0; }
      buffer= new;
    }
    if (ch == '\n') break;
  } /* ch == EOF || ch == '\n' */
  /* check for EOF */
  if (length == 0 || ferror(f)) { free(buffer); return 0; }
  buffer[length]= '\0';
  return buffer;
}
#endif

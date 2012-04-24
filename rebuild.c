#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// untokenises tokened input, emitting complete standard text, with
// whitespaces (possibly empty) following each word.

// Example, input:

// :%32%32
// The:%32
// quick:
// ,:%32
// brown:%13
// fox:%32%32
// jumps:
// !:%32
// over:%13

// generated output:

//   The quick, brown
// fox  jumps! over

// Note: compile with -DNDEBUG to turn off debugging chatter.

void
usage ()
{
  fprintf (stderr,
	   "Tokenize stdin, stdout is one token/line, whitespace encoded.\n");
  fprintf (stderr, "Usage: rebuild < file-to-tokenize > token-file\n");
  fprintf (stderr, "Example: (echo a man, a; echo plan)|rebuild\n");
}

inline int
putss (char *s)
{
  return fputs (s, stdout);
}


int
main (int argc, char *argv[])
{
  if (argc != 1)
    {
      usage ();
      exit (1);
    }

  char temp = 0;
  int ch;

  while ((ch = getchar ()) != EOF)
    {
#ifndef NDEBUG
     // fprintf (stderr, "(%c)", ch);
#endif
	
	if(ch == '3' && temp == '1'){
		putchar ('\n');
	}

	else if(ch == '2' && temp == '3'){
               	putchar (' ');
	}

	else if(ch == '9' && temp == '%'){
		putchar ('\t');
	}
	
     	else if(isalpha(ch)){
		putchar (ch);
     		
	}

	else if(ch == '\n'){

	}
	
	else if(ch == ':' && temp == ':'){
		putchar(temp);
	}

	else if(ch == ':' && temp == '%' ){
		putchar(temp);
	}

	else if(ch == ':'){

	}
	
	else if(ch == '%'){

	}

	else if(ch == '3' && temp == '%'){

	}

	else if(ch == '1' && temp == '%'){

	}

	else if(ch == ' '){

	}

	else{
		putchar (ch);
	}
      temp = ch;
    }
}

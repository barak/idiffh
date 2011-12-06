#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// tokenises standard input, emitting one line/token, with trailing
// whitespace (possibly empty) following each token represented by a
// colon followed by percent-encoded ascii.

// Example, input:

//   The quick, brown
// fox  jumps! over

// generated output:

// :%32%32
// The:%32
// quick:
// ,:%32
// brown:%13
// fox:%32%32
// jumps:
// !:%32
// over:%13

<<<<<<< HEAD
main(int argc, char *argv[]) {
int x = 0;
while (x < 3){
 		 
	FILE *original;
	FILE *words;
	 char ch ;
	original = fopen(argv[x+1], "r"); 
        if(x == 0){
		words = fopen("/home/user/words3", "w");
	}
	if(x == 1){
		words = fopen("/home/user/words4", "w");
	}
	if(x == 2){	
		words = fopen("/home/user/words5", "w");
	}
fseek(original, 0, SEEK_END); // seek to end of file
int size = ftell(original); // get current file pointer
fseek(original, 0, SEEK_SET);
char temp;
int i = 0;
		
	//need to add extra loop for the amount of files.
	  while ( i<size )// Need to change to lenght of file!
	        {
	             ch= fgetc ( original ) ;
			printf("%c", ch);
	               if ( ch == EOF ){
	                      break ;
			}
			else if(ch == '\t' && (isalpha(temp))){
				fputs(": ", words);
				fputs("%9", words);
			}
			else if(i == 0 && (isalpha(ch))){
				fputc(ch, words);
				}
			else if((isalpha(temp)) && ch == '\n'){
				fputs(": ", words);
				fputs("%13", words);
				fputc('\n', words);
				
			}
			else if(!(isalpha(temp)) && ch == '\n'){
				fputs(": ", words);
				fputs("%13", words);
				fputc('\n', words);
				
			}
			else if(temp == ' ' && ch == '\n'){
				fputs("%13", words);
				fputc('\n', words);
				
			}
			else if(ch == '\n'){
				fputs(": ", words);
				fputs("%13", words);
				fputc('\n', words);
				
			}
			else if(temp == '\n' && isalpha(ch)){
				fputc(ch, words);
			}
			else if(temp == '\n' && (!isalpha(ch))){
				fputc(ch, words);
			}
	               else if(temp == ' ' && (isalpha(ch))){
              
	            	 	fputc('\n', words);
	            		fputc(ch, words);
	               }
			
		       else if((!(isalpha(temp))) && (isalpha(ch))){
				fputs(": ", words);
				fputc ('\n', words);
				fputc (ch, words);
 		       }
	               else if(isalpha(ch)){
	            		fputc(ch, words);
	               }

	               else if((isalpha(temp)) && ch == ' '){           	   
			   fputs(": " , words);
                           fputs("%32", words);
				
	               }
			else if((!(isalpha(temp))) && temp != ' ' && ch == ' '){
				 fputs(": " , words);
                          	 fputs("%32", words);
				}
                       else if (ch == ' '){
                	fputs("%32", words);
                       }
		       else if((!(isalpha(ch))) && temp == ' '){
			  fputc ('\n', words);
			  fputc ( ch, words );
			   

		       }
		      
		       else if((isalpha(temp)) && (!(isalpha(ch)))){
			fputs(": ", words);
			fputc ('\n', words);
			fputc ( ch, words );
	            	 
 		       }
		       else if(!(isalpha(temp)) && (!(isalpha(ch)))){
			fputs(": ", words);			
			fputc ('\n', words);
			fputc ( ch, words );
	            	 
 		       }
			
                       else{
	            	   fputc ( ch, words );
	               }

	        temp = ch;
		i++;
	        }
		


	        fclose ( original ) ;
            fclose(words);
		x++;
		}
=======
// Note: compile with -DNDEBUG to turn off debugging chatter.

void
usage ()
{
  fprintf (stderr,
	   "Tokenize stdin, stdout is one token/line, whitespace encoded.\n");
  fprintf (stderr, "Usage: separate < file-to-tokenize > token-file\n");
  fprintf (stderr, "Example: (echo a man, a; echo plan)|separate\n");
}

inline int
putss (char *s)
{
  return fputs (s, stdout);
>>>>>>> 7ba30e665100683ffd07a8a26575934e5e75a706
}


int
main (int argc, char *argv[])
{
  if (argc != 1)
    {
      usage ();
      exit (1);
    }

  char temp;
  int i = 0;
  int ch;

  while ((ch = getchar ()) != EOF)
    {
#ifndef NDEBUG
      fprintf (stderr, "(%c)", ch);
#endif
      if (ch == '\t' && (isalpha (temp)))
	{
	  putss (": ");
	  putss ("%9");
	}
      else if (i == 0 && (isalpha (ch)))
	{
	  putchar (ch);
	}
      else if ((isalpha (temp)) && ch == '\n')
	{
	  putss (": ");
	  putss ("%13");
	  putchar ('\n');

	}
      else if (!(isalpha (temp)) && ch == '\n')
	{
	  putss (": ");
	  putss ("%13");
	  putchar ('\n');

	}
      else if (temp == ' ' && ch == '\n')
	{
	  putss ("%13");
	  putchar ('\n');

	}
      else if (ch == '\n')
	{
	  putss (": ");
	  putss ("%13");
	  putchar ('\n');

	}
      else if (temp == '\n' && isalpha (ch))
	{
	  putchar (ch);
	}
      else if (temp == '\n' && (!isalpha (ch)))
	{
	  putchar (ch);
	}
      else if (temp == ' ' && (isalpha (ch)))
	{
	  putchar ('\n');
	  putchar (ch);
	}

      else if ((!(isalpha (temp))) && (isalpha (ch)))
	{
	  putchar ('\n');
	  putchar (ch);
	}
      else if (isalpha (ch))
	{
	  putchar (ch);
	}

      else if ((isalpha (temp)) && ch == ' ')
	{
	  putss (": ");
	  putss ("%32");

	}
      else if ((!(isalpha (temp))) && temp != ' ' && ch == ' ')
	{
	  putss (": ");
	  putss ("%32");
	}
      else if (ch == ' ')
	{
	  putss ("%32");
	}
      else if ((!(isalpha (ch))) && temp == ' ')
	{
	  putchar ('\n');
	  putchar (ch);


	}

      else if ((isalpha (temp)) && (!(isalpha (ch))))
	{
	  putchar ('\n');
	  putchar (ch);

	}
      else if (!(isalpha (temp)) && (!(isalpha (ch))))
	{
	  putchar ('\n');
	  putchar (ch);

	}

      else
	{
	  putchar (ch);
	}

      temp = ch;
      i++;
    }
}

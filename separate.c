
#include <stdio.h>
#include <ctype.h>

main(int argc, char *argv[]) {
int x = 0;
while (x < 2){
 		 
	FILE *original;
	FILE *words;
	 char ch ;
	original = fopen(argv[x+1], "r"); 
        if(x == 0){
		words = fopen("/home/user/words3", "w");
	}
	else{
		words = fopen("/home/user/words4", "w");
		
		
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
			fputc ('\n', words);
			fputc ( ch, words );
	            	 
 		       }
		       else if(!(isalpha(temp)) && (!(isalpha(ch)))){
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
}


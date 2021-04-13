#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define true 1
#define false 0

#define PI 3.141592653589793

#define MAXIMUMLENGHT 512
#define MAXPRECISION 20
#define DEFAULTPRECISION 5
#define FUNCTIONSEPARATOR "|"

#ifndef NAN
#define NAN (0.0/0.0)
#endif 

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif


//******************************************************************************
struct token {
	char symbol;
	char typeOfTok;
	char *numString;
	unsigned int numStringCount;
	float numVal;
	struct token *next;
};


float stringToFloat(char*);
void removeToken(struct token**);
void makeNewToken(struct token**, struct token**);
struct token* convertToToken(char*);
unsigned int precedenceOperator(char);
struct token* shuntingYardAlgorithm(struct token*);
float calculations(float, float, char);
struct token* revList(struct token*, struct token*);
struct token* rpn(struct token*);
int error(char* given);
int errorCheckBacktoBack(char* given);


float stringToFloat (char* numString) {
	unsigned int start;
	float fractalPartOfString=0;
	int numPartOfString = 0;
	unsigned int i;	

    // check if the number is negative or positive 
	if(numString[0] != '-'){
		start=0;
	}else{
		start=1;
	}
	

	int point = -1;
	for ( i = start; i < strlen(numString); i++ ) {
        // enters here on the first iteration, checks if first or the second character is  a decimal point
		// if it is a decimal point, 0 gets assigned to it and it skips an interation
		if(point<0){
			if ( numString[i] == '.' ) {
			point = 0;
			continue;
			}
		}
		// second iteration onwards, it comes here, makes sure that the next character in the array is not a decimal, and
		// if it is indeed not, iterates point and skips to the next iteration each time it does that
		if ( point >= 0 ) {
			if ( numString[i] != '.' ){
				point=point+1;
			}
			continue;
		}
        // stores the number part of the decimal inside this variable
		numPartOfString=numString[i]-48+(numPartOfString*10);
	}
	
	
	for ( i = (int) strlen(numString) - 1;point > 0; i--, point--) {

		if ( i > 1 ){
			if(numString[i] == '.' ){
				i=i-1;
			}
		}
		float PowerVar = pow(10, 0 - point);
		// subtract 48 to get an integer, because thats how ASCII works
		float AsciiSub = (numString[i] - 48);
		//Adds the calculated decimal to 0, so that we have the final decimal
		fractalPartOfString = fractalPartOfString+AsciiSub*PowerVar;
	}
	// adds the number and fraction part of the string together
	fractalPartOfString = fractalPartOfString+numPartOfString;
	float FinalString = 0;
	// if the final output is negative, subtract 0 to give a negative result 
	if (start==1){
		fractalPartOfString=0-fractalPartOfString;
	}
	// returns the answer in the form of a float 
	return fractalPartOfString;
}

void removeToken (struct token** first) {
	struct token *placeHolder = *first;
	while ( placeHolder ) {
		placeHolder = NULL;
		free(*first);
		*first = placeHolder;
	}
}
void makeNewToken (struct token** first, struct token** last) {

	struct token *placeHolder = malloc(sizeof(struct token));
	(*placeHolder).typeOfTok = 'n';
	(*placeHolder).numString = NULL;
	(*placeHolder).numStringCount = 0;
	(*placeHolder).numVal = 0.0;
	(*placeHolder).next = NULL;
	
	if ( *first ) {
		(*last)->next = placeHolder;
		*last = (*last)->next;
	}
	else {
		*first = placeHolder;
		*last = placeHolder;
	}
}
struct token* convertToToken (char *term) {
/*
	char symbol;
	char typeOfTok;
	char *numString;
	unsigned int numStringCount;
	float numVal;
	struct token *next;

*/
	struct token *first = NULL;
	struct token *last = NULL;
	
	struct token *finalDigit = NULL;
	
	int countForBracket = 0;
	int countForNm = 0;
	struct token *placeHolder = NULL;
	
	char actual_typeOfTok = 'o';
	for ( unsigned int i = 0; i < strlen(term); i++ ) {
		
		if ( (term[i] < 48 || term[i] > 57) && term[i] != '.' && term[i] != '+' 
		&& term[i] != '-' && term[i] != '*' && term[i] != '/' && term[i] != '(' && term[i] != ')' && term[i] != '^' ){
			continue;
		}

		// assign the value of the token type for last to actual_typePfTok	
		if (last!=NULL){
			actual_typeOfTok = (*last).typeOfTok;
		}
			
		// If its an operator, and if its an opening bracket enter the if 
		if ( (term[i] >= 48 && term[i] <= 57) | term[i] == '.' | (term[i] == '-' 
		& (actual_typeOfTok == 'o' | actual_typeOfTok == '(')) ) {
			// if the type of token is not a number, we make a new token - where first and last are null
			// during the first iteration countfor num is 0 so it enters the second if for the first time and returns first 
			if ( actual_typeOfTok != 'n' ) {
				makeNewToken(&first, &last);
				finalDigit = last;
				countForNm++;
				if ( !(countForNm == 1) ) {
					removeToken(&first);
					makeNewToken(&first, &last);
					return first;
				}
			}
			
			(*last).numStringCount++;
			if ( (*last).numStringCount == 1 ){// for the first iteration of the for loop it enters here and allocates memory to last
				(*last).numString = malloc(2 * sizeof(char));
			}else{// second iteration onwards it reallocates the memory 
				(*last).numString = realloc((*last).numString, ((*last).numStringCount + 1) * sizeof(char));
			}
			// i dont get these two lines 	
			(*last).numString[(*last).numStringCount - 1] = term[i];// adds to the stack 
			(*last).numString[(*last).numStringCount] = '\0';// points to the null character
			continue;
		}
		// makes new token 
		makeNewToken(&first, &last);
		
		// this creates token for oprators 
		if ( term[i] == '+' || term[i] == '-' || term[i] == '*' || term[i] == '/'|| term[i] == '^' ) {
			countForNm--;
			if ( countForNm ) {
				removeToken(&first);
				makeNewToken(&first, &last);
				return first;
			}
			(*last).symbol = term[i];
			(*last).typeOfTok = 'o';
			continue;
		}
		// this litrally just replaces - with -1*, so -(4) will be relaced by -1*4 and creates the token for said feature 
		if ( term[i] == '(' || term[i] == ')' ) {
			if ( actual_typeOfTok == 'n' && term[i] == '(' && finalDigit ) {
				if ( (*finalDigit).numStringCount == 1 && (*finalDigit).numString[0] == '-' ) {
					(*finalDigit).numStringCount++;
					(*finalDigit).numString = realloc((*finalDigit).numString, 3 * sizeof(char));
					(*finalDigit).numString[1] = '1';
					(*finalDigit).numString[2] = '\0';
					(*last).symbol = '*';
					(*last).typeOfTok = 'o';
					countForNm--;
					makeNewToken(&first, &last);
				}
			}
			//adds symbol and the token type of the character to the last pointer 
			(*last).symbol = term[i];
			(*last).typeOfTok = term[i];
			// counts the brackets and add to countForBracket

			if(term[i] == '('){
				countForBracket += 1; 
			}else{
				countForBracket += -1; 
			}

			// if there are no brackets then return the "first" - new token was made previously - this exist out
			if ( countForBracket < 0 ) {
				removeToken(&first);
				makeNewToken(&first, &last);
				return first;
			}
		}
	}
	
	// if the totoal numbers are more than 1 make new tokens 
	if ( countForNm != 1 ) {
		removeToken(&first);
		makeNewToken(&first, &last);
		return first;
	}
	// if there are brackets make tokens 
	if ( countForBracket != 0 ) {
		removeToken(&first);
		makeNewToken(&first, &last);
		return first;
	}
	
	placeHolder = first;
	// if the temporary placeholder that we populatd earlier is not null - which it is not - this goes in 
	while ( placeHolder ) {
		// this is for decimal and converting to decimals 
		if ( (*placeHolder).typeOfTok == 'n' ) {
			(*placeHolder).numVal = stringToFloat((*placeHolder).numString);
			free((*placeHolder).numString);
		}
		placeHolder = (*placeHolder).next;
	}
	// if first is not null - which its nto - makes new token 
	if ( !first ){
		makeNewToken(&first, &last);
	}

	return first;
}

unsigned int precedenceOperator (char symbol) {
	if(symbol == '+'||symbol == '-'){
		return 2; 
	}else if(symbol == '*'||symbol =='/'){
		return 3;
	}else if(symbol=='^'){
		return 4;
	}else{
		return 0;
	}
	
}

struct token* shuntingYardAlgorithm (struct token* given) {
	
	struct token *initialSymbol = NULL;
	struct token *finalAns = NULL;
	struct token *finalSymbol = NULL;
	
	struct token *next = NULL;
	struct token *placeHolder = NULL;
	
	while (given!=NULL) {
		// assigning the argument to next 
		next = (*given).next;
		(*given).next = NULL;
		
		// checks if the argument's given token is new  
		if ( (*given).typeOfTok == 'n' ) {
			if (!(initialSymbol==NULL)){
				(*finalAns).next = given;
				finalAns = (*finalAns).next;
			}
			else { // this is where it enters for the first iteration 
				// assign given argument;s values to initialSymbol and finalAns 
				initialSymbol = given;
				finalAns = given;
			}
		}

		// if the gievn argument is an operator 
		if ( (*given).typeOfTok == 'o' ) {
			//checks that there is an operator symbol, goes inside the precendence function to check the precendence - this gets skipped
			// for first iteraton 
			while ( finalSymbol!=NULL && precedenceOperator((*finalSymbol).symbol) > precedenceOperator((*given).symbol) ) {
				placeHolder = finalSymbol;
				finalSymbol = (*finalSymbol).next;
				(*finalAns).next = placeHolder;
				finalAns = (*finalAns).next;
			}// while loop ends 

			if (finalSymbol!=NULL) {
				(*given).next = finalSymbol;
				finalSymbol = given;
			}
			else{// this is what's exwcuted for first iteration, 
				finalSymbol = given;
			}
				
		}

		// if the given argument is an opening bracket it goes in and populates finalSymbol with the given argument
		if ( (*given).typeOfTok == '(' ) {
			if ( finalSymbol!=NULL ) {
				(*given).next = finalSymbol;
				finalSymbol = given;
			}
			else
				finalSymbol = given;
		}
		// if the tokn has a closing braces ntr here
		if ( (*given).typeOfTok == ')' ) {
			// go through everything between the brackets 
			while (finalSymbol!=NULL && (*finalSymbol).typeOfTok != '(') {
				placeHolder = finalSymbol;
				finalSymbol = (*finalSymbol).next;
				(*finalAns).next = placeHolder;
				finalAns = (*finalAns).next;
			}
			if ( finalSymbol ) {
				placeHolder = finalSymbol;
				finalSymbol = (*finalSymbol).next;
				
			}
			free(given);
			free(placeHolder);
		}
		
		given = next;
	}
	
	while (finalSymbol!=NULL ) {
		placeHolder = finalSymbol;
		finalSymbol = (*finalSymbol).next;
		(*finalAns).next = placeHolder;
		finalAns = (*finalAns).next;
	}


	(*finalAns).next = NULL;
	
	return initialSymbol;
}

float calculations (float first, float second, char symbol) {

	if(symbol == '+'){
		return (first + second);
	}else if(symbol == '-'){
		return (first - second);
	}else if(symbol == '*'){
		return (first * second);
	}else if(symbol == '^'){
		return pow(first, second);
	}else{
		if ( !second )
				return 0;
		return (first / second);
	}

}
struct token* revList (struct token* previous, struct token* element) {
	
	struct token *next = (*element).next;
	(*element).next = previous;
	
	if ( !next )
	{
		return element;
	}
		return revList(element, next);
}




struct token* rpn (struct token* calculated) {
	
	struct token *reversed, *first, *remaining, *second, *symbol, *placeHolder;
	
	if (!(*calculated).next){
			return calculated;
	}else if(!calculated  ){
		return calculated; 
	}
		
	remaining = (*calculated).next;

	for (placeHolder = calculated;	!((*placeHolder).typeOfTok == 'o'); remaining = (*remaining).next ) {
		placeHolder = remaining;
	
	}
	
	(*placeHolder).next = NULL;
	reversed = revList(NULL, calculated);
	
	while ( (*reversed).next ) {
		symbol = reversed;
		second = (*reversed).next;
		first = (*second).next;
		(*first).numVal = calculations((*first).numVal, (*second).numVal, (*symbol).symbol);
		reversed = first;
		free(symbol);
		free(second);
		while ( remaining ) {
			placeHolder = remaining;
			remaining = (*remaining).next;
			(*placeHolder).next = reversed;
			reversed = placeHolder;
			if ( (*placeHolder).typeOfTok == 'o' )
				break;
		}
	}
	
	return calculated;
}





int error(char* given){
	//char firstBracket = '('; 
	//char secondBracket = ')'; 
	int count, count1  = 0 ; 
	for (int i =0; i<strlen(given); i++){
		if(given[i] == '('){
			count++;
		}
	}
	for (int i =0; i<strlen(given); i++){
		if(given[i] == ')'){
			count1++;
		}
	}
	if (count  == count1){
		return 0; 
	}else{
		return 1; 
	}
}

int errorCheckBacktoBack(char* given){
    int count; 
	for (int i =0; i<strlen(given); i++){
		if(given[i] == '+' && given[i+1] == '+'){
			printf("INVALID");
			return 0;
			exit(0);
		}else if(given[i] == '+' && given[i+1] == '-'){
          printf("INVALID");
          return 0;
          exit(0);
        }else if(given[i] == '+' && given[i+1] == '*'){
          printf("INVALID");
          return 0;
          exit(0);
        }
      else if(given[i] == '+' && given[i+1] == '/'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '-' && given[i+1] == '+'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '-' && given[i+1] == '-'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '-' && given[i+1] == '*'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '-' && given[i+1] == '/'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '/' && given[i+1] == '/'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '/' && given[i+1] == '-'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '/' && given[i+1] == '+'){
          printf("INVALID");
          return 0;
          exit(0);      
      }else if(given[i] == '/' && given[i+1] == '*'){
          printf("INVALID");
          return 0;
          exit(0);
      }else if(given[i] == '*' && given[i+1] == '/'){
          printf("INVALID");
          return 0;
          exit(0);    
      }else if(given[i] == '*' && given[i+1] == '-'){
          printf("INVALID");
          return 0;
          exit(0);    
      }else if(given[i] == '*' && given[i+1] == '+'){
          printf("INVALID");
          return 0;
          exit(0);    
      }else if(given[i] == '*' && given[i+1] == '*'){
          printf("INVALID");
          return 0;
          exit(0);    
      }else if(given[i] == '/' && given[i+1] == '0'){
          printf("INFINITY\n");
          return 0;
          exit(0);
      }
	}
	  return 1; 
}





int main (int argc, const char * argv[]) {
	// makes a character array with the length specified above
	char userInput[MAXIMUMLENGHT];

	// if the argument is more tahn 1 then it enters the loop
	if ( argc > 1 ) {

		for ( unsigned int i = 1; i < argc; i++ ) {
			// if there is only one argument - adds ot to the userInput character array
			if ( i == 1 )
				strcpy(userInput, argv[i]);
			else {
				// concatenates the arguments and adds it to the raw term character
				// array
				strcat(userInput, argv[i]);
			}
		}
	}
	else {
		// gets the input 
		fgets(userInput, MAXIMUMLENGHT, stdin);
		printf( "\n\n" );
	}

    while(true){
        if (error(userInput) == 0){
		break;
	}else if(error(userInput) == 1){
        printf( "INVALID INPUT\n");
		exit(0);
	}
    }

    if(errorCheckBacktoBack(userInput)==0){
        printf( "Exiting program ......");
        exit(0);
    }
	
	
	
	// covert the given string to token and assign it to the tokens variable
	struct token *tokens = convertToToken(userInput);

	
	printf( "\n\n" );
	
	tokens = shuntingYardAlgorithm(tokens);
	
	printf( "\n\n" );
	
	struct token *result = rpn(tokens);
	if ( (*result).numVal == (int) (*result).numVal )
		printf( "Result: %d", (int) (*result).numVal );
	else
		printf( "Result: %f", (*result).numVal );
	
	printf( "\n\n" );
	
	
	removeToken(&tokens);
	
	
	return 0;
}
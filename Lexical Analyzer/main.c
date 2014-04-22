#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define KEYWORD_LIST_SIZE 39
char* keywordList[]= {"ASSIGN","BACKSPACE","BLOCK DATA","CALL","CLOSE","COMMON","CONTINUE","DATA","DIMENSION","DO","ELSE","ELSE IF","END","ENDFILE","ENDIF","ENTRY","EQUIVALENCE","EXTERNAL","FORMAT","FUNCTION","GOTO","IF","IMPLICIT","INQUIRE","INTRINSIC","OPEN","PARAMETER","PAUSE","PRINT","PROGRAM","READ","RETURN","REWIND","REWRITE","SAVE","STOP","SUBROUTINE","THEN","WRITE"};
/* Global declarations */
/* Variables */
int charClass;
char lexeme [100];
char nextChar='\n';
int lexLen;
int token;
int nextToken;
FILE *in_fp, *fopen();
char previousChar='\n';

int commentFlag=0;
//int newLineFlag;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();

/* Character classes */
#define LETTER 0
#define DIGIT 1
#define LOGICAL_EXP 2
#define UNKNOWN 99
/* Token codes */

#define INT_LIT 10
#define IDENT 11
#define KEYWORD 12
#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define MULT_OP 23
#define DIV_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26

#define STRING_LIT 27
#define COMMA_OP 28
#define COMPARISON_OP 29
int main()
{
    char fileName[50];
    printf("Write the name of the file (.for)\n");
    gets(fileName);

    /* Open the input data file and process its contents */
    if ((in_fp = fopen(fileName, "r")) == NULL)
        printf("ERROR - cannot open %s \n",fileName);
    else
    {
        getChar();
//        newLineFlag=1;
        do
        {
            lex();
        }
        while (nextToken != EOF);
    }
    return 0;
}
/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
int lookup(char ch)
{
    switch (ch)
    {
    case '(':
        addChar();
        nextToken = LEFT_PAREN;
        break;
    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        break;
    case '+':
        addChar();
        nextToken = ADD_OP;
        break;
    case '-':
        addChar();
        nextToken = SUB_OP;
        break;
    case '*':
        addChar();
        nextToken = MULT_OP;
        break;
    case '/':
        addChar();
        nextToken = DIV_OP;
        break;
    case '\'':
        /*String literal*/
//        position = ftell(stw_fp);/*For backtracing*/

        /*Read through EOF or " */
        addChar();
        do
        {
            getChar();
            addChar();
        }
        while(nextChar!= EOF && nextChar != '\'');

        if (nextChar == '\'')/*No error Return to the startting point*/
        {
//            fseek(stw_fp,position-1,SEEK_SET);
            nextToken = STRING_LIT;
        }
        else if(nextChar == EOF)/*Unclosed comment exception*/
        {
            printf("Unclosed string!\n");
//            exceptionHandler(EX_STRING_UNCLOSED);
        }
        break;
    case ',':
        addChar();
        nextToken = COMMA_OP;
        break;
    case '=':
        addChar();
        nextToken=ASSIGN_OP;
        break;
    default:
        addChar();
        nextToken = EOF;
        break;
    }
    return nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar()
{
    if (lexLen <= 98)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
    else
        printf("Error - lexeme is too long \n");
}
/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar()
{
    commentFlag=0;
    if(previousChar=='\n'&& nextChar=='C')
        commentFlag=1;
    previousChar=nextChar;
//    newLineFlag=0;
    if ((nextChar = getc(in_fp)) != EOF)
    {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else if(nextChar=='.')
            charClass=LOGICAL_EXP;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
//    while(nextChar==' ')
//    {
//        nextChar=getc(in_fp);
//        if(nextChar==EOF)
//            break;
//    }
////    if(newLineFlag==1)
////        newLineFlag=0;
//    if(nextChar=='\n')
//        newLineFlag=1;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank()
{
//    if(nextChar=='n')
//        newLineFlag=1;
    while (isspace(nextChar))
        getChar();
}
/****************************************************/
/* lex - a simple lexical analyzer for arithmetic
expressions */
int lex()
{
    lexLen = 0;
    getNonBlank();
    switch (charClass)
    {
        /* Parse identifiers */
    case LETTER:
        addChar();
        getChar();
        while (charClass == LETTER || charClass == DIGIT)
        {
            addChar();
            getChar();
        }

        int i,keywordFlag=0;
        for(i=0; i<KEYWORD_LIST_SIZE; i++)
            if(strcasecmp(keywordList[i],lexeme)==0)
            {
                keywordFlag=1;
                break;
            }
        if(keywordFlag==1)
            nextToken = KEYWORD;
        else
            nextToken = IDENT;
        break;
        /* Parse integer literals */
    case DIGIT:
        addChar();
        getChar();
        while (charClass == DIGIT)
        {
            addChar();
            getChar();
        }
        nextToken = INT_LIT;
        break;
        /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        break;
    case LOGICAL_EXP:
        addChar();
        getChar();
        while (charClass == LETTER )
        {
            addChar();
            getChar();
        }
        getChar();
        if(charClass=='.')
        {
            addChar();
            nextToken = COMPARISON_OP;
        }

        break;

        /* EOF */
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        break;
    } /* End of switch */
//    if( =='\n'|| strcasecmp("C",lexeme)!=0)
if(commentFlag==0)
    printf("Next token is: %d, Next lexeme is %s\n",
           nextToken, lexeme);
           if(commentFlag==1)
           {
               while(nextChar!='\n')

        nextChar=getc(in_fp);
                 nextToken=STRING_LIT;
           }

//    if(previousChar=='\n' && strcasecmp("C",lexeme)==0)
//    {
//
//
//        while(nextChar!='\n')
//
//        nextChar=getc(in_fp);
//                 nextToken=STRING_LIT;
//    }
    return nextToken;
} /* End of function lex */

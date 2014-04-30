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
FILE *tkn_fp;
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
#define ASTERISK_OP 23
#define SLASH_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26


#define STRING_LIT 27
#define COMMA_OP 28
#define COMPARISON_OP 29

#define GT_OP 30
#define LT_OP 31
#define GE_OP 32
#define LE_OP 33
#define EQ_OP 34
#define NE_OP 35

#define POW_OP 36
#define REAL_LIT 37

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

        char *x = strrchr(fileName,'.');
        strcpy(x,".lex");
        tkn_fp=fopen(fileName,"w+");
        commentFlag=0;
        getChar();

//        newLineFlag=1;
        do
        {
            lex();
            commentFlag=0;
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
        nextToken = ASTERISK_OP;
        break;
    case '/':
        addChar();
        nextToken = SLASH_OP;
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

    if((previousChar=='\n'&& nextChar=='C')&& commentFlag==0)
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
                nextToken = INT_LIT;
        while ((charClass == DIGIT ||nextChar=='D')||((nextChar=='.' || nextChar=='E' )||(nextChar=='F' || nextChar=='G')))
        {
//             if((nextChar=='.' || nextChar=='E' )||(nextChar=='F' || nextChar=='G'))
//            {
//
//                nextToken=REAL_LIT;
//            }
            addChar();
            getChar();


        }

        break;
        /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar);
        getChar();
        if(previousChar=='*' && nextChar=='*')
        {
            addChar();
            nextToken=POW_OP;
            getChar();
        }
        break;
    case LOGICAL_EXP:
        addChar();
        getChar();
        while (charClass == LETTER )
        {
            addChar();
            getChar();
        }
        if(nextChar=='.')
        {
//             nextToken = COMPARISON_OP;
            addChar();

            getChar();
            if(strcasecmp(lexeme,".gt.")==0)
                nextToken=GT_OP;
            else if(strcasecmp(lexeme,".lt.")==0)
                nextToken=LT_OP;
            else if(strcasecmp(lexeme,".ge.")==0)
                nextToken=GE_OP;
            else if(strcasecmp(lexeme,".le.")==0)
                nextToken=LE_OP;
            else if(strcasecmp(lexeme,".eq.")==0)
                nextToken=EQ_OP;
            else if(strcasecmp(lexeme,".ne.")==0)
                nextToken=NE_OP;



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
    {
        printf("Next token is: %d, Next lexeme is %s\n",
               nextToken, lexeme);
        fprintf(tkn_fp,"(%d,%s)",nextToken,lexeme);
    }
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

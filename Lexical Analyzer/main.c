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
int line=1;
int exception=0;
int errors=0;
//int newLineFlag;

/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void exceptionHandler(int);

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

#define NOT_OP 38
#define AND_OP 39
#define OR_OP 40
#define EQV_OP 41
#define NEQV_OP 42

#define EXC_STRING_UNCLOSED 1000
#define EXC_UNDEFINED_CHAR 1001
#define EXC_LONG_ID 1002
#define EXC_IMPROPER_ID 1003
#define EXC_IMPROPER_REAL_TYPE 1004
#define EXC_UNDEFINED_COMP_OP 1005
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
        exception=0;
        getChar();
        do
        {
            lex();
            commentFlag=0;
            exception=0;
        }
        while (nextToken != EOF);
        if(errors>0)
            printf("There are %d errors in source code, .lex file not created.\n",errors);
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
//    case '\'':
//        /*String literal*/
////        position = ftell(stw_fp);/*For backtracing*/
//
//        /*Read through EOF or " */
//        addChar();
//        do
//        {
//            getChar();
//            addChar();
//        }
        while((nextChar!= EOF && nextChar != '\'')&& nextChar!='\n');
//
//        if (nextChar == '\'')/*No error Return to the startting point*/
//        {
////            fseek(stw_fp,position-1,SEEK_SET);
//            nextToken = STRING_LIT;
//        }
        else /*Unclosed comment exception*/
//        {
//            printf("Unclosed string!\n");
            exceptionHandler(EXC_STRING_UNCLOSED);
//        }
//        break;
    case ',':
        addChar();
        nextToken = COMMA_OP;
        break;
    case '=':
        addChar();
        nextToken=ASSIGN_OP;
        break;
    case EOF:
        addChar();
        nextToken=EOF;
        break;
    default:
        exceptionHandler(EXC_UNDEFINED_CHAR);
        break;
    }
    return nextToken;
}
/*****************************************************/
/* addChar - a function to add nextChar to lexeme */
void addChar()
{
    if (lexLen <= 31)
    {
        lexeme[lexLen++] = nextChar;
        lexeme[lexLen] = 0;
    }
//    else
//        exceptionHandler(EXC_LONG_ID);
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
    if(nextChar=='\n')
        line++;
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
        while ((charClass == LETTER || charClass == DIGIT)|| nextChar=='_')
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
        int dotFlag=0;
        while (charClass==DIGIT)
        {
            addChar();
            getChar();
        }
        if(charClass==LETTER)
        {
            exceptionHandler(EXC_IMPROPER_ID);
            getChar();
            break;
        }
        if(nextChar=='.')
        {
            nextToken=REAL_LIT;
            addChar();
            getChar();
        }


        while ((charClass == DIGIT ||nextChar=='D')||( nextChar=='E' ||(nextChar=='F' || nextChar=='G')))
        {
//            if(nextChar=='.'&&dotFlag==0)
//                dotFlag=1;
//            if(( nextChar=='E' ||nextChar=='F') ||( nextChar=='G'||nextChar=='D'))
//            {
//                if(dotFlag==1)
//                    nextToken=REAL_LIT;
//                else exceptionHandler(EXC_IMPROPER_REAL_TYPE);
//            }
//            else exceptionHandler(EXC_IMPROPER_REAL_TYPE);
//            if(isalpha(nextChar)&&dotFlag==0)
//                exceptionHandler(EXC_IMPROPER_ID);
//            addChar();
//            getChar();
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
            else if(strcasecmp(lexeme,".not.")==0)
                nextToken=NOT_OP;
            else if(strcasecmp(lexeme,".and.")==0)
                nextToken=AND_OP;
            else if(strcasecmp(lexeme,".or.")==0)
                nextToken=OR_OP;
            else if(strcasecmp(lexeme,".eqv.")==0)
                nextToken=EQV_OP;
            else if(strcasecmp(lexeme,".neqv.")==0)
                nextToken=NEQV_OP;
            else exceptionHandler(EXC_UNDEFINED_COMP_OP);
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
    if(lexLen>31)
    exceptionHandler(EXC_LONG_ID);
    if(commentFlag==0&&exception==0)
    {
        printf("Next token is: %d, Next lexeme is %s\n",
               nextToken, lexeme);
        fprintf(tkn_fp,"(%d,%s)\n",nextToken,lexeme);
    }
    if(commentFlag==1)
    {
        while(nextChar!='\n')

            nextChar=getc(in_fp);
        nextToken=STRING_LIT;
    }

    return nextToken;
} /* End of function lex */
void exceptionHandler(int exceptionCode)
{
    errors++;
    printf("ERROR! ");
    exception=1;
    switch(exceptionCode)
    {
    case EXC_STRING_UNCLOSED:
        printf("String not closed in line: %d\n",line-1);
        break;
    case EXC_IMPROPER_ID:
        printf("Identiifer started with digit in line: %d\n",line-1);
        break;
    case EXC_IMPROPER_REAL_TYPE:
        printf("Real type defined improperly in line: %d\n",line-1);
        break;
    case EXC_LONG_ID:
        printf("Identifier has more character than 31 in line: %d\n",line-1);
        break;
    case EXC_UNDEFINED_CHAR:
        printf("Undefined character in line: %d\n",line-1);
        break;
    case EXC_UNDEFINED_COMP_OP:
        printf("Undefined compresion operator in line: %d\n",line-1);
        break;
    }
}

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define TYPE_MAX 31
struct Terminal
{
    char token [TYPE_MAX];
    char lexeme [TYPE_MAX];
    int tokenNumber;
    struct Terminal *sonraki;

} typedef TERMINAL;
struct TreeNode
{
    TERMINAL* term;
    struct TreeNode* rightNode;
    struct TreeNode* leftNode;
} typedef TREENODE;
struct VariableNode
{
    char identifierName[TYPE_MAX];
    int value;
    struct VariableNode* sonraki;

}
typedef VARIABLENODE;
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
char * expressionToParse;
int commentFlag=0;
int line=1;
int exception=0;
int errors=0;
int endFlag=0;
int positionFileEnd;
int keywordFlag=0;
char outputIdentifier []= "OUTPUT";
int degiskenListedeVar=0;
/* Function declarations */
void addChar();
void getChar();
void getNonBlank();
int lex();
void exceptionHandler(int);
void addToList(TERMINAL **, TERMINAL *);
void printList(TERMINAL *);
int lookup(char ch,TERMINAL * terminal);
void parse();
void execute();
int expression();
TREENODE * tree;
TERMINAL *list;
TERMINAL *onceki;
TERMINAL *sonraki;
TERMINAL *variable;
VARIABLENODE *variableList;
int getIdentifierValue(VARIABLENODE *,char*,int*);
void addToVariableList(VARIABLENODE **bas, VARIABLENODE *yeni);
/* Character classes */
#define LETTER 0
#define DIGIT 1
#define UNKNOWN 99
/* Token codes */

#define INT_LIT 10
#define IDENT 11
#define OUTPUT_IDENT 12

#define ASSIGN_OP 20
#define ADD_OP 21
#define SUB_OP 22
#define ASTERISK_OP 23
#define SLASH_OP 24
#define LEFT_PAREN 25
#define RIGHT_PAREN 26

#define STRING_LIT 27
#define EOL 28


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
    printList(list);
    parse();
    execute();
    getchar();

    return 0;
}
int getIdentifierValue(VARIABLENODE *bas,char* lexeme,int *bulundu)
{
    VARIABLENODE *gecici=bas;

    while(gecici!=NULL)
    {
        if(strcasecmp(gecici->identifierName,lexeme)==0)
        {
            *bulundu=1;
            return gecici->value;
        }
        gecici=gecici->sonraki;
    }
    *bulundu=0;
    return 0;
}
void execute()
{
    VARIABLENODE* variable;
    TREENODE * gecici=tree;
    char mathExp[100]="\0";
    char degiskenDegeriStr[20]="\0";
    static int solSag=0;
    while(gecici->term->tokenNumber!=EOF&&gecici->leftNode->term->tokenNumber!=EOF)
    {


        if(gecici->leftNode->term->tokenNumber==IDENT)
        {
            variable=malloc(sizeof(VARIABLENODE));
            strcpy(variable->identifierName,gecici->leftNode->term->lexeme);

            gecici=gecici->rightNode;
            while(gecici->term->tokenNumber!=EOL)
            {
                if(solSag==0)
                {
                    solSag=1;
                    if(gecici->leftNode->term->tokenNumber==IDENT)
                    {

                        int degiskenDegeri=getIdentifierValue(variableList,gecici->leftNode->term->lexeme,&degiskenListedeVar);

                        sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
                        strcat(mathExp,degiskenDegeriStr);
                    }
                    else if(gecici->leftNode->term->tokenNumber!=EOL)
                        strcat(mathExp,gecici->leftNode->term->lexeme);
                    else if(gecici->leftNode->term->tokenNumber==EOL)
                        continue;

                }
                else
                {
                    solSag=0;
                    if(gecici->term->tokenNumber==IDENT)
                    {

                        int degiskenDegeri=getIdentifierValue(variableList,gecici->term->lexeme,&degiskenListedeVar);

                        sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
                        strcat(mathExp,degiskenDegeriStr);
                    }
                    else if(gecici->term->tokenNumber!=EOL)
                    {
                        strcat(mathExp,gecici->term->lexeme);
                        gecici=gecici->rightNode;
                        solSag=1;

                        if(gecici->leftNode->term->tokenNumber!=EOL)
                            strcat(mathExp,gecici->leftNode->term->lexeme);
                        continue;
                    }
                    else if(gecici->term->tokenNumber==EOL)
                    {
                        gecici=gecici->rightNode;
                        continue;
                    }
                    gecici=gecici->rightNode;

                }
            }
            expressionToParse=mathExp;
            variable->value=expression();
            addToVariableList(&variableList,variable);
        }

//
//
//
//
//        if(gecici->leftNode->term->tokenNumber==IDENT)
//        {
//
//
//            variable=malloc(sizeof(VARIABLENODE));
//            strcpy(variable->identifierName,gecici->leftNode->term->lexeme);
//
//            gecici=gecici->rightNode;
//            while(gecici->term->tokenNumber!=EOL)
//            {
//                if(gecici->leftNode->term->tokenNumber==IDENT)
//                {
//
//                    int degiskenDegeri=getIdentifierValue(variableList,gecici->leftNode->term->lexeme,&degiskenListedeVar);
//
//                    sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
//                    strcat(mathExp,degiskenDegeriStr);
//                }
//                else if(gecici->leftNode->term->tokenNumber!=EOL)
//                    strcat(mathExp,gecici->leftNode->term->lexeme);
//
//                if(gecici->term->tokenNumber==IDENT)
//                {
//                    int listedeVar;
//                    int degiskenDegeri=getIdentifierValue(variableList,gecici->term->lexeme,&listedeVar);
//
//                    sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
//                    strcat(mathExp,degiskenDegeriStr);
//                }
//                else  if(gecici->term->tokenNumber!=EOL)
//
//
//                    strcat(mathExp,gecici->term->lexeme);
//                gecici=gecici->rightNode;
//            }
//            expressionToParse=mathExp;
//            variable->value=expression();
//            addToVariableList(&variableList,variable);
//        }
        else if(gecici->leftNode->term->tokenNumber==OUTPUT_IDENT)
        {

            if(gecici->term->tokenNumber==IDENT)
                printf(" %d ",getIdentifierValue(variableList,gecici->term->lexeme,&degiskenListedeVar));
            else if(gecici->term->tokenNumber==STRING_LIT)
                printf(" %s ",variableList,gecici->term->lexeme);
            gecici=gecici->rightNode;


            while(1==1)
            {
                if(gecici->leftNode->term->tokenNumber!=EOF&& gecici->leftNode->term->tokenNumber!=EOL)
                {

                    if(gecici->leftNode->term->tokenNumber==IDENT)
                        printf(" %d ",getIdentifierValue(variableList,gecici->leftNode->term->lexeme,&degiskenListedeVar));
                    else if(gecici->leftNode->term->tokenNumber==STRING_LIT)
                        printf(" %s ",gecici->leftNode->term->lexeme);



                }
                else break;

                if(gecici->term->tokenNumber!=EOF&& gecici->term->tokenNumber!=EOL)
                {

                    if(gecici->term->tokenNumber==IDENT)
                        printf(" %d ",getIdentifierValue(variableList,gecici->term->lexeme,&degiskenListedeVar));
                    else if(gecici->term->tokenNumber==STRING_LIT)
                        printf(" %s ",gecici->term->lexeme);
                }
                else break;

                gecici=gecici->rightNode;

            }





            while(gecici->leftNode->term->tokenNumber==EOL)
            {
                if(gecici->leftNode->term->tokenNumber==IDENT)
                {
                    int listedeVar;
                    int degiskenDegeri=getIdentifierValue(variableList,gecici->leftNode->term->lexeme,&listedeVar);

                    sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
                    strcat(mathExp,degiskenDegeriStr);
                }
                else
                    strcat(mathExp,gecici->leftNode->term->lexeme);

                if(gecici->term->tokenNumber==IDENT)
                {
                    int listedeVar;
                    int degiskenDegeri=getIdentifierValue(variableList,gecici->term->lexeme,&listedeVar);

                    sprintf(degiskenDegeriStr,"%d",degiskenDegeri);
                    strcat(mathExp,degiskenDegeriStr);
                }
                else


                    strcat(mathExp,gecici->term->lexeme);
                gecici=gecici->rightNode;
            }
        }
    gecici=gecici->rightNode;
    }


}
/*****************************************************/
/* lookup - a function to lookup operators and parentheses
and return the token */
void parse()
{
    static int leftRightFlag=0;
    tree =malloc(sizeof(TREENODE));
    TERMINAL * gecici=list;
    TREENODE *geciciTree=tree;
    while(gecici->tokenNumber!=EOF&&gecici->sonraki->tokenNumber!=EOF)
    {


        if(gecici->tokenNumber==IDENT)
        {
            geciciTree->leftNode=malloc(sizeof(TREENODE));
            geciciTree->leftNode->term=gecici;


            if(gecici->sonraki->tokenNumber==ASSIGN_OP)
            {


                geciciTree->term=gecici->sonraki;
                geciciTree->rightNode=malloc(sizeof(TREENODE));
                gecici=gecici->sonraki->sonraki;
                geciciTree=geciciTree->rightNode;
                geciciTree->leftNode=malloc(sizeof(TREENODE));
                while(gecici->tokenNumber!=EOL)
                {
                    if(leftRightFlag==0)
                    {
                        geciciTree->leftNode->term=gecici;
                        gecici=gecici->sonraki;
                        leftRightFlag=1;
                        geciciTree->rightNode=malloc(sizeof(TREENODE));
                    }
                    else
                    {
                        geciciTree->term=gecici;
                        gecici=gecici->sonraki;

                        geciciTree=geciciTree->rightNode;
                        leftRightFlag=0;
                        geciciTree->leftNode=malloc(sizeof(TREENODE));
                    }
                }
                geciciTree->term=gecici;
                geciciTree=geciciTree->rightNode;
                gecici=gecici->sonraki;
                if(leftRightFlag==1)
                    leftRightFlag=0;


//
//                while(gecici->sonraki->tokenNumber!=EOL)
//                {
//                    geciciTree=geciciTree->rightNode;
//                    geciciTree->leftNode=malloc(sizeof(TREENODE));
//                    geciciTree->leftNode->term=gecici;
//                    geciciTree->term=gecici->sonraki;
//                    gecici=gecici->sonraki->sonraki;
//                    geciciTree->rightNode=malloc(sizeof(TREENODE));
//                }
//                geciciTree->leftNode=malloc(sizeof(TREENODE));
//                geciciTree->leftNode->term=gecici;
            }
        }

        else if (gecici->tokenNumber==OUTPUT_IDENT)
        {

            geciciTree->leftNode=malloc(sizeof(TREENODE));
            while(gecici->tokenNumber!=EOL)
            {
                if(leftRightFlag==0)
                {
                    geciciTree->leftNode->term=gecici;
                    gecici=gecici->sonraki;
                    leftRightFlag=1;
                    geciciTree->rightNode=malloc(sizeof(TREENODE));
                }
                else
                {
                    geciciTree->term=gecici;
                    gecici=gecici->sonraki;

                    geciciTree=geciciTree->rightNode;
                    leftRightFlag=0;
                    geciciTree->leftNode=malloc(sizeof(TREENODE));
                }
            }
            geciciTree->term=gecici;
            geciciTree->rightNode=malloc(sizeof(TREENODE));
            geciciTree=geciciTree->rightNode;
            gecici=gecici->sonraki;


        }

    }
    geciciTree->leftNode=malloc(sizeof(TREENODE));
    geciciTree->leftNode->term=gecici;
}
int lookup(char ch,TERMINAL * terminal)
{
    switch (ch)
    {
    case '(':
        addChar();
        nextToken = LEFT_PAREN;
        strcpy(terminal->token,"LEFT_PAREN");
        break;
    case ')':
        addChar();
        nextToken = RIGHT_PAREN;
        strcpy(terminal->token,"RIGHT_PAREN");
        break;
    case '+':
        addChar();
        nextToken = ADD_OP;
        strcpy(terminal->token,"ADD_OP");
        break;
    case '-':
        addChar();
        nextToken = SUB_OP;
        strcpy(terminal->token,"SUB_OP");
        break;
    case '*':
        addChar();
        nextToken = ASTERISK_OP;
        strcpy(terminal->token,"ASTERISK_OP");
        break;
    case '/':
        addChar();
        nextToken = SLASH_OP;
        strcpy(terminal->token,"SLASH_OP");
        break;
    case '"':
        /*String literal*/

        /*Read through EOF or " */
        addChar();
        do
        {
            getChar();
            addChar();
        }
        while((nextChar!= EOF && nextChar != '"')&& nextChar!=';');

        if (nextChar == '"')/*No error Return to the startting point*/
        {
            nextToken = STRING_LIT;
        }
        else /*Unclosed comment exception*/
        {

            exceptionHandler(EXC_STRING_UNCLOSED);
        }
        strcpy(terminal->token,"STRING_LIT");
        break;
    case ':':
        addChar();
        getChar();
        if(  nextChar=='=')
        {
            addChar();
            nextToken=ASSIGN_OP;
            strcpy(terminal->token,"ASSIGN_OP");
        }
        else

            exceptionHandler(EXC_UNDEFINED_CHAR);






        break;
    case ';':
        addChar();
        nextToken=EOL;
        strcpy(terminal->token,"EOL");
        break;

    case EOF:
        addChar();
        nextToken=EOF;
        strcpy(terminal->token,"EOF");
        break;
    default:
        exceptionHandler(EXC_UNDEFINED_CHAR); // Undefined character exception
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
}
/*****************************************************/
/* getChar - a function to get the next character of
input and determine its character class */
void getChar()
{



    if ((nextChar = getc(in_fp)) != EOF)
    {
        if (isalpha(nextChar))
            charClass = LETTER;
        else if (isdigit(nextChar))
            charClass = DIGIT;
        else charClass = UNKNOWN;
    }
    else
        charClass = EOF;
    if(nextChar=='\n')
        line++;
}
/*****************************************************/
/* getNonBlank - a function to call getChar until it
returns a non-whitespace character */
void getNonBlank()
{
    while (isspace(nextChar))
        getChar();
}
/****************************************************/
/* lex - a simple lexical analyzer for arithmetic
expressions */
int lex()
{
    lexLen = 0;
    TERMINAL * terminal = malloc(sizeof(TERMINAL));
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
        nextToken=IDENT;
        strcpy(terminal->token,"IDENT_LIT");
        break;
        /* Parse integer literals */
    case DIGIT:
        addChar();
        getChar();
        nextToken = INT_LIT;
        strcpy(terminal->token,"DIGIT_LIT");
        while (charClass==DIGIT)
        {
            addChar();
            getChar();
        }
        if(charClass==LETTER) //Identifier started with digit exception
        {
            exceptionHandler(EXC_IMPROPER_ID);
            getChar();
        }
        break;
        /* Parentheses and operators */
    case UNKNOWN:
        lookup(nextChar,terminal);
        getChar();


        break;
        /* EOF */
    case EOF:
        nextToken = EOF;
        lexeme[0] = 'E';
        lexeme[1] = 'O';
        lexeme[2] = 'F';
        lexeme[3] = 0;
        strcpy(terminal->token,"EOF");
        break;
    } /* End of switch */
    strcpy(terminal->lexeme,lexeme);



    if(0==strcasecmp(outputIdentifier,lexeme))
    {

        nextToken=OUTPUT_IDENT;
        strcpy( terminal->token,outputIdentifier);
    }
    terminal->tokenNumber=nextToken;
    addToList(&list,terminal);
    if(lexLen>31)
        exceptionHandler(EXC_LONG_ID);
    if(exception==0)
    {
        printf("Next token is: %d, Next lexeme is %s\n",
               nextToken, lexeme);
        fprintf(tkn_fp,"(%d,%s)\n",nextToken,lexeme);


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
void addToList(TERMINAL **bas, TERMINAL *yeni)
{
    TERMINAL *gecici, *onceki;

    if(*bas==NULL) //kuyruk bossa
    {
        yeni->sonraki=NULL;
        *bas=yeni;
    }
    else
    {
        onceki=*bas;
        gecici=(*bas)->sonraki;
        while((gecici!=NULL)) //eklenecek uygun yer araniyor
        {
            onceki=gecici;
            gecici=gecici->sonraki;
        }
        yeni->sonraki=gecici; //gecici NULL ise en sona, degilse onceki dugumu ile gecici dugumu arasina ekleniyor
        onceki->sonraki=yeni;
    }
}
void addToVariableList(VARIABLENODE **bas, VARIABLENODE *yeni)
{
    VARIABLENODE *gecici, *onceki;

    if(*bas==NULL) //kuyruk bossa
    {
        yeni->sonraki=NULL;
        *bas=yeni;
    }
    else
    {
        onceki=*bas;
        gecici=(*bas)->sonraki;
        while((gecici!=NULL)) //eklenecek uygun yer araniyor
        {
            onceki=gecici;
            gecici=gecici->sonraki;
        }
        yeni->sonraki=gecici; //gecici NULL ise en sona, degilse onceki dugumu ile gecici dugumu arasina ekleniyor
        onceki->sonraki=yeni;
    }
}
void printList(TERMINAL *bas)
{
    TERMINAL *gecici;

    gecici=bas;
    printf("\n");
    while(gecici!=NULL)
    {
        printf("\t%s \t %s\n", gecici->lexeme, gecici->token);

        gecici=gecici->sonraki;
    }
}
char * expressionToParse;

char peek()
{
    return *expressionToParse;
}

char get()
{
    return *expressionToParse++;
}



int number()
{
    int result = get() - '0';
    while (peek() >= '0' && peek() <= '9')
    {
        result = 10*result + get() - '0';
    }
    return result;
}

int factor()
{
    if (peek() >= '0' && peek() <= '9')
        return number();
    else if (peek() == '(')
    {
        get(); // '('
        int result = expression();
        get(); // ')'
        return result;
    }
    else if (peek() == '-')
    {
        get();
        return -expression();
    }
    return 0; // error
}

int term()
{
    int result = factor();
    while (peek() == '*' || peek() == '/')
        if (get() == '*')
            result *= factor();
        else
            result /= factor();
    return result;
}

int expression()
{
    int result = term();
    while (peek() == '+' || peek() == '-')
        if (get() == '+')
            result += term();
        else
            result -= term();
    return result;
}



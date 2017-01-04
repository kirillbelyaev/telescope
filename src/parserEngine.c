/*
 Copyright (c) 2012-2017 Kirill Belyaev
 * kirillbelyaev@yahoo.com
 * kirill@cs.colostate.edu
 * TeleScope - XML Message Stream Broker/Replicator Platform
 * This work is licensed under the Creative Commons Attribution-NonCommercial 3.0 Unported License. 
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc/3.0/ or send 
 * a letter to Creative Commons, 444 Castro Street, Suite 900, Mountain View, California, 94041, USA.
 */

/*
 "DION
 The violent carriage of it
 Will clear or end the business: when the oracle,
 Thus by Apollo's great divine seal'd up,
 Shall the contents discover, something rare
 Even then will rush to knowledge. Go: fresh horses!
 And gracious be the issue!"

 Winter's Tale, Act 3, Scene 1. William Shakespeare
 */

#include "parserEngine.h"

char Expression[MAX_LINE];

long long int MatchingMessages = 0;

pthread_mutex_t updateExpLock;

const char * opChars = "=><!&|(P";


enum OP
{
    EQUAL,
    GREATER,
    LESS,
    NOT,
    AND,
    OR,
    PAREN,
    PRIMITIVE
};

typedef struct EXP
{
        const char * expStr;
        bool isValue;
        xmlChar * expXmlStr;
        double expStrValue;
        bool isAttr;
        int expStrLen;

        struct EXP * lhs;
        enum OP op;
        struct EXP * rhs;
} Exp;

//! @brief Root node of the parsed expression tree.
Exp * root;

int count = 0;
void printExpImpl ( Exp * n )
{
    count++;
    int a = count;
    printf ("%d : %c : \"%.*s\"\n", a, *(opChars + n->op), n->expStrLen, n->expStr );
    if ( n->lhs )
    {
        printf (" %d LHS\n", a );
        printExpImpl( n->lhs );
    }
    if ( n->rhs )
    {
        printf (" %d RHS\n", a );
        printExpImpl( n->rhs );
    }
}

void printExp( Exp * n )
{
    count = 0;
    printExpImpl(n);
}

//! @brief Allocate memory for an Exp
Exp * newExp()
{
    Exp * retval;
    if ( ( retval = (Exp *) malloc ( sizeof(Exp) ) ) == NULL )
    {
        logMessage( stderr, "Failed to allocate memory for Exp.\n");
        exit(-1);
    }
    memset( retval, '\0', sizeof(Exp) );
    return retval;
}

int getParenthesisExp( const char ** exp, int expLen )
{
    char * matchedClose;
    char * matchedOpen;
    char * otherOpen;

    // Find first open parenthesis
    matchedOpen = strchr( *exp, '(' );
    if ( matchedOpen == NULL )
        return -1;

    // Find closing parenthesis ignoring embedded parenthesis
    matchedClose = strchr( matchedOpen, ')' );
    if ( matchedClose == NULL )
        return -1;
    otherOpen = strchr( matchedOpen + 1, '(' );
    for(;;)
    {
        if ( otherOpen == NULL || otherOpen > matchedClose )
            break;
        matchedClose = strchr( matchedClose + 1, ')' );
        if ( matchedClose == NULL )
            return -1;
        otherOpen = strchr( otherOpen + 1, '(' );
    }
    *exp = matchedOpen;
    return matchedClose - matchedOpen + 1;
}

Exp * parseExpression( const char * exp, int expLen )
{
    bool notExists;
    int newExpLen;
    int j;
    const char * op = exp + expLen, *tmp;
    const char * i;
    Exp * currentNode;
    Exp * tmpNode;
    enum OP opChar;

    currentNode = newExp();
    currentNode->expStr = exp;
    currentNode->expStrLen = expLen;

    // Find first high level operation character
    for ( j = 0, i = "(&|"; j < 3; j++, i++ )
    {
        tmp = strchr( exp, *i );
        if ( tmp != NULL && tmp < op )
            op = tmp;
    }
    // If '(', '&', or '|' found
    if ( op < exp + expLen )
    {
        switch (*op)
        {
            case '&':
                currentNode->lhs = parseExpression( exp, op-exp);
                currentNode->rhs = parseExpression( op+1, expLen - ( op - exp + 1 ) );
                currentNode->op = AND;
                if ( currentNode->lhs == NULL || currentNode->rhs == NULL )
                {
                    free(currentNode);
                    return NULL;
                }
                return currentNode;
            case '|':
                currentNode->lhs = parseExpression( exp, op-exp);
                currentNode->rhs = parseExpression( op+1, expLen - ( op - exp + 1 ) );
                currentNode->op = OR;
                if ( currentNode->lhs == NULL || currentNode->rhs == NULL )
                {
                    free(currentNode);
                    return NULL;
                }
                return currentNode;
            case '(':
                // Check for non-space chars before open paren.
                newExpLen = getParenthesisExp( &op, expLen );
                if ( newExpLen == -1 )
                {
                    logMessage( stderr, "Invalid '('\n", *i);
                    free(currentNode);
                    return NULL;
                }
                for ( i = exp; i < op; i++ )
                {
                    if ( *i == '!' )
                    {
                        notExists = true;
                    }
                    else if ( ! isspace(*i) )
                    {
                        logMessage( stderr, "Need operator between '%c' and '('\n", *i);
                        free(currentNode);
                        return NULL;
                    }
                }
                if ( notExists )
                {
                    tmpNode = newExp();
                    tmpNode->expStr = op;
                    tmpNode->expStrLen = newExpLen;
                    tmpNode->op = PAREN;
                    tmpNode->lhs = parseExpression( op + 1, newExpLen - 2 );
                    // NOT before a paren
                    if ( tmpNode->lhs == NULL )
                    {
                        free(tmpNode);
                        free(currentNode);
                        return NULL;
                    }
                    currentNode->op = NOT;
                    currentNode->lhs = tmpNode;
                }
                else
                {
                    currentNode->op = PAREN;
                    currentNode->lhs = parseExpression( op + 1, newExpLen - 2 );
                    if ( currentNode->lhs == NULL )
                    {
                        free(currentNode);
                        return NULL;
                    }
                }
                for ( i = op + newExpLen; i < exp + expLen; i++ )
                {
                    if ( *i == '&'
                            || *i == '|'
                            || *i == '='
                            || *i == '>'
                            || *i == '<'
                                    )
                    {
                        switch ( *i )
                        {
                            case '&':
                                opChar = AND;
                                break;
                            case '|':
                                opChar = OR;
                                break;
                            case '=':
                                opChar = EQUAL;
                                break;
                            case '>':
                                opChar = GREATER;
                                break;
                            case '<':
                                opChar = LESS;
                                break;
                        }
                        tmpNode = newExp();
                        tmpNode->expStr = exp;
                        tmpNode->expStrLen = i-exp;
                        tmpNode->op = currentNode->op;
                        tmpNode->lhs = currentNode->lhs;
                        currentNode->op = opChar;
                        currentNode->lhs = tmpNode;
                        currentNode->rhs = parseExpression( i+1, expLen - ( i - exp + 1 ) );
                        if ( currentNode->lhs == NULL || currentNode->rhs == NULL )
                        {
                            free(currentNode);
                            return NULL;
                        }
                        return currentNode;
                    }
                    else if ( ! isspace(*i) )
                    {
                        logMessage( stderr, "Invalid character '%c' after ')'\n", *i);
                        if ( currentNode->lhs )
                            free( currentNode->lhs );
                        free( currentNode );
                        return NULL;
                    }
                }
                return currentNode;
            default:
                break;
        }
    }

    // Handle NOT and get rid of leading white space.
    // NOTE: cannot use trim since trim changes the string it operates on.
    for ( i = exp; i < exp+expLen; i++ )
    {
        if ( *i == '!' )
        {
            // NOT before a paren
            currentNode->op = NOT;
            currentNode->lhs = parseExpression( i + 1, expLen - (i - exp + 1) );
            if ( currentNode->lhs == NULL )
            {
                free(currentNode);
                return NULL;
            }
            return currentNode;
        }
        else if ( ! isspace(*i) )
        {
            break;
        }
        currentNode->expStrLen--;
    }
    currentNode->expStr = i;

    // Find first low level operation character
    for ( j = 0, i = "=><"; j < 3; j++, i++ )
    {
        tmp = strchr( exp, *i );
        if ( tmp != NULL && tmp < op )
            op = tmp;
    }
    // If '=', '>', or '<' found
    if ( op < exp + expLen )
    {
        currentNode->lhs = parseExpression( exp, op-exp);
        currentNode->rhs = parseExpression( op+1, expLen - ( op - exp + 1 ) );
        if ( currentNode->lhs == NULL || currentNode->rhs == NULL )
        {
            free(currentNode);
            return NULL;
        }
        switch (*op)
        {
            case '=':
                currentNode->op = EQUAL;
                return currentNode;
            case '>':
                currentNode->op = GREATER;
                return currentNode;
            case '<':
                currentNode->op = LESS;
                return currentNode;
            default:
                break;
        }
    }

    // Exp is a primitive (Field name or value)
    // get rid of trailing whitespace
    for ( i = currentNode->expStr+currentNode->expStrLen-1; i >= currentNode->expStr; i-- )
    {
        if ( isspace(*i) )
            currentNode->expStrLen--;
        else
            break;
    }
    if (  currentNode->expStrLen <= 0 )
    {
        logMessage( stderr, "Empty primitive\n", *i);
        free(currentNode);
        return NULL;
    }
    currentNode->op = PRIMITIVE;
    currentNode->isValue = true;
    for ( j = 0, i = currentNode->expStr; j < currentNode->expStrLen; j++, i++ )
    {
        if ( isalpha( *i ) )
            currentNode->isValue = false;
    }
    currentNode->expStrValue = strtold( currentNode->expStr, NULL );
    currentNode->isAttr = ( strncmp( currentNode->expStr, "length", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "version", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "type", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "value", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "code", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "timestamp", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "datetime", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "precision_time", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "withdrawn_len", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "path_attr_len", currentNode->expStrLen ) == 0
                         || strncmp( currentNode->expStr, "label", currentNode->expStrLen ) == 0 );
    currentNode->expXmlStr = malloc(currentNode->expStrLen + 1 );
    strncpy( (char *)currentNode->expXmlStr, currentNode->expStr, currentNode->expStrLen );
    return currentNode;
}

char * get_attribute( xmlNode * a_node, xmlChar * name )
{
    xmlNode * cur;
    char * tmp;
    xmlChar * buf;
    for( cur = a_node; cur != NULL; cur = cur->next )
    {
        if ( cur->properties != NULL  && cur->properties->type == XML_ATTRIBUTE_NODE )
        {
            buf = xmlGetProp( cur, name );
            if ( buf )
                return (char*)buf;
        }
        tmp = get_attribute(cur->children, name);
        if ( tmp )
            return tmp;
    }
    return NULL;
}

char * get_element( xmlNode * a_node, xmlChar * name, bool elementFound )
{
    xmlNode * cur;
    char * tmp;
    for( cur = a_node; cur != NULL; cur = cur->next )
    {
        if ( elementFound && cur->type == XML_TEXT_NODE )
        {
            return (char*)cur->content;
        }
        if ( cur->type == XML_ELEMENT_NODE && xmlStrEqual( cur->name, name ) )
        {
            elementFound = true;
        }
        tmp = get_element(cur->children, name, elementFound);
        elementFound = false;
        if ( tmp )
            return tmp;
    }
    return NULL;
}

bool stringCompare ( const char * value, const char * exp, int expLen, bool prevWild )
{
    const char * i;
    const char * expEnd;
    const char * wild;

    // Return true if no string length
    if ( ! expLen )
        return true;

    // Find next wild chard in exp and set search area between exp and expEnd
    wild = strchr( exp, '%' );
    if ( wild && wild < exp + expLen )
    {
        expEnd = wild;
    }
    else
    {
        wild = NULL;
        expEnd = exp + expLen;
    }

    if ( prevWild )
    {
        // If you had a previous wild card, find all matches in value of the substring between exp and expEnd
        for ( i = value; *i ; i++ )
        {
            if( strncmp(i, exp, expEnd - exp ) == 0 )
            {
                // Found match, recursively match the rest of exp after the wild card character
                if ( wild && stringCompare( i + ( expEnd - exp ), wild + 1, expLen - ( expEnd - exp + 1), true ) )
                    return true;
                // Or if you matched to the end of value, and it is the end of exp, return true
                else if ( !wild && strlen(i) == expLen )
                    return true;
            }
        }
    }
    else
    {
        // If no previous wild, need to match from the beginning of value
        if( strncmp( value, exp, expEnd - exp ) == 0 )
        {
            // Found match, recursively match the rest of exp after the wild card character
            if ( wild && stringCompare( value + ( expEnd - exp ), wild + 1, expLen - ( expEnd - exp + 1), true ) )
                return true;
            // Or if you matched to the end of value, and it is the end of exp, return true
            else if ( !wild && strlen(value) == expLen)
                return true;

        }
    }

    // No match, return false
    return false;
}

bool evaluateExpression ( Exp * n, xmlNode * a_node ) {
    double value;
    char * text;

    if ( !n )
        return true;
    switch( n->op )
    {
        case EQUAL:
            if ( n->lhs->isAttr )
                text = get_attribute(a_node, n->lhs->expXmlStr);
            else
                text = get_element(a_node, n->lhs->expXmlStr, false);
            if ( text )
            {
                if ( n->rhs->isValue )
                {
                    value = strtold(text, NULL);
                    return value == n->rhs->expStrValue;
                }
                return stringCompare ( text, n->rhs->expStr, n->rhs->expStrLen, false );
            }
            return false;
        case GREATER:
            if ( n->lhs->isAttr )
                text = get_attribute(a_node, n->lhs->expXmlStr);
            else
                text = get_element(a_node, n->lhs->expXmlStr, false);
            if ( text )
            {
                if ( n->rhs->isValue )
                {
                    value = strtold(text, NULL);
                    return value > n->rhs->expStrValue;
                }
                return strncmp( text, n->rhs->expStr, n->rhs->expStrLen ) > 0;
            }
            return false;
        case LESS:
            if ( n->lhs->isAttr )
                text = get_attribute(a_node, n->lhs->expXmlStr);
            else
                text = get_element(a_node, n->lhs->expXmlStr, false);
            if ( text )
            {
                if ( n->rhs->isValue )
                {
                    value = strtold(text, NULL);
                    return value < n->rhs->expStrValue;
                }
                return strncmp( text, n->rhs->expStr, n->rhs->expStrLen ) < 0;
            }
            return false;
        case NOT:
            return ! evaluateExpression(n->lhs, a_node);
        case AND:
            return evaluateExpression(n->lhs, a_node) && evaluateExpression(n->rhs, a_node);
        case OR:
            return evaluateExpression(n->lhs, a_node) || evaluateExpression(n->rhs, a_node);
        case PAREN:
            return evaluateExpression(n->lhs, a_node);
        case PRIMITIVE:
            return true;
    }
    return false;
}
void deleteExpression( Exp * n ) {
    if ( n->lhs != NULL )
        deleteExpression(n->lhs);
    if ( n->rhs != NULL )
        deleteExpression(n->rhs);
    if ( n->expXmlStr )
        free( n->expXmlStr );
    free(n);
}

int InitializeParseEngine( char *ev ) {
    if ( root )
        deleteExpression( root );
    else
        if ( pthread_mutex_init( &updateExpLock, NULL ) )
        {
            perror( "unable to init mutex ID lock" );
            exit( 1 );
        }
    lockMutex( &updateExpLock );
    if ( strlen(ev) )
    {
        root = parseExpression(ev, strlen(ev));
        if ( !root )
        {
            logMessage(stderr, "Failed to parse expression \"%s\"\n", ev);
            exit(-1);
        }
    }
    else
    {
        root = NULL;
    }
    strcpy( Expression, ev );
    unlockMutex( &updateExpLock );
    return 0;
}

int analyze( const char *filename, xmlNode *root_element, char * buf )
{
    int fd;
    lockMutex( &updateExpLock );
    bool evalResult = evaluateExpression ( root, root_element );
    unlockMutex( &updateExpLock );
    if ( evalResult )
    {
        fprintf( logfile, "analyze: exp is true\n" );
        fd = open( filename, O_CREAT | O_APPEND | O_RDWR, S_IRWU );
        write( fd, buf, strlen( buf ) );
        write( fd, "\n", 1 );
        close(fd);
        MatchingMessages++;
        if ( serverflag == 1 )
        {

            fprintf( logfile, "Writer: putting the message into the Queue\n" );
            writeQueueTable( getQueueTable(), buf );
        }
        else
        {
            fprintf( logfile, "Writer: not putting the message into the Queue\n" );
        }
    }
    return 0;
}

char * getExpression()
{
    return Expression;
}

long long int getMatchingMessages()
{
    return MatchingMessages;
}

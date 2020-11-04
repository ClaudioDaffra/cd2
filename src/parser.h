#ifndef cdParser
#define cdParser

#include <stdio.h>
#include <ctype.h>
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "../lib/mstack.h"
#include "ast.h"
#include "symTable.h"


// MATCH

wchar_t* doMatchMessage( wchar_t C0 , wchar_t* token )  ;

#define $MATCH( SYM_EXPECTED , WCHAR_EXPECTED )\
do {\
	if ( this->lexer->sym == SYM_EXPECTED )\
	{\
		parserGetToken(this);\
	}\
	else\
	{\
		$matchError( doMatchMessage(WCHAR_EXPECTED,this->lexer->token) ) ;\
		return NULL ;\
	}\
} while(0) ;


// parser : fields

struct parser_s
{
	int 		fDebug 					;
	FILE*		pFileOutputParser		;  
	char*		fileNameOutputParser	;
	char*		fileInputName			;
	plexer_t	lexer 					;
	past_t		ast						; 
} ;

typedef struct parser_s 	parser_t ;
typedef struct parser_s* 	pparser_t ;

// parser : methods

// parser.c

pparser_t	parserAlloc		( void ) ;			    
void  		parserDealloc	( pparser_t this ) ;
void 		parserCtor		( pparser_t this ) ;
void 		parserDtor		( pparser_t this ) ;

int 		parserPrintToken( pparser_t this ) ;
sym_t 		parserGetToken	( pparser_t this ) ;
pnode_t 	parserScan		( pparser_t this ) ;
pnode_t 	parserBlock		( pparser_t this , stScope_t	scope ) ;
pnode_t 	parserStatement ( pparser_t this , node_t* nBlock ) ;
pnode_t 	parserFunction	( pparser_t this , node_t* nBlock ) ;
pnode_t 	parserMainBlock	( pparser_t this ) ;

// decl.c

pnode_t  	parserDeclConst		( pparser_t this , stScope_t scope ) ; 
pnode_t  	parserDeclVar		( pparser_t this , stScope_t scope ) ;
pnode_t  	parserArrayDim		( pparser_t this ) ;
pnode_t  	parserArrayDimDecl	( pparser_t this ) ;
pnode_t  	parserDeclArray		( pparser_t this , stScope_t scope ) ;
pnode_t  	parserDeclType		( pparser_t this , stScope_t scope ) ;
pnode_t 	parserDeclaration	( pparser_t this , node_t* nBlock , stScope_t	scope ) ;
pnode_t  	parserDeclFunction	( pparser_t this ) ;

// expr.c

node_t* 	parserTerm		( pparser_t this ) ;
node_t* 	parserMulDivMod	( pparser_t this ) ;
node_t* 	parserAddSub	( pparser_t this ) ;
node_t* 	parserAssign	( pparser_t this ) ;
node_t* 	parserExpr		( pparser_t this ) ;



#endif

   

/**/



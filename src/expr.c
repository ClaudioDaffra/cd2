#include "error.h"
#include "lexer.h"
#include "../lib/mstack.h"
#include "ast.h"
#include "parser.h"
//#include "symTable.h"


// ................................................... private prefix in expr

static
psPrefixOp_t	parserPrefixNew( plexer_t lexer )
{
	psPrefixOp_t	prefix	= gcMalloc( sizeof(struct sPrefixOp_s) ) ;
	
	if ( prefix == NULL )
	{
		 $astInternal ( malloc , outOfMemory , L"expr.c" , L"parserMakePrefix") ;
	}
	else
	{
		prefix->sym 		= lexer->sym ;
		prefix->row_start	= lexer->row_start ;
		prefix->col_start 	= lexer->col_start ;
		prefix->token 		= gcWcsDup( lexer->token ) ;
	}
	return prefix ;
}

static
psPrefixOp_t	parserPrefixDelete( psPrefixOp_t prefix )
{
	if ( prefix != NULL )
	{
		 gcFree(prefix->token);
		 prefix = NULL ;
	}
	return prefix ;
}

// ................................................... TERM

node_t* parserTerm( pparser_t this )
{
	// PREFIX PUSH

		stackType( psPrefixOp_t , sPrefixOp ) ;
		stackNew ( sPrefixOp , 128 ) ;

		while ( this->lexer->sym == sym_add
			||	this->lexer->sym == sym_sub
			||	this->lexer->sym == sym_not
		)
		{
			if ( this->lexer->sym != sym_add ) // prefix -> skip '+'
			{
				if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserPrefix push [%d]\n",this->lexer->sym );
				
				psPrefixOp_t pprefix = parserPrefixNew( this->lexer ) ;
				
				stackPush ( sPrefixOp , pprefix  ) ;
			}
			parserGetToken(this);
		}

	// END PREFIX PUSH

	node_t* n=NULL ;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserTerm\n" );

	switch ( this->lexer->sym )
	{
		case sym_p0 :

			if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parser Match ( .\n" );

			$MATCH( sym_p0, L'(' ) ;

			n=parserExpr(this);

			$MATCH( sym_p1, L')' ) ;

			if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parser Match ) . \n" );

		break ;

		case sym_integer :

			#ifdef _MSC_VER
				fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lld].\n",L"sym_integer",this->lexer->value.integer );
			#else
				fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ld].\n" ,L"sym_integer",this->lexer->value.integer );
			#endif

			n = astMakeNodeTermInteger( this->ast,this->lexer,this->lexer->value.integer ) ;

			parserGetToken(this);

		break;

		case sym_real:

			fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lf].\n",L"sym_real",this->lexer->value.real );

			n = astMakeNodeTermReal( this->ast,this->lexer,this->lexer->value.real ) ;

			parserGetToken(this);

		break;

		case sym_char :

			fwprintf ( this->pFileOutputParser , L"%-30ls :: [%lc].\n",L"sym_char",g.outputSpecialCharInChar(this->lexer->value.wchar) ) ;

			n = astMakeNodeTermChar( this->ast,this->lexer, this->lexer->value.wchar ) ;

			parserGetToken(this);

		break;

		case sym_string :

			fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ls].\n",L"sym_string",g.outputSpecialCharInString(this->lexer->value.wstring) );

			n = astMakeNodeTermString( this->ast,this->lexer, this->lexer->value.wstring ) ;

			parserGetToken(this);

		break;
/*
		case sym_id:

			fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ls].\n",L"sym_id",this->lexer->value.id );

			// #1 const
			psymTable_t pID=stFindIDinMap(this->lexer->value.id) ;
				
			if ( pID==NULL )
			{
				// allora e' una variabile : x ... gestione post fix in seguito [] ()
				n=makeNodeTermVar(lexer.id);
			}
			else // e' un tipo gia' definito
			{
				// un volta accertato che l'identificatore esiste ...
				switch( pID->kind )
				{
					case stKindConst : // SE è una costante ...
					
							switch( pID->type ) // sostituiamo il simbolo con la costante a seconda del tipo
							{
								case stTypeInteger : 
									n = astMakeNodeTermInteger( this->ast,this->lexer,pID->value.integer ) ; // valore della costante nella ST
								break;
								
								case stTypeReal :
									n = astMakeNodeTermReal( this->ast,this->lexer,pID->value.real ) ;
								break;
								
								default: 
									$parserInternal(parseExpr,notImplemetedYet,L"expr.c",L"case sym_id->switch( pID->type )");
									n=NULL ;
								break ;
							}
					
					break ;
					
					//case stKindVar : // errore vengono gestiti qui solo le costanti !

					break ;
					
					default:
						$parserInternal(parseExpr,notImplemetedYet,L"expr.c",L"case sym_id->switch( pID->kind )");
						n=NULL ;
					break ;
			    }
				
			}

			cdParserGetToken();

		break;
*/
		default:
			
			$syntaxError

		break;
	}

	// PREFIX POP

		while(stackSize(sPrefixOp))
		{
			if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserPrefix pop [%d]\n",this->lexer->sym );
			
			n = astMakeNodePrefix( this->ast,stackTop(sPrefixOp) , n ) ;
			
			parserPrefixDelete(stackTop(sPrefixOp));
			
			stackPop(sPrefixOp);
		}

	// END PREFIX POP

 return n ;
}

// ................................................... mul div mod * / % 

node_t* parserMulDivMod( pparser_t this )
{
	node_t *left=NULL;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserMulDivMod\n" );

	left=parserTerm(this);

	while ( 	this->lexer->sym == sym_mul
		||		this->lexer->sym == sym_div
		||		this->lexer->sym == sym_mod
		)
	{
		sym_t 		symSave  	= this->lexer->sym ;
		wchar_t*	tokenSave 	= gcWcsDup(this->lexer->token);
		uint32_t	rowSave 	= this->lexer->row_start ;
		uint32_t	colSave 	= this->lexer->col_start ;
		
		node_t *right=NULL;

		parserGetToken(this);

		right=parserTerm(this);
		
		if ( ( symSave == sym_div ) || ( symSave == sym_mod ) )
		{
			if ( right->type == nTypeTermInteger) 
				if ( right->term.integer == 0 )
					$parserError( parseExpr , division_by_zero );

			if ( right->type == nTypeTermReal) 
				if ( right->term.real == 0.0 )
					$parserError( parseExpr , division_by_zero );

		} ;
	    
	    left = astMakeNodeBinOP( this->ast,this->lexer,symSave , right,left ) ;
	    
		left->token = 	tokenSave 	; 
		left->row	=	rowSave 	;
		left->col	=	colSave 	;
    } 

 return left ;
}

// ................................................... Add Sub + - 

node_t* parserAddSub( pparser_t this )
{
	node_t *left=NULL;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAddSub\n" );

	left=parserMulDivMod(this);

	while ( 	this->lexer->sym == sym_add
		||		this->lexer->sym == sym_sub
		)
	{
		sym_t 		symSave  	= this->lexer->sym ;
		wchar_t*	tokenSave 	= gcWcsDup(this->lexer->token);
		uint32_t	rowSave 	= this->lexer->row_start ;
		uint32_t	colSave 	= this->lexer->col_start ;
		
		node_t *right=NULL;

		parserGetToken(this);

		right=parserMulDivMod(this);

		left = astMakeNodeBinOP( this->ast,this->lexer,symSave , right,left ) ;
		
		left->token = 	tokenSave 	; 
		left->row	=	rowSave 	;
		left->col	=	colSave 	;
	} ;

 return left ;
}

// ................................................... Assign :=

node_t* parserAssign( pparser_t this )
{
	node_t *left=NULL;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAssign\n" );

	left=parserAddSub(this);

	while ( this->lexer->sym == sym_assign )
	{
		node_t *right=NULL;

		parserGetToken(this);

		right=parserAddSub(this);

		left = astMakeNodeAssign( this->ast,this->lexer,left,right ) ; // invertiti
	} ;

 return left ;
}

// ................................................... EXPR

node_t* parserExpr( pparser_t this )
{
	if ( this->lexer->sym == sym_end ) return NULL ;

	node_t *n=NULL;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserExpr\n" );

	n=parserAssign(this);

 return n ;
}


/**/


#include "error.h"
#include "lexer.h"
#include "../lib/mstack.h"
#include "ast.h"
#include "parser.h"
#include "symTable.h"


// ................................................... declaration

/*
 
	### {parserDeclConst}

	"const" -> #1	[id] 	-> "=" 	|->	const 	integer		|->		|				->	";" -> #2
									|->	const	real		|-^		|-> "," -> #1
									|->	const 	string		|-^
									|->	const 	char		|-^
									
	#2

	Questo nodo è un caso speciale, in quanto non genera direttamente 
	una foglia (nodo) dell'albero (ast), piuttosto va a sostituire
	il simbolo con la costante.

*/

pnode_t  parserDeclConst( pparser_t this , stScope_t scope )
{
	if ( kError ) return NULL  ;  
	
	// *****
	// BEGIN
	// *****

	node_t* nBlock=astMakeNodeBlock(this->ast) ;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDeclConst\n" );

	// ............................... "const"
	if ( this->lexer->sym==sym_kw_const )  
	{
		do {
			node_t *n=NULL;
			
			wchar_t*	idTemp 			= NULL 		;
			sym_t		symTemp 		= sym_end 	;
			node_t*		pnTermTemp		= NULL		;

			parserGetToken(this);

	// ............................... new sym table
			//psymTable_t	pstNew = stMakeSymTable() ; // ................... ST
			//pstNew->kind = stKindConst ; // .............................. ST
 
	// ............................... [id]
			if ( this->lexer->sym==sym_id ) 
			{
				idTemp = gcWcsDup( this->lexer->token ) ;
				
				//pstNew->id = gcWcsDup( lexer.token ) ; // ................ ST
				parserGetToken(this);
				
	// ............................... "="
				if ( this->lexer->sym==sym_eq ) 
				{
					
					parserGetToken(this);

	// ............................... [integer,real,char,string]
	
					symTemp = this->lexer->sym ;
					
					switch(this->lexer->sym)
					{
						case sym_integer :
						
							//pstNew->type = stTypeInteger ; // ............ ST
							//pstNew->value.integer = lexer.integer ; // ... ST
							pnTermTemp = astMakeNodeTermInteger( this->ast , this->lexer , this->lexer->value.integer ) ;
							parserGetToken(this);
							
							break ;
							
						case sym_real :
						
							//pstNew->type = stTypeReal ; // ............... ST
							//pstNew->value.real = lexer.real ; // ......... ST
							pnTermTemp = astMakeNodeTermReal( this->ast , this->lexer , this->lexer->value.real );
							parserGetToken(this);
							
							break ;
							
						case sym_char :
						
							//pstNew->type = stTypeReal ; // ............... ST
							//pstNew->value.real = lexer.wchar ; // ......... ST
							pnTermTemp = astMakeNodeTermChar( this->ast , this->lexer , this->lexer->value.wchar ) ;
							parserGetToken(this);
							
							break ;
						
						case sym_string :
						
							//pstNew->type = stTypeReal ; // ............... ST
							//pstNew->value.real = lexer.wstring ; // ......... ST
							pnTermTemp = astMakeNodeTermString( this->ast , this->lexer , this->lexer->value.wstring ) ;
							parserGetToken(this);
							
							break ;
						
						default: $syntaxError ; break ;
					}
				}
				else 
				{
					$syntaxError ; 
					return NULL ;
				} 
			}
			else 
			{
				$syntaxError ; 
				return NULL ;
			}
			
			//
/*			
			stDebugSymTableNode(pstNew) ; // DEBUG
			// cerca se l'identificativo è già presente, nella ST il nuovo e' ancora da inserire
			psymTable_t pstTemp = stFindIDinMap(pstNew->id); 
			
			if ( pstTemp ) 
			{
				$scannerErrorExtra(scanning,duplicateSymbolName,pstNew->id) ;
			}
			
			whmapInsert( mapST, stGetFullName(pstNew->id)   , pstNew ); // altrimenti inserisci name space + id
*/	
			// ---------------
			// make node const
			// ---------------
			
			n = astMakeNodeDeclConst( this->ast , idTemp , symTemp , pnTermTemp , scope ) ;
			
			if ( n!=NULL ) astPushNodeBlock( this->ast , nBlock , n ) ;
			
			//

	// ............................... ","
		} while ( this->lexer->sym==sym_v ) ;
		
	// ............................... ";"
	$MATCH( sym_pv, L';' ) ; // can be omitted
	
	}

	// ***
	// END
	// ***
		
	if ( kError ) return NULL ; 

 return nBlock ;
}

/*
 
	### {parserDeclVar}

	"Var" -> #1	[id] 	-> ":"	|->		integer		|->		|										->	";" -> #2
								|->		real		|-^		|-> 					->	"," -> #1
								|->		char		|-^		|-> :=	-> expr  		->
								|->		byte		|-^
								|->		id			|		|->	:=	->	InitList	->
	#2


*/

pnode_t  parserDeclVar( pparser_t this , stScope_t scope )
{
	if ( kError ) return NULL  ;  
	
	// *****
	// BEGIN
	// *****

	node_t* nBlock=astMakeNodeBlock(this->ast) ;

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDeclVar\n" );

	// ............................... "var"
	if ( this->lexer->sym==sym_kw_var )  
	{
		do {
			node_t *n=NULL;
			
			wchar_t*	idTemp 			= NULL 		;
			sym_t		symTemp 		= sym_end 	;
			node_t*		exprTemp		= NULL		;

			parserGetToken(this);

	// ............................... new sym table
			//psymTable_t	pstNew = stMakeSymTable() ; // ................... ST
			//pstNew->kind = stKindVar ; // .............................. ST
 
	// ............................... [id]
			if ( this->lexer->sym==sym_id ) 
			{
				idTemp = gcWcsDup( this->lexer->token ) ;
				
				//pstNew->id = gcWcsDup( lexer.token ) ; // ................ ST
				parserGetToken(this);
				
	// ............................... ":"
				if ( this->lexer->sym==sym_dp ) 
				{
					parserGetToken(this);

	// ............................... [integer,real,char,byte]
	
					symTemp = this->lexer->sym ;
					
					int fInitializerTerm;
					
					fInitializerTerm=1;
					switch(this->lexer->sym)
					{
						case sym_kw_integer	:
						case sym_kw_real 	:
						case sym_kw_char 	:
						case sym_kw_byte 	:

							fInitializerTerm = 1;
							parserGetToken(this);
							break ;
							
						case sym_id 		:	// la struttua ha bisogno di un'initializer list
						
							fInitializerTerm = 0 ;
							parserGetToken(this);
							break ;
						
						default: $syntaxError ; break ;
					}
					
	// ............................... (:=)?
	
					if ( fInitializerTerm==1)
					{ 
						if ( this->lexer->sym==sym_assign ) 
						{
							parserGetToken(this);
							
							exprTemp = parserExpr(this);
						}
					}
					
				}
				else 
				{
					$syntaxError ; 
					return NULL ;
				}
			}
			else 
			{
				$syntaxError ; 
				return NULL ;
			}
		
			//
/*			
			stDebugSymTableNode(pstNew) ; // DEBUG
			// cerca se l'identificativo è già presente, nella ST il nuovo e' ancora da inserire
			psymTable_t pstTemp = stFindIDinMap(pstNew->id); 
			
			if ( pstTemp ) 
			{
				$scannerErrorExtra(scanning,duplicateSymbolName,pstNew->id) ;
			}
			
			whmapInsert( mapST, stGetFullName(pstNew->id)   , pstNew ); // altrimenti inserisci name space + id
*/	
			// ---------------
			// make node Var
			// ---------------
			
			n = astMakeNodeDeclVar( this->ast , idTemp , symTemp, exprTemp  , scope  ) ;

			if ( n!=NULL ) astPushNodeBlock( this->ast , nBlock , n ) ;
			
			//

	// ............................... ","
		} while ( this->lexer->sym==sym_v ) ;
		
	// ............................... ";"
	$MATCH( sym_pv, L';' ) ; // can be omitted
	
	}

	// ***
	// END
	// ***
		
	if ( kError ) return NULL ; 

 return nBlock ;
}


/*
 
	### {parserDeclArray}

	"array" -> #1	[id] 	-> ":"	[]		|->		integer		|->		|									->	";" -> #2
											|->		real		|-^		|-> 					->	"," -> #1
											|->		char		|-^		|-> :=	-> InitList  	->
											|->		byte		|-^
											|->		id			|-^
	#2


*/

pnode_t  parserArrayDim( pparser_t this )
{
	node_t* nArrayDim 	=	NULL ;
	
	// ................................ [] index

	// alloca un nuovo il blocco del	vettore di array nod	[][][]
	
	nArrayDim	=	astMakeNodeArrayDim( this->ast );
	
	vectorClear ( nArrayDim->arrayDim.ndx ) ; // azzeralo

	do {
		
		$MATCH( sym_pq0, L'[' ) ;
		
		vectorPushBack (  nArrayDim->arrayDim.ndx , parserExpr ( this ) ) ;
		
		$MATCH( sym_pq1, L']' ) ;
		
	} while ( this->lexer->sym == sym_pq0 ) ;
	
	return nArrayDim ;
}

pnode_t  parserDeclArray( pparser_t this , stScope_t scope )
{
	if ( kError ) return NULL  ;  
	
	// *****
	// BEGIN
	// *****	
	
	node_t* nBlock		=	astMakeNodeBlock(this->ast) ;	// alloca il blocco del vettore di nodi			decl,decl,decl

	node_t* nArrayDim 	=	NULL ; // global all'interno della funzione

	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDeclArray\n" );

	// ............................... "array"
	if ( this->lexer->sym==sym_kw_array )  
	{
		do {
			node_t *n=NULL;
			
			wchar_t*	idTemp 			= NULL 		;
			sym_t		symTemp 		= sym_end 	;
			node_t*		ilTemp			= NULL		;	// initializer list
			
			parserGetToken(this);

	// ............................... new sym table
			//psymTable_t	pstNew = stMakeSymTable() ; // ................... ST
			//pstNew->kind = stKindVar ; // .............................. ST
 
	// ............................... [id]
			if ( this->lexer->sym==sym_id ) 
			{
				idTemp = gcWcsDup( this->lexer->token ) ;
				
				//pstNew->id = gcWcsDup( lexer.token ) ; // ................ ST
				parserGetToken(this);
				
	// ............................... ":"
				if ( this->lexer->sym==sym_dp ) 
				{
					parserGetToken(this);
/*
	// ................................ [] index

					// alloca un nuovo il blocco del	vettore di array nod	[][][]
					nArrayDim	=	astMakeNodeArrayDim( this->ast );	
					vectorClear ( nArrayDim->arrayDim.ndx ) ; // azzeralo
					
					do {
						$MATCH( sym_pq0, L'[' ) ;
						vectorPushBack (  nArrayDim->arrayDim.ndx , parserExpr ( this ) ) ;
						$MATCH( sym_pq1, L']' ) ;
					} while ( this->lexer->sym == sym_pq0) ;
*/
					nArrayDim = parserArrayDim(this);
					
	// ............................... [integer,real,char,byte,id]
	
					symTemp = this->lexer->sym ;
					
					switch(this->lexer->sym)
					{
						case sym_kw_integer	:
						case sym_kw_real 	:
						case sym_kw_char 	:
						case sym_kw_byte 	:
						case sym_id 		:

							parserGetToken(this);
							break ;
						
						default: 
						
							$syntaxError ; 
							return NULL ;
							
							break ;
					}
					
	// ............................... (:=)?

					if ( this->lexer->sym==sym_assign ) 
					{
						parserGetToken(this);
						
						ilTemp = NULL ;
						
						fwprintf ( stderr , L" !! array : initializer list not implemented yet\n") ;
					
						// initializer list
					}

				}
				else 
				{
					$syntaxError ; 
					return NULL ;
				} 
			}
			else 
			{
				$syntaxError ; 
				return NULL ;
			}
		
			//
/*			
			stDebugSymTableNode(pstNew) ; // DEBUG
			// cerca se l'identificativo è già presente, nella ST il nuovo e' ancora da inserire
			psymTable_t pstTemp = stFindIDinMap(pstNew->id); 
			
			if ( pstTemp ) 
			{
				$scannerErrorExtra(scanning,duplicateSymbolName,pstNew->id) ;
			}
			
			whmapInsert( mapST, stGetFullName(pstNew->id)   , pstNew ); // altrimenti inserisci name space + id
*/	
			// ---------------
			// make node Var
			// ---------------
			
			n = astMakeNodeDeclArray( this->ast , idTemp , nArrayDim , symTemp, ilTemp  , scope  ) ;

			if ( n!=NULL ) astPushNodeBlock( this->ast , nBlock , n ) ;
			
			//

	// ............................... ","
		} while ( this->lexer->sym==sym_v ) ;
		
	// ............................... ";"
	$MATCH( sym_pv, L';' ) ; // can be omitted
	
	}

	// ***
	// END
	// ***
		
	if ( kError ) return NULL ; 

 return nBlock ;
}


/*
 
	### {parserDeclType}

	"type" -> [id] 	-> "{"	|->		parserDeclVar		|->		|			 |->							|->	";" -> #2
							|->		parserDeclArray		|-^		|-> 	"}"  |-> := { Initialiser List } 	|-^

	#2


*/

pnode_t  parserDeclType( pparser_t this , stScope_t scope )
{
	if ( kError ) return NULL  ;  
	
	// *****
	// BEGIN
	// *****	
	
	node_t* 	nBlock				=	astMakeNodeBlock(this->ast) ;	// alloca il blocco del vettore di nodi			field,field,field
	node_t*		pnode 				= 	NULL ;
	node_t* 	nBlockVectorTemp 	= 	NULL ;
	size_t 		fDecl				=	0 ;
	wchar_t*	idTemp 				=   NULL ;
	
	// ............................... "type"
	
	if ( this->lexer->sym==sym_kw_type )  
	{ 
		parserGetToken(this);
		
	// ............................... [id]
			if ( this->lexer->sym==sym_id ) 
			{
				idTemp = gcWcsDup( this->lexer->token ) ;
				
				//pstNew->id = gcWcsDup( lexer.token ) ; // ................ ST
				parserGetToken(this);
		
				$MATCH( sym_pg0 , L'{' ) ; 
		
				do {
					
					fDecl=0; 
					
					nBlockVectorTemp	=	parserDeclVar(this,stScopeGlobal); // VAR

					fDecl += astPushAllNodeBlock ( this->ast , nBlock , nBlockVectorTemp ) ;

					nBlockVectorTemp	=	parserDeclArray(this,stScopeGlobal); // ARRAY

					fDecl += astPushAllNodeBlock ( this->ast , nBlock , nBlockVectorTemp ) ;
					
				} while ( 	fDecl > 0 						&&
							this->lexer->sym != sym_end 	&&
							!kError 
						) ;
						
				$MATCH( sym_pg1 , L'}' ) ;
				
			}
			else 
			{
				$syntaxError ; 
				return NULL ;
			} 
			
			// crea nodo type 
			pnode = astMakeNodeDeclType( this->ast , idTemp ,  scope ) ;
			
			// inserisci i nodi nel vettore campi ( field )
			
			// TODO struct without field -> error
			
			const size_t kVectorSize = vectorSize ( nBlock->block.next ) ;
			for ( uint32_t i = 0 ; i < kVectorSize ; i++ )
			{
				pnode_t  node = nBlock->block.next.data[i] ;
				if ( node != NULL ) 
				{
					vectorPushBack( pnode->declType.field , node  ) ;
				}
			}

		$MATCH( sym_pv , L';' ) ;
	}

	// ***
	// END
	// ***
		
	if ( kError ) return NULL ; 

 return pnode ;
}

// ......................................................... parser declaration	:	Const, Var, Array,	Type

pnode_t parserDeclaration( pparser_t this , node_t* nBlock , stScope_t	scope ) 
{
	pnode_t 	pnode = NULL ;
	node_t* 	nBlockVectorTemp 	= NULL 	;
	size_t 		fDecl=0;

	// -----------
	// DECLARATION
	// -----------
			
	do {
		
		fDecl=0; 
		
		// dato che è un loop potrebbe esserci sia dichiarazioni di const/var come potrebbero non esserci

		nBlockVectorTemp	=	parserDeclConst(this,scope);
		fDecl += astPushAllNodeBlock ( this->ast , nBlock , nBlockVectorTemp ) ; 

		nBlockVectorTemp	=	parserDeclVar(this,scope);
		fDecl += astPushAllNodeBlock ( this->ast , nBlock , nBlockVectorTemp ) ;

		nBlockVectorTemp	=	parserDeclArray(this,scope);
		fDecl += astPushAllNodeBlock ( this->ast , nBlock , nBlockVectorTemp ) ;
		
		pnode = parserDeclType( this , scope ) ;
		astPushNodeBlock( this->ast , nBlock , pnode ) ;
		if (pnode!=NULL) fDecl++; 

	} while ( 	fDecl > 0 						&&
				this->lexer->sym != sym_end 	&&
				!kError 
			) ;
			
	return nBlock ;
}


/*
 
	### {parserDeclFunction}


	"function" 	f	( parserDeclVar	|| parserDeclArray) : integer || real { block } ;
	 
	#2i


*/

pnode_t  parserDeclFunction( pparser_t this )
{
	if ( kError ) return NULL  ;  
	
	// *****
	// BEGIN
	// *****	

	node_t* 	nBlockParam			=	astMakeNodeBlock(this->ast) ;	// alloca il blocco del vettore di nodi			param,param,param
	node_t* 	nBlockCode			=	astMakeNodeBlock(this->ast) ;
	
	node_t*		pnode 				= 	NULL ;
	node_t* 	nBlockVectorTemp 	= 	NULL ;
	size_t 		fDecl				=	0 ;
	wchar_t*	idTemp 				=   NULL ;
	sym_t		retTypeTemp 		=   sym_end ;
	
	// ............................... "type"
	
	if ( this->lexer->sym==sym_kw_function )  
	{ 
		parserGetToken(this);
		
	// ............................... [id]
			if ( this->lexer->sym==sym_id ) 
			{
				idTemp = gcWcsDup( this->lexer->token ) ;

				//pstNew->id = gcWcsDup( lexer.token ) ; // ................ ST
				parserGetToken(this);
				
	// ............................... [(]
	
				$MATCH( sym_p0 , L'(' ) ; 

	// ............................... [param list] : var ...,array ...
			
				do {
					
					fDecl=0; 
					
					nBlockVectorTemp	=	parserDeclVar(this,stScopeLocal); // VAR

					fDecl += astPushAllNodeBlock ( this->ast , nBlockParam , nBlockVectorTemp ) ;

					nBlockVectorTemp	=	parserDeclArray(this,stScopeLocal); // ARRAY

					fDecl += astPushAllNodeBlock ( this->ast , nBlockParam , nBlockVectorTemp ) ;
					
				} while ( 	fDecl > 0 						&&
							this->lexer->sym != sym_end 	&&
							!kError 
						) ;
						
	// ............................... [)]
	
				$MATCH( sym_p1 , L')' ) 
				
	// ............................... [return type] ( integer , real , byte )
	
				switch(this->lexer->sym)
				{
					case	sym_kw_integer	:
					case	sym_kw_real		:
					case	sym_kw_byte		:
					
						retTypeTemp = this->lexer->sym ;
						parserGetToken(this);
						break ;
						
					default:
						$syntaxError ; 
						return NULL ;
						break ;
				}
					
	// ............................... [{]
	
				$MATCH( sym_pg0 , L'{' ) ;
				

	// ............................... [function block]

	nBlockCode = parserStatement ( this , nBlockCode ) ;

//OK				//nBlockCode = parserExpr(this);
/*
	do {

		if ( kError ) break ;
		
		pnode=parserExpr(this);
		
		//$MATCH( sym_pv , L';' ) ;
		
		if ( pnode!=NULL ) astPushNodeBlock( this->ast , nBlockCode , pnode );
		
	} while ( 		pnode!=NULL 
				&&  this->lexer->sym != sym_end 
				&&  this->lexer->sym != sym_pv // occorre identificare un termine di uscita
				&&  !kError 
			) ;
$MATCH( sym_pv , L';' ) ;
*/
//fwprintf ( stderr , L"\n§§§ %p %d\n",nBlockCode,0 ) ;

	// ............................... [}]

				$MATCH( sym_pg1 , L'}' ) ;
				
			}
			else 
			{
				$syntaxError ; 
				return NULL ;
			} 
			
			// crea nodo type 
			pnode = astMakeNodeDeclFunction( this->ast , idTemp ,  retTypeTemp ,  NULL , nBlockCode ) ;
 ;

/*			
			// inserisci i nodi nel vettore campi ( field )
			
			// TODO struct without field -> error
*/	
		
			const size_t kVectorSize = vectorSize ( nBlockParam->block.next ) ;
			
			for ( uint32_t i = 0 ; i < kVectorSize ; i++ )
			{
				pnode_t  node = nBlockParam->block.next.data[i] ;
				if ( node != NULL ) 
				{
					vectorPushBack( pnode->declFunction.param , node  ) ;
				}
			}

		$MATCH( sym_pv , L';' ) ;
	}

	// ***
	// END
	// ***
		
	if ( kError ) return NULL ; 

 return pnode ;
}



/**/



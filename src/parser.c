
#include "lexer.h"
#include "parser.h"


// ................................................... MATCH

wchar_t* doMatchMessage( wchar_t C0 , wchar_t* token ) 
{
	wchar_t* extra0 = gcWcsDup(L" expected [?] instead -> [") ; // [ 11 ] posizione
    extra0[11] 		= C0 ;

	if ( wcslen(token) > 20 ) 
	{
		token[20]=0;
		token[19]=L'.';
		token[18]=L'.';
		token[17]=L'.';
	}

	uint32_t extraSize = wcslen( extra0 ) ;
	uint32_t tokenSize = wcslen( token  ) ;	
	wchar_t* extra = gcMalloc( sizeof(wchar_t) * (tokenSize + extraSize + 3 ) ) ;

	wcscpy ( extra , extra0 ) ;
	wcscat ( extra , token ) ;
	wcscat ( extra , L"] " ) ;

	return extra ;
}

// ......................................................... parserPrintToken

int parserPrintToken( pparser_t this )
{
    if ( this->lexer->sym == sym_weof ) return 0; // non viene visualizzato, viene ritornato solo END
    if ( this->lexer->sym == sym_end  ) return 0; // non viene visualizzato ( this->lexer->fileInputName NULL )

    // better format

    fwprintf ( this->pFileOutputParser,L"-" ) ; // migliora visualizzazione con output : stdout > file.txt

    if ( wcslen(this->lexer->fileInputName) > 20 )
        fwprintf ( this->pFileOutputParser,L"[%-17.17ls...]" ,this->lexer->fileInputName ) ;
    else
        fwprintf ( this->pFileOutputParser,L"[%-20.20ls]"    ,this->lexer->fileInputName ) ;
    
    fwprintf ( this->pFileOutputParser , L"[%03d,%03d] len(%02d) sym(%03d) "
		,	this->lexer->row_start
		,	this->lexer->col_start-1
		,	(int)this->lexer->tokenSize
		,	(uint32_t)this->lexer->sym
    ) ;    

    if ( wcslen(this->lexer->token) > 20 )
        fwprintf ( this->pFileOutputParser,L"[%.17ls...]" ,this->lexer->token ) ;
    else
        fwprintf ( this->pFileOutputParser,L"[%20ls]" ,this->lexer->token ) ;

#if defined(_MSC_VER)
    if ( this->lexer->sym==sym_integer )  fwprintf ( this->pFileOutputParser , L" -> [[%lld]]",this->lexer->value.integer  ) ;
#else
    if ( this->lexer->sym==sym_integer )  fwprintf ( this->pFileOutputParser , L" -> [[%ld]]",this->lexer->value.integer   ) ;
#endif
    if ( this->lexer->sym==sym_real    )  fwprintf ( this->pFileOutputParser , L" -> [[%lf]]",this->lexer->value.real      ) ; 

    if ( this->lexer->sym==sym_char    )  fwprintf ( this->pFileOutputParser , L" -> [[%lc]]",g.outputSpecialCharInChar(this->lexer->value.wchar)     ) ;
    
    if ( this->lexer->sym==sym_string  )  fwprintf ( this->pFileOutputParser , L" -> [[%ls]]",g.outputSpecialCharInString(this->lexer->value.wstring)   ) ;
    
    fwprintf ( this->pFileOutputParser , L"\n" ) ;
    
    return 0 ;
}


// ................................................... Parser Debug Get Token

sym_t parserGetToken( pparser_t this )
{
    lexerGetToken(this->lexer) ;

    if ( this->fDebug ) 
    {
		parserPrintToken(this);
    }

   return this->lexer->sym ;    
} 

// ......................................................... parser alloc

pparser_t  	parserAlloc			( void )
{
	pparser_t pParser = gcMalloc( sizeof(parser_t) ) ;
	
	if ( pParser==NULL )
	{
		$parserInternal( malloc , outOfMemory , NULL , NULL ) ;
	}
	else // init default parameter
	{
		pParser->fDebug 				= 0 		;
		pParser->pFileOutputParser 		= NULL   	;  
		pParser->fileNameOutputParser	= NULL	    ;
		pParser->fileInputName			= NULL		;
		pParser->lexer 					= NULL		; 
		pParser->ast					= NULL 		;
	}

	return pParser ;
}

// ......................................................... parser dealloc

void  	parserDealloc			( pparser_t this )
{
	if ( this!=NULL )
	{
		
		gcFree(this);
	}
	else
	{
		$parserInternal( dealloc , errUnknown , NULL , NULL );
	}
}

// ......................................................... parser constructor

void parserCtor( pparser_t this )
{
	// file debug parser
	
	if ( this->fDebug ) 
	{
		if ( this->fileInputName!=NULL )
		{
				this->fileNameOutputParser  = g.makeFileWithNewExt( this->fileInputName , ".parser"  ) ;
				stdFileWOpen ( &this->pFileOutputParser , this->fileNameOutputParser , "w+","ccs=UTF-8" ) ;
				
				if ( this->pFileOutputParser != NULL )
				{
					fwprintf ( this->pFileOutputParser , L"\n%-20ls : [%018p] -> [%-20hs]\n" 
						,L"file parser"	    
						,this->pFileOutputParser 
						,this->fileNameOutputParser
					) ;
				}
				
				// *********
				//  LEXER
				// *********

				plexer_t lexer = lexerAlloc();

				lexerCtor(lexer);

				if ( this->fDebug == 1 ) lexer->fDebug = 1 ;

				this->lexer = lexer ;

				// *********
				//  AST
				// *********

				past_t	ast = astAlloc();

				if ( this->fDebug == 1 ) ast->fDebug = 1 ;

				astCtor(ast,this->fileInputName);

				this->ast =ast ;

		}
		else
		{
			this->lexer = NULL ;
			this->ast	= NULL ;
			this->pFileOutputParser = NULL ;
			$parserInternal( checkFileExists , noInputFiles , L"{null}" , NULL );
		}
	}

}

// ......................................................... parser destructor

void parserDtor( pparser_t this )
{
	if ( this->fDebug) 	if ( this->pFileOutputParser!=NULL ) fclose(this->pFileOutputParser);

	if ( this->lexer != NULL )
	{
		lexerDtor(this->lexer);

		lexerDealloc(this->lexer);
	}

	if ( this->ast != NULL )
	{
		astDtor(this->ast);

		astDealloc(this->ast);
	}
}

// ......................................................... parser statement

pnode_t parserStatement( pparser_t this , node_t* nBlock ) 
{
	pnode_t 	pnode = NULL ;
	//node_t* 	nBlockVectorTemp 	= NULL 	;
	//size_t 		fDecl=0;
	
	// -----------
	// EXPR
	// -----------

	pnode = NULL ;

	do {
		
		if  ( this->lexer->sym==sym_pv)  parserGetToken(this);

		if ( kError ) break ;
		
		pnode=parserExpr(this);
		
		if ( pnode!=NULL ) astPushNodeBlock( this->ast , nBlock , pnode );
		
		//if  ( this->lexer->sym==sym_pv) $MATCH( sym_pv , L';' ) else break ;
		
	} while ( 		pnode!=NULL 
				&&  this->lexer->sym != sym_end 
				&&  this->lexer->sym == sym_pv
				&&  !kError 
			) ;
			
	//if ( this->lexer->sym==sym_pv) $MATCH( sym_pv , L';' ) ;

	return nBlock ;
}

// ......................................................... parser Function

pnode_t parserFunction( pparser_t this , node_t* nBlock ) 
{
	pnode_t 	pnode = NULL ;

	// -----------
	// Function
	// -----------

	pnode = NULL ;

	do {

		if ( kError ) break ;
		
		pnode=parserDeclFunction(this);
		
		if ( pnode!=NULL ) astPushNodeBlock( this->ast , nBlock , pnode );
		
	} while ( 		pnode!=NULL 
				&&  this->lexer->sym != sym_end 
				&&  !kError 
			) ;

	return nBlock ;
}

// ......................................................... parser block : Declaration , Statement

pnode_t parserBlock( pparser_t this , stScope_t	scope )
{
	// *********
	//  BLOCK
	// *********

	node_t* nBlock=astMakeNodeBlock(this->ast);

	nBlock = parserDeclaration( this ,  nBlock , scope ) ;

	nBlock = parserStatement( this ,  nBlock ) ;

	return nBlock ;
}

// ......................................................... parser block : Declaration , Function , statement

pnode_t parserMainBlock( pparser_t this )
{
	// *********
	//  MAIN
	// *********

	node_t* nBlock=astMakeNodeBlock(this->ast);

	nBlock = parserDeclaration( this ,  nBlock , stScopeGlobal ) ;

	nBlock = parserFunction( this ,  nBlock ) ;
	
	nBlock = parserStatement( this ,  nBlock ) ;

	//fwprintf ( stderr, L"\n§§§[%ls %d]\n",this->lexer->token,this->lexer->sym ) ;

	if ( this->lexer->sym != sym_end ) $syntaxError ;

	return nBlock ;
}

// ......................................................... parser scan 

pnode_t parserScan( pparser_t this )
{
	// parser start message
		
	if ( this->fDebug ) fwprintf ( this->pFileOutputParser,L"\n# Parser  -> scanning ...\n\n"  );
	
	// parser init

		pnode_t	pnode = NULL ;

		lexerInclude ( this->lexer , this->fileInputName ) ;

		parserGetToken(this);
		
	// parser begin
	
		pnode = parserMainBlock( this ) ; // decl(global); Function; statement[expr];
	
	// parser end

	// ritorna il nodo principale ( GC for deallocation )

	return pnode ;
}



/**/



/* 
  
		// parser init
		
			// *********
			//  LEXER
			// *********

			plexer_t lexer = lexerAlloc();
			
			lexerCtor(lexer);
			
			if ( this->fDebug == 1 ) lexer->fDebug = 1 ;

			// *********
			//  NODE
			// *********
			
			pnode_t	pnode = NULL ;
					
			// *********
			//  AST
			// *********
			
			past_t	ast = astAlloc(pnode);
			
			if ( this->fDebug == 1 ) ast->fDebug = 1 ;
			
			astCtor(ast,fileInputName);

		// parser loop

			// LEXER
			
				lexerInclude ( lexer , fileInputName ) ;
				for(;;)
				{
					lexerGetToken(lexer) ;
					if ( lexer->sym == sym_end ) break; 
				}
				
			// END LEXER
				
		// parser exit	
				
			lexerDtor(lexer);
			
			lexerDealloc(lexer);

			astDtor(ast);
			
			astDealloc(ast);
			
			// ritorna il nodo principale
			// ( GC for deallocation )
			
		return pnode ;

*/



#include "error.h"
#include "lexer.h"
#include "../lib/mstack.h"
#include "ast.h"
#include "parser.h"
//#include "symTable.h"


// ................................................... private prefix in expr

static
psPrefixOp_t    parserPrefixNew( plexer_t lexer )
{
    psPrefixOp_t    prefix    = gcMalloc( sizeof(struct sPrefixOp_s) ) ;
    
    if ( prefix == NULL )
    {
         $astInternal ( malloc , outOfMemory , L"expr.c" , L"parserMakePrefix") ;
    }
    else
    {
        prefix->sym             = lexer->sym ;
        prefix->row_start        = lexer->row_start ;
        prefix->col_start         = lexer->col_start ;
        prefix->token             = gcWcsDup( lexer->token ) ;
        prefix->fileInputName     = gcWcsDup( (wchar_t*)lexer->fileInputName ) ;
    }
    return prefix ;
}

static
psPrefixOp_t    parserPrefixDelete( psPrefixOp_t prefix )
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
            ||    this->lexer->sym == sym_sub
            ||    this->lexer->sym == sym_not
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
                Gestione Array Function Const Var Field
                
                1) id    [    :    array
                2) id    (    :    function
                3) id         :    const
                4)            :    var
                5)            :    field

        */
         
        case sym_id : // Array || Function || Const || Var || Field
        {
                wchar_t*    idTemp = gcWcsDup ( this->lexer->token)  ;
                uint32_t    rowTemp = this->lexer->row ;
                uint32_t    colTemp = this->lexer->col ;
                 
                fwprintf ( this->pFileOutputParser , L"%-30ls :: [%ls].\n",L"ID",this->lexer->value.id );
                    
                parserGetToken(this);
                
                switch ( this->lexer->sym )
                {
                    case    sym_pq0    :    // #1 ARRAY [
                            {
                                node_t* nArrayDim    =    parserArrayDim(this);
                                if (nArrayDim!=NULL )
                                {
                                    n=astMakeNodeTermArray    ( this->ast , idTemp  , nArrayDim ) ;
                                } ;
                            }
                            break;
                            
                    case    sym_p0:        // #2 FUNZIONE (
                            {
                                node_t*     nBlockParam            =    astMakeNodeBlock(this->ast) ;
                                
                                $MATCH( sym_p0 , L'(' ) ;
                                
                                    int fLoop=0;
                                    do {
                                        fLoop=0; // REPETITA JUVANT :
                                        pnode_t nTemp    =    parserExpr(this); // -> esce con termine nuovo, è all'entrata che ocorre getToken.

                                        astPushNodeBlock ( this->ast , nBlockParam , nTemp ) ; // controllo automatico NULL

                                        if ( this->lexer->sym == sym_v ) { fLoop=1; parserGetToken(this); } ;
                                    } while ( 
                                                fLoop==1      &&
                                                this->lexer->sym != sym_end     &&
                                                !kError 
                                            ) ;

                                $MATCH( sym_p1 , L')' ) ;

                                if (nBlockParam!=NULL )
                                {
                                        n=astMakeNodeTermFunction    ( this->ast , idTemp  , nBlockParam ) ;
                                } ;
                            }
                            break;

                    default:
                    {
                        // [v] variabile    :    termVar
                        // [] costante      :    return Term const
                        // [] tipo          :    termID

                        psymTable_t pstTemp = stFindIDinMap(idTemp);

                        if ( !pstTemp ) // se != 0 è già presente
                        {
                            $pushErrLog
                            (parser,error,parseExpr,symbolNotDeclared,rowTemp,(colTemp-wcslen(idTemp)+1),this->lexer->fileInputName , idTemp) ;
                            n=NULL ;
                        }
                        else
                        {
                            // sostituisci la constante simbolo con la costante intera
                            
                            switch ( pstTemp->kind ) 
                            {
                                case    stKindConst    : // ............. #3
                                        // replace const
                                        if ( wcslen(idTemp) > maxTokenSize ) idTemp[maxTokenSize-1]=0;
                                        wcscpy ( this->lexer->token 	, 	idTemp ) ;
                                        this->lexer->row	=	rowTemp ;
                                        this->lexer->col	=	colTemp ;
                                        
                                        switch (pstTemp->type)
                                        {
											case	stTypeInteger  :
												n=astMakeNodeTermInteger(this->ast,this->lexer,pstTemp->value.integer ) ;
												break;
												
											case	stTypeReal  :
												n=astMakeNodeTermReal(this->ast,this->lexer,pstTemp->value.real ) ;
												break;
												
											case	stTypeChar  :
												n=astMakeNodeTermChar(this->ast,this->lexer,pstTemp->value.wchar ) ;
												break;
												
											case	stTypeConstString  :
												n=astMakeNodeTermString(this->ast,this->lexer,pstTemp->value.wstring ) ;
												break;
												
											default:
												$pushErrLog
												(parser,internal,parseExpr,errUnknown,rowTemp,(colTemp-wcslen(idTemp)+1),L"expr.c" 
												, L"parserTerm -> .... switch ( pstTemp->kind ) -> switch (pstTemp->type)" ) ;
												n=NULL ;
											break;
										}

                                        break;
                                        
                                case    stKindVar    :  // .............. #4
                                        n=astMakeNodeTermVar ( this->ast , idTemp , rowTemp , colTemp ) ;
                                        break;
                                        
                                default: // ............................. #5
                                        $pushErrLog
                                        (parser,error,parseExpr,invalidUseOf,rowTemp,(colTemp-wcslen(idTemp)+1),this->lexer->fileInputName , idTemp) ;
                                        n=NULL ;
                                        break;
                            }
                        }
                        break;
                    }
                }
        }
        break;

        default:
            
            /*
               Le espressioni come le frasi del parser possono ritornare NULL, 
               come possiamo vedere nella definizione degli array C : int a[] ...
               per tanto andra gestita l'espressione NULL e con quanto ne consgue. 
            */
            
            //$syntaxError

            n=NULL ; // non trova espressione allora ritorna NULL !
            
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

// ................................................... dot .

node_t* parserDot( pparser_t this )
{
	node_t *left=NULL;
	
	if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDot BEGIN\n" ) ;

	// ---------------------------------------------------------------------------------------------------- TERM
	
	left=parserTerm(this);

	if (	this->lexer->sym != sym_dot	 )
	{
		if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDot END\n" ) ;
		return left ;
	}
	
	// ---------------------------------------------------------------------------------------------------- TERM STRUCT
	
		if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDot LOOP\n" );
	
		size_t vSizeSave = vectorSize(stNameSpace) ; 
		// salva le dimensione del name space in quanto per ogni iterazione viene aggiunto il livello di ricerca
	
		psymTable_t pstTemp = NULL ;
		pnode_t 	nStruct = astMakeNodeTermStruct( this->ast ) ;

		if ( ( left->type != nTypeTermVar 		)	) pstTemp = stFindIDinMap(left->termVar.id);
		if ( ( left->type != nTypeTermArray 	)	) pstTemp = stFindIDinMap(left->termArray.id);

		if ( pstTemp == NULL )
		{
			fwprintf ( stderr, L"\n!! [expr.c::parserDot] :  ( pstTemp == NULL )\n") ;	
			exit(-1) ;
		}
		
		// trovando id del primo elemento, troveremo anche il nome della struttura
		// l1.x -> l1(line_t).x
		// fwprintf(stderr,L"\n[[%ls]]\n",pstTemp->typeID ) ;
	
		nStruct->termStruct.id 		= gcWcsDup ( pstTemp->typeID ) ;

		// ricerca il riferimento alla struttura
		
		psymTable_t pstTypeNameTemp  = stFindIDinMap(pstTemp->typeID);
		if ( pstTypeNameTemp == NULL )
		{
			fwprintf ( stderr, L"\n!! [expr.c::parserDot] :  ( pstTypeNameTemp == NULL )\n") ;	
			exit(-1) ;
		}
		nStruct->termStruct.pvst	= (void*)pstTypeNameTemp ;

		while (	this->lexer->sym == sym_dot	)
		{
/*			
fwprintf ( stderr, L"\n namespace : " );
for(int i=0;i<vectorSize(stNameSpace);i++)
{
	fwprintf ( stderr, L"[%ls]", vectorAt( stNameSpace , i ) );
}
*/
				if ( ( left->type != nTypeTermVar )	&& ( left->type != nTypeTermArray) ) 
				{
					fwprintf ( stderr, L"\n!! [expr.c::parserDot] : term not var or array\n") ;	
					exit(-1) ;
				}
				
				if ( ( left->type != nTypeTermVar 		)	) pstTemp = stFindIDinMap(left->termVar.id);
				if ( ( left->type != nTypeTermArray 	)	) pstTemp = stFindIDinMap(left->termArray.id);

				if ( ( left->type != nTypeTermVar 	)	) //	campo var
				{
					//fwprintf ( stderr,L"{{%d}}",vectorSize(stNameSpace)) ;
					if ( vectorSize(stNameSpace)>=2 ) vectorPopBack(stNameSpace);
					vectorPushBack(stNameSpace,pstTemp->typeID 	) ;
//fwprintf ( stderr, L"[[%ls]]", left->termVar.id );
					pstTemp = stFindIDinMap(left->termVar.id);
				}
				if ( ( left->type != nTypeTermArray	) 	) //	campo array
				{
					//fwprintf ( stderr,L"{{%d}}",vectorSize(stNameSpace)) ;
					if ( vectorSize(stNameSpace)>=2 ) vectorPopBack(stNameSpace);
					vectorPushBack(stNameSpace,pstTemp->typeID 	) ;
					pstTemp = stFindIDinMap(left->termArray.id);
				}

				vectorPushBack ( nStruct->termStruct.vField , left ) ;

				//fwprintf ( stderr, L"\n!! [expr.c::parserDot] : pst[%p]\n",pstTemp) ;
				//exit(-1);

				parserGetToken(this);

				left=parserTerm(this);

		}
		// inserisci ultimo campo struttura
		vectorPushBack ( nStruct->termStruct.vField , left ) ;
		
		// ripristina le dimensionie del name space
		stNameSpace.size = vSizeSave ;

		return nStruct ;
}

// ................................................... mul div mod * / % 

node_t* parserMulDivMod( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserMulDivMod\n" );

    left=parserDot(this);

    while (     this->lexer->sym == sym_mul
        || 		this->lexer->sym == sym_div
        ||    	this->lexer->sym == sym_mod
        )
    {
        sym_t       symSave   	= this->lexer->sym ;
        wchar_t*    tokenSave   = gcWcsDup(this->lexer->token);
        uint32_t    rowSave     = this->lexer->row_start ;
        uint32_t    colSave     = this->lexer->col_start ;
        
        node_t *right=NULL;

        parserGetToken(this);

        right=parserDot(this);
        
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
        
        left->token 	=  	tokenSave	; 
        left->row    	= 	rowSave     ;
        left->col    	= 	colSave     ;
    } 

 return left ;
}

// ................................................... Add Sub + - 

node_t* parserAddSub( pparser_t this )
{
    node_t *left=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserAddSub\n" );

    left=parserMulDivMod(this);

    while (     this->lexer->sym == sym_add
        ||        this->lexer->sym == sym_sub
        )
    {
        sym_t         symSave      = this->lexer->sym ;
        wchar_t*    tokenSave     = gcWcsDup(this->lexer->token);
        uint32_t    rowSave     = this->lexer->row_start ;
        uint32_t    colSave     = this->lexer->col_start ;
        
        node_t *right=NULL;

        parserGetToken(this);

        right=parserMulDivMod(this);

        left = astMakeNodeBinOP( this->ast,this->lexer,symSave , right,left ) ;
        
        left->token =     tokenSave     ; 
        left->row    =    rowSave     ;
        left->col    =    colSave     ;
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


#include "global.h"
#include "error.h"
#include "ast.h"

// TERM : INTEGER

node_t* astMakeNodeTermInteger( past_t this , plexer_t lexer , int64_t _integer )
{
    #ifdef _MSC_VER
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lld]\n",L"astMakeNodeTermInteger",_integer ) ;
    #else
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ld]\n",L"astMakeNodeTermInteger",_integer  ) ;
    #endif

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermInteger") ;

    nNew->type         = nTypeTermInteger ;
    nNew->term.integer = _integer ;
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;   

    return nNew ;
}

// TERM : REAL

node_t* astMakeNodeTermReal( past_t this , plexer_t lexer , double _real )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lf]\n",L"astMakeNodeTermReal",_real ) ;
  
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermReal") ;

    nNew->type         = nTypeTermReal ;
    nNew->term.real    = _real ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;  
    
    return nNew ;
}

// TERM : CHAR

node_t* astMakeNodeTermChar( past_t this , plexer_t lexer , wchar_t _wchar )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lc]\n",L"astMakeNodeTermChar",g.outputSpecialCharInChar(_wchar) );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermChar") ;

    nNew->type             = nTypeTermChar ;
    nNew->term.wchar     = _wchar ;
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// TERM : STRING

node_t* astMakeNodeTermString( past_t this , plexer_t lexer , wchar_t* _wstring )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeNodeTermString",g.outputSpecialCharInString(_wstring) );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermString") ;

    nNew->type         = nTypeTermString ;
    nNew->term.wstring = gcWcsDup(_wstring);
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// TERM : VAR

node_t* astMakeNodeTermVar( past_t this , wchar_t* _name , uint32_t row , uint32_t col )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeNodeTermVar",_name );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermVar") ;

    nNew->type           = nTypeTermVar ;
    nNew->termVar.id     = gcWcsDup(_name);
    
    nNew->row    =    row ;
    nNew->col    =    col - 1;
    nNew->token  =    gcWcsDup(_name) ;    

    return nNew ;
}

// TERM : ID

node_t* astMakeNodeTermField( past_t this , plexer_t lexer , wchar_t* _name )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeNodeTermField",_name );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermField") ;

    nNew->type           = nTypeTermField ;
    nNew->termField.id     = gcWcsDup(_name);
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// BINOP

node_t* astMakeNodeBinOP(  past_t this , plexer_t lexer , sym_t sym , node_t* left , node_t* right  )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: sym [%d]\n",L"astMakeNodeBinOP",sym );
  
    if ( left==NULL || right==NULL ) // se uno dei due operandi è nullo 
    {
        $pushErrLog( parser,error,parseExpr,expectedPrimaryExprBefore,lexer->row,lexer->col,lexer->fileInputName,lexer->token ) ;
    }
 
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeBinOP") ;

    nNew->type          = nTypeBinOp;
    nNew->binOp.sym     = sym       ;  
    nNew->binOp.left    = left      ;
    nNew->binOp.right   = right     ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;  
    
    return nNew ;
}

// PREFIX

node_t* astMakeNodePrefix( past_t this , psPrefixOp_t prefix , node_t* left ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%d]\n",L"astMakeNodePrefix",prefix->sym );
    
     if ( left==NULL ) 
    {
        $pushErrLog( parser,error,parseExpr,expectedPrimaryExprAfter,prefix->row_start,prefix->col_start,prefix->fileInputName,prefix->token ) ;
    } 

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodePrefix") ;

    nNew->type         = nTypePrefix    ;
    nNew->prefix.sym   = prefix->sym    ;
    nNew->prefix.right = left             ;

    nNew->row    =    prefix->row_start ;
    nNew->col    =    prefix->col_start -1 ;
    nNew->token  =    gcWcsDup(prefix->token)  ; 

    return nNew ;
}

// BLOCK

node_t* astMakeNodeBlock( past_t this ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeBlock" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeBlock") ;

    nNew->type = nTypeBlock  ;

    nNew->row    =    0;
    nNew->col    =    0;
    nNew->token  =    NULL ;

    vectorNew(nNew->block.next,128);

    return nNew ;
}

size_t astPushNodeBlock( past_t this , node_t * nBlock , node_t * next ) 
{
    if ( nBlock  == NULL ) return 0 ;
    if ( next    == NULL ) return 0 ;
    
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [block]:[%018p] -> [next]:[%018p]\n",L"astPushNodeBlock",nBlock,next );
    
    if ( next != NULL )    vectorPushBack( nBlock->block.next , next  ) ;
    
    return vectorSize(nBlock->block.next);
}

size_t     astPushAllNodeBlock    ( past_t this , node_t * nBlockDest ,  node_t * nBlockSource )
{
    if ( nBlockDest   == NULL ) return 0 ;
    if ( nBlockSource == NULL ) return 0 ;
    
    size_t kBlockSize = vectorSize ( nBlockSource->block.next ) ;

    if (kBlockSize > 0 )
    {
        for ( uint32_t i = 0 ; i < kBlockSize ; i++ )
        {
            astPushNodeBlock( this , nBlockDest , nBlockSource->block.next.data[i] );
        }
    }
    return kBlockSize ;
} 

// DECL CONST

pnode_t     astMakeNodeDeclConst    ( past_t this , wchar_t* id ,  sym_t sym , pnode_t _term , stScope_t    scope ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astmakeNodeDeclConst" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astmakeNodeDeclConst") ;

    nNew->type                    =    nTypeDeclConst     ;
    nNew->declConst.id            =    gcWcsDup( id )     ;
    nNew->declConst.sym           =    sym                ;
    nNew->declConst.term          =    _term              ;
    nNew->declConst.scope         =    scope              ;

    return nNew ;
}

// DECL VAR

pnode_t     astMakeNodeDeclVar    ( past_t this , wchar_t* id ,  sym_t sym , pnode_t _expr , stScope_t    scope ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeDeclVar" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeDeclVar") ;

    nNew->type              =    nTypeDeclVar    ;
    nNew->declVar.id        =    gcWcsDup( id )  ;
    nNew->declVar.sym       =    sym             ;
    nNew->declVar.expr      =    _expr           ;
    nNew->declVar.scope     =    scope           ;
    
    return nNew ;
}

// DECL ARRAY

pnode_t     astMakeNodeDeclArray    ( past_t this , wchar_t* id ,  pnode_t _dim  , sym_t sym , pnode_t _il , stScope_t    scope ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeDeclArray" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeDeclArray") ;

    nNew->type                    =    nTypeDeclArray     ;
    nNew->declArray.id            =    gcWcsDup( id )     ;
    nNew->declArray.dim           =    _dim               ;
    nNew->declArray.sym           =    sym                ;
    nNew->declArray.il            =    _il                ;
    nNew->declArray.scope         =    scope              ;

    return nNew ;
}

// ARRAY DIM

pnode_t     astMakeNodeArrayDim    ( past_t this ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeArrayDim" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeArrayDim") ;

    nNew->type     =     nTypeArrayDim    ;

    vectorNew ( nNew->arrayDim.ndx , 12 ) ;    // alloca un massimo di dodici indici

    return nNew ;
}

// node terminale variabile semplice

node_t* astMakeNodeAssign  (  past_t this , plexer_t lexer ,  node_t * lhs , node_t * rhs )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeAssign" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeAssign") ;

    nNew->type = nTypeAssign  ;
    nNew->assign.lhs = lhs ;
    nNew->assign.rhs = rhs ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ; ;

    return nNew ;
}

// DECL TYPE

pnode_t     astMakeNodeDeclType    ( past_t this , wchar_t* id , stScope_t    scope ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeDeclArray" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeDeclArray") ;

    nNew->type                  =     nTypeDeclType    ;
    nNew->declType.id           =    gcWcsDup( id )    ;
    nNew->declType.scope        =    scope             ;

     vectorNew( nNew->declType.field , 12 ) ; // struttura iniziale con 12 membri

    return nNew ;
}

// DECL FUNCTION

pnode_t     astMakeNodeDeclFunction    ( past_t this , wchar_t* id , sym_t retType , pnode_t pParamList , pnode_t pBlockCode ) 
{
	(void)pParamList ;
	
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeDeclFunction" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeDeclFunction") ;

    nNew->type                     =     nTypeDeclFunction    ;
    nNew->declFunction.id          =    gcWcsDup( id )        ;
    nNew->declFunction.retType     =    retType               ;
    //nNew->declFunction.paramList =    pParamList            ;
    nNew->declFunction.blockCode   =    pBlockCode            ;

    vectorNew(     nNew->declFunction.param , 12 ) ; // struttura iniziale con 12 membri

    return nNew ;
}

// TERM ARRAY

pnode_t     astMakeNodeTermArray    ( past_t this , wchar_t* id  , pnode_t pArrayDim ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeTermArray" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermArray") ;

    nNew->type                    =    nTypeTermArray        ;
    nNew->termArray.id            =    gcWcsDup( id )        ;
    nNew->termArray.dim           =    pArrayDim             ;

    return nNew ;
}

// TERM FUNCTION

pnode_t     astMakeNodeTermFunction    ( past_t this , wchar_t* id  , pnode_t pArrayParam ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeTermFunction" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermFunction") ;

    nNew->type                  =     nTypeTermFunction     ;
    nNew->termFunction.id       =     gcWcsDup( id )        ;
    nNew->termFunction.param    =     pArrayParam           ;

    return nNew ;
}

// TERM STRUCT

node_t* astMakeNodeTermStruct( past_t this ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeTermStruct" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermStruct") ;

    nNew->type = nTypeTermStruct  ;

    nNew->row    =    0;
    nNew->col    =    0;
    nNew->token  =    NULL ;

    vectorNew(nNew->termStruct.vField,128);

    return nNew ;
}


// ***********
// astDebug
// ***********

#define $astDebugRowColToken(FLAG)    if ( this->FLAG ) fwprintf ( this->pFileOutputNode , L" { %03d/%03d :: %ls }\n",n->row,n->col,n->token );

node_t* astNodeDebug( past_t this , node_t* n) 
{
  if ( n == NULL ) return NULL  ;

/*
  // level
  static int nodeLevel=-1;
  ++nodeLevel;

  // visualizzazione pessima con i prefissi meglio una di fila all'altra
  for(int i=0;i<nodeLevel;i++) 
    if ( this->fDebug ) 
        fwprintf ( this->pFileOutputNode , L"   " ) ; // 4 space for level
*/  
  // 
  
  switch ( n->type )
  {
      case  nTypeUndefined :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] undefined  :: \n",(void*)n );
            }
            
            break;   
            
      case  nTypeTermInteger :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%lld]",(void*)n,L"term integer"  ,n->term.integer );
                $astDebugRowColToken(fDebug);   
            }          
            
            break;
            
       case  nTypeTermReal :

            if ( this->fDebug ) 
            {    
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%lf]",(void*)n,L"term real" ,n->term.real );
                $astDebugRowColToken(fDebug);
            }

            break;  

      case  nTypeTermChar :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%lc]",(void*)n,L"term char"  ,g.outputSpecialCharInChar(n->term.wchar) );
                $astDebugRowColToken(fDebug);
            }             
            
            break;
            
      case  nTypeTermString :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%ls]",(void*)n,L"term string"  ,g.outputSpecialCharInString(n->term.wstring) );
                $astDebugRowColToken(fDebug);
            }             
            
            break;

      case  nTypeTermVar :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%ls]",(void*)n,L"term var"  ,n->termVar.id );
                $astDebugRowColToken(fDebug);
            }             
            
            break;

      case  nTypeTermField :

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%ls]",(void*)n,L"ter field"  ,n->termField.id );
                $astDebugRowColToken(fDebug);
            }             
            
            break;
            
        case  nTypeBinOp :

            astNodeDebug( this,n->binOp.right ) ;
            astNodeDebug( this,n->binOp.left  ) ;

             if ( this->fDebug ) 
             {
                 fwprintf ( 
                    this->pFileOutputNode ,L"node [%018p] %-10ls :: Left[%018p] Right[%018p] :: sym [%d]",(void*)n,L"BinOp" 
                    ,(void*)n->binOp.left 
                    ,(void*)n->binOp.right
                    ,n->binOp.sym 
                );
                $astDebugRowColToken(fDebug);
            }

            break;             
 
        case  nTypePrefix :

            astNodeDebug( this,n->prefix.right ) ;

            if ( this->fDebug ) 
            {
                fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls :: [%d]",(void*)n,L"prefix" ,n->prefix.sym );
                $astDebugRowColToken(fDebug);
            }

            break;  

        case  nTypeBlock :
    
            if ( this->fDebug ) fwprintf ( this->pFileOutputNode , L"node [%018p] %-10ls\n\n",(void*)n,L"block");
            
            for (size_t i = 0 ; i<n->block.next.size; i++)
            {
                if ( this->fDebug ) 
                {    
                    if ( n->block.next.data[i] != NULL )
                    {
                        fwprintf ( this->pFileOutputNode , L"node [%018p] -> [%03d/%03d]::[%018p]\n",n,(int)i,(int)n->block.next.size-1,n->block.next.data[i] );
                    }
                }
                if ( n->block.next.data[i] != NULL ) 
                {
                    astNodeDebug( this,n->block.next.data[i] ) ; 
                }
                fwprintf ( this->pFileOutputNode , L"\n"); // \n per meglio visualizzare i blocchi
            }

            break; 

        case nTypeAssign :

            astNodeDebug( this,n->assign.lhs ) ;
            
            astNodeDebug( this,n->assign.rhs ) ;
    
            if ( this->fDebug ) 
            {
                fwprintf 
                    ( 
                        this->pFileOutputNode , L"node [%018p] %-10ls :: lhs [%018p] rhs [%018p]"
                        ,(void*)n
                        ,L"nTypeAssign" 
                        ,(void*)n->assign.lhs
                        ,(void*)n->assign.rhs  
                    );
                $astDebugRowColToken(fDebug);
            }
        
        break ;  
    
        case nTypeDeclConst :
        
            astNodeDebug( this , n->declConst.term ) ;
            
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls] sym[%d] term[%018p] scope[%03d]\n"
                        ,(void*)n
                        ,L"DeclConst" 
                        ,n->declConst.id
                        ,n->declConst.sym
                        ,n->declConst.term
                        ,n->declConst.scope  
                    );
            }
        
        break ; 
 
        case nTypeDeclVar :
        
            astNodeDebug( this , n->declVar.expr ) ;
            
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls] sym[%d] expr[%018p] scope[%03d]\n"
                        ,(void*)n
                        ,L"DeclVar" 
                        ,n->declVar.id
                        ,n->declVar.sym
                        ,n->declVar.expr
                        ,n->declVar.scope  
                    );
            }
        
        break ; 

        case nTypeArrayDim :
        
            for (size_t i = 0 ; i< vectorSize ( n->arrayDim.ndx ) ; i++)
            {
                astNodeDebug(  this , n->arrayDim.ndx.data[i] ) ;
            }
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: dim[%d]\n"
                        ,(void*)n
                        ,L"ArrayDim"
                        ,(int)vectorSize ( n->arrayDim.ndx )
                    );
            }

        break ;

        case nTypeDeclArray :
        
            astNodeDebug( this , n->declArray.dim ) ;

            astNodeDebug( this , n->declArray.il ) ;
                        
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls]  : dim[%018p] sym[%d] il[%018p] scope[%03d]\n"
                        ,(void*)n
                        ,L"DeclArray" 
                        ,n->declArray.id
                        ,n->declArray.dim
                        ,n->declArray.sym
                        ,n->declArray.il
                        ,n->declArray.scope  
                    );
            }
        
        break ; 

        case nTypeDeclType :
        
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls]  : scope[%03d]\n"
                        ,(void*)n
                        ,L"DeclType" 
                        ,n->declArray.id
                        ,n->declArray.scope  
                    );
            } 
               
            fwprintf ( this->pFileOutputNode , L"\n{\n" )  ;   
               
            for ( uint32_t i = 0 ; i < vectorSize ( n->declType.field ) ; i++ )
            {
                astNodeDebug( this , n->declType.field.data[i]  )  ;
            }
            
            fwprintf ( this->pFileOutputNode , L"}\n" )  ;
        
        break ; 
 
        case nTypeDeclFunction :

            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls]  ( %018p ) -> [%03d] { %018p } "
                        ,(void*)n
                        ,L"DeclFunction" 
                        ,n->declFunction.id
                        ,&n->declFunction.param
                        ,n->declFunction.retType
                        ,n->declFunction.blockCode
                    );
            } 
  
            fwprintf ( this->pFileOutputNode , L"\n(\n" )  ;   
               
            for ( uint32_t i = 0 ; i < vectorSize ( n->declFunction.param ) ; i++ )
            {
                astNodeDebug( this , n->declFunction.param.data[i]  )  ;
            }
            
            fwprintf ( this->pFileOutputNode , L")\n" )  ;
            
             fwprintf ( this->pFileOutputNode , L"{\n" )  ;   
               
            astNodeDebug( this , n->declFunction.blockCode  )  ;

            fwprintf ( this->pFileOutputNode , L"}\n"   );   
            
        break ;  

        case nTypeTermArray :
        
            astNodeDebug( this , n->termArray.dim  )  ;
            
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls]  : dim[%03d]\n"
                        ,(void*)n
                        ,L"nTypeTermArray" 
                        ,n->termArray.id
                        ,vectorSize ( n->termArray.dim->arrayDim.ndx  )   
                    );
            } 
        
            break ;

        case nTypeTermFunction :
        
            if ( this->fDebug )
            { 
                fwprintf 
                    ( this->pFileOutputNode , L"node [%018p] %-10ls :: id[%ls]  : param[%03d]\n"
                        ,(void*)n
                        ,L"nTypeTermFunction" 
                        ,n->termFunction.id
                        ,vectorSize ( n->termFunction.param->arrayDim.ndx  )   
                    );
            } 
            fwprintf ( this->pFileOutputNode , L"\n(\n");
            
            astNodeDebug( this , n->termFunction.param  )  ;  
             
            fwprintf ( this->pFileOutputNode , L")\n");  
               
            break ;
 
      default :

            $nodeInternal ( debug , errUnknown , L"ast.c" , L"node_t* astDebug(node_t* n) -> switch ( n->type )") ;
            return NULL ;

      break;
  }
  
  //--nodeLevel;
  
  return NULL ;
}

void astDebug( past_t this , pnode_t n) 
{
    if ( this->fDebug ) 
    {
        fwprintf ( this->pFileOutputNode,L"\n# node   -> debugging ...\n\n"  );

        if ( n != NULL )
        {
            astNodeDebug(this,n);
        }
        else
        {
            fwprintf ( this->pFileOutputNode,L"\n# node   -> {null}...\n"  );
        }
    }
}

// ......................................................... ast alloc

past_t  astAlloc ( void )
{
    past_t past = gcMalloc( sizeof(ast_t) ) ;
    
    if ( past==NULL )
    {
        $astInternal( malloc , outOfMemory , NULL , NULL ) ;
    }
    else // init default parameter
    {
        past->fDebug              = 0         ;
        past->pFileOutputAST      = NULL      ;  
        past->fileNameOutputAST   = NULL      ; 
        past->pFileOutputNode     = NULL      ;  
        past->fileNameOutputNode  = NULL      ; 
    }
    return past ;
}

// ......................................................... ast dealloc

void      astDealloc ( past_t this )
{
    if ( this!=NULL )
    {
        gcFree(this);
    }
    else
    {
        $astInternal( dealloc , errUnknown , NULL , NULL );
    }
}

// ......................................................... ast constructor

void astCtor( past_t this , char* fileInputName )
{
    // file debug parser
    
    if ( this->fDebug ) 
    {
        if ( fileInputName!=NULL )
        {

            // .ast 
            
            this->fileNameOutputAST  = g.makeFileWithNewExt( fileInputName , ".ast"  ) ;
            stdFileWOpen ( &this->pFileOutputAST , this->fileNameOutputAST , "w+","ccs=UTF-8" ) ;
            
            if ( this->pFileOutputAST != NULL )
            {
                fwprintf ( this->pFileOutputAST , L"\n%-20ls : [%018p] -> [%-20hs]\n" 
                    ,L"file ast"        
                    ,this->pFileOutputAST 
                    ,this->fileNameOutputAST
                ) ;
            }
            
            // .node
            
            this->fileNameOutputNode  = g.makeFileWithNewExt( fileInputName , ".node"  ) ;
            stdFileWOpen ( &this->pFileOutputNode , this->fileNameOutputNode , "w+","ccs=UTF-8" ) ;
            
            if ( this->pFileOutputNode != NULL ) // \n\n per meglio visualizzare i blocchi
            {
                fwprintf ( this->pFileOutputNode , L"\n%-20ls : [%018p] -> [%-20hs]\n\n" 
                    ,L"file node"   
                    ,this->pFileOutputNode 
                    ,this->fileNameOutputNode
                ) ;
            }

        }
        else
        {
            $astInternal( checkFileExists , noInputFiles , L"{null}" , NULL );
        }
    }
}

// ......................................................... ast destructor

void astDtor ( past_t this )
{
    if (this->fDebug)     
    {
        fclose(this->pFileOutputAST);
        fclose(this->pFileOutputNode);
    } ;
}

#undef $astDebugRowColToken



/**/



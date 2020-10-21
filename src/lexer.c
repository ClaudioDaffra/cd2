#include "global.h"
#include "error.h"
#include "lexer.h"

#include <stdint.h>
#include <math.h> // HUGE...

#ifdef __linux__
#include <wctype.h>
#endif

#ifdef __APPLE__
#include <wctype.h>
#endif

// n.b.
//
// il calcolo riga e la colonna viene effettuato con row,col+1, per tanto occorrerà visualizzare
// il risultato con row,col-1 e passarlo così  anche al parser 


// var global main.c

//extern struct global_s g ;

// mappa parole chiave

mapKW_t mapArrayKW[] = 
{
    {   L"while"    , sym_kw_while		}    ,	//	+1
    {   L"for"      , sym_kw_for		}    , 	//	+2
    {   L"const"    , sym_kw_const		}    ,  //	+3 
    {   L"var"    	, sym_kw_var		}    , 	//	+4
    {   L"integer"  , sym_kw_integer	}    ,	//	+5 
    {   L"real"    	, sym_kw_real		}    ,	//	+6  
    {   L"char"   	, sym_kw_char		}    ,	//	+7  
    {   L"byte"   	, sym_kw_byte		}    ,	//	+8        
    {   L"array"   	, sym_kw_array		}    ,	//	+9   
    {   L"type"   	, sym_kw_type		}    ,	//	+10   
             
    {   NULL        , 0            		}    ,
} ;

//*********
//  LEXER   
//*********

// ......................................................... lexer body

int lexerInclude( plexer_t this , char * fileInputName )
{
	FILE* fi ;  
	errno_t err=0 ;

	// #0 file debug lexer
	
	if ( this->fDebug ) 
	{
		this->fileNameOutputLexer  = g.makeFileWithNewExt( fileInputName , ".lexer"  ) ;
		
		stdFileWOpen ( &this->pFileOutputLexer , this->fileNameOutputLexer , "w+","ccs=UTF-8" ) ;
		
		if ( this->pFileOutputLexer != NULL )
		{
			fwprintf ( this->pFileOutputLexer , L"\n%-20ls : [%018p] -> [%-20hs]\n" 
				,L"file lexer"	    
				,this->pFileOutputLexer 
				,this->fileNameOutputLexer 
			) ;
		}
	}
    
    // #1 salva le informazioni del lexer attuale in cima allo stack

    lexerBuffer_t tempLexBuffer = gcMalloc ( sizeof(struct lexerBuffer_s) ) ;
    
    tempLexBuffer->row                = this->row             ;  
    tempLexBuffer->col                = this->col             ;    
    tempLexBuffer->fileInputName      = this->fileInputName   ;
    tempLexBuffer->pfileInput         = this->pfileInput      ; 
	
	stackPush ( this->sLexBuffer , tempLexBuffer ) ;

    // #2 #include "..." 

    if (fileInputName==NULL) 
    {
        $lexer ( fatal,checkFileExists,fileNotFound,tempLexBuffer->row,tempLexBuffer->col,L"{?}",L"lexer include" );
        this->pfileInput=NULL;        
        return err=1;
    }

    // #3 open file 
	
    err = cdFileWOpen ( &fi , fileInputName , "r" , "ccs=UTF-8" ) ; 

    if (err)
    { 
        wchar_t buffer[16] ;
        #if defined(__MINGW32__) || defined(__MINGW64__)
        snwprintf ( buffer,16,L"ERR_T <%d>",(uint32_t)err) ;
        #else
        swprintf ( buffer,16,L" ERR_T <%d>",(uint32_t)err) ;
        #endif
        $lexer ( error,checkFileExists,fileNotFound,tempLexBuffer->row,tempLexBuffer->col,$2WS(fileInputName),buffer );
        this->pfileInput=NULL;        
        return err;
    }

    // #4 parti col nuovo buffer
    
    this->row               =   1 ;
    this->col               =   0 ;
    this->fileInputName     =   $2WS(fileInputName) ;    
    this->pfileInput        =   fi ;

	return 0 ;
}

// ......................................................... lexer get char

wchar_t lexerGetChar( plexer_t this )
{
	if ( this->pfileInput==NULL ) return _WEOF ; 

    // save for $prev
    
    this->old_col = this->col ;
    this->old_row = this->row ;    
    // character 
    this->c0 = fgetwc ( this->pfileInput  ) ;
 
    // salta gestici \r\n windows
    if ( this->c0==L'\r' ) 
    {
         this->c0 = fgetwc ( this->pfileInput  ) ;
    }
   
    //  se il carattere è \n resetta row,col e non incrementare col
    if ( this->c0==L'\n' ) 
    {
		this->row++;          
		this->col=0;
    }
    else
    {
		// è un qualsiasi altro carattere allora incrementa 
		// dato che parte da '0' e 'col' è uint32_t 
		// quado makeToken andrà decrementato di 1
		
		this->col++ ;

		if ( this->c0 == L'\t' ) 
		{
			uint32_t tempCol = this->col ;
			uint8_t  modCol  = ( tempCol + this->tabSize ) % this->tabSize ;
			this->col = this->col + ( this->tabSize - modCol ) ;
		}
	}
	
    // lookahead
    
    this->c1 = fgetwc ( this->pfileInput  ) ;
    ungetwc ( this->c1 , this->pfileInput ) ;
 

    return this->c0  ;
}

// ......................................................... lexerPrintInfoChar

int lexerPrintInfoChar( plexer_t this )
{
    wchar_t C = this->c0;

	switch ( C ) 
	{
		case L'\n' 	: C = L'N'; break ;
		case L'\t' 	: C = L'T'; break ;
		case L'\v' 	: C = L'V'; break ;
		case L'\f' 	: C = L'F'; break ;
		default		: break;
	}
	    
    fwprintf ( this->pFileOutputLexer , L"\n[%-20ls][%03d,%03d](%6d)[%lc]"
		,	this->fileInputName	
		,	this->row_start		
        ,	this->col_start-1	
        ,	(uint32_t)this->c0	
        ,	C 
    ) ;
        
    return 0 ;
}


int lexerPrintToken( plexer_t this )
{
    if ( this->sym == sym_weof ) return 0; // non viene visualizzato, viene ritornato solo END
    if ( this->sym == sym_end  ) return 0; // non viene visualizzato ( this->fileInputName NULL )

    // better format

    fwprintf ( this->pFileOutputLexer,L"-" ) ; // migliora visualizzazione con output : stdout > file.txt

    if ( wcslen(this->fileInputName) > 20 )
        fwprintf ( this->pFileOutputLexer,L"[%-17.17ls...]" ,this->fileInputName ) ;
    else
        fwprintf ( this->pFileOutputLexer,L"[%-20.20ls]"    ,this->fileInputName ) ;
    
    fwprintf ( this->pFileOutputLexer , L"[%03d,%03d] len(%02d) sym(%03d) "
		,	this->row_start
		,	this->col_start-1
		,	(int)this->tokenSize
		,	(uint32_t)this->sym
    ) ;    

    if ( wcslen(this->token) > 20 )
        fwprintf ( this->pFileOutputLexer,L"[%.17ls...]" ,this->token ) ;
    else
        fwprintf ( this->pFileOutputLexer,L"[%20ls]" ,this->token ) ;

#if defined(_MSC_VER)
    if ( this->sym==sym_integer )  fwprintf ( this->pFileOutputLexer , L" -> [[%lld]]",this->value.integer  ) ;
#else
    if ( this->sym==sym_integer )  fwprintf ( this->pFileOutputLexer , L" -> [[%ld]]",this->value.integer   ) ;
#endif
    if ( this->sym==sym_real    )  fwprintf ( this->pFileOutputLexer , L" -> [[%lf]]",this->value.real      ) ; 
 
    if ( this->sym==sym_char    )  fwprintf ( this->pFileOutputLexer , L" -> [[%lc]]",g.outputSpecialCharInChar(this->value.wchar)     ) ;
    
    if ( this->sym==sym_string  )  fwprintf ( this->pFileOutputLexer , L" -> [[%ls]]",g.outputSpecialCharInString(this->value.wstring)	) ;
        
    fwprintf ( this->pFileOutputLexer , L"\n" ) ;
    
    return 0 ;
}

// ......................................................... lexerSkipWhite

int lexerSkipWhite( plexer_t this )
{
	// se il prossimo carattere non è un blank
	// continua con for(;;) e ottieni $c0
    while ( 
			iswblank(this->c1)		// \t blank
        || 	this->c1==L'\u2003'		// Em Space UNICODE character 
        || 	iswcntrl(this->c1)   	// whitespaces (000-01f 07f)     
    )
    {
        lexerGetChar(this) ;
    }

    return 0 ;
}

// ......................................................... isID

int lexerIsID( plexer_t this , wchar_t c )
{
	(void)this;
    if ( iswalnum(c)  ) return 1 ;
    if ( c==L'$'      ) return 1 ;
    if ( c==L'_'      ) return 1 ;

    return 0 ;
}

// ......................................................... getToken

int lexerPushToken( plexer_t this , wchar_t c )
{
    // se la lunghexa del TEXT eccede il buffer
    // di immagazzinamento del TOKEN
    // predisponi il termine del lexer [sym_end] 
    // ritorna 0 ; indica uscita dal lexer e da tutti i file stack.
    if ( (this->tokenSize+1) >= maxTokenSize )
    {  
       $lexerInternal( pushToken,overflowTokenText,this->fileInputName,NULL ) ;
       this->sym = sym_end;
       return 0 ;
    }
    this->token[this->tokenSize++] = c ;

    return 1 ;
}

// ......................................................... makeToken

int lexerMakeToken( plexer_t this , sym_t sym )
{
    this->token[this->tokenSize]    = L'\0'  ;

    this->sym                       = sym    ;

    return 1 ;
}

// ......................................................... get const number 

// $pushToken($c0) : se length text > text size return 0 in push token -> RETURN 0 ;

int lexerGetConstNumber( plexer_t this , int base )
{
    int fConst=0;
    
    do {
            switch ( base ) // 0xffff:ffff 1'000.000
            {
                case 16 :    
                case  2 :   if ( $c0 == L':' ) { $next; continue ; }
                            break ; 
                
                case 10 :  
                case  8 :   if ( $c0 == L'\'' ) { $next; continue ; }
                            break ; 

                default: break ;
            }
            fConst=0;
            switch ( base ) 
            {
                case 16 :   if ( iswxdigit($c0) ) fConst=1;         break ;
                case 10 :   if ( iswdigit ($c0) ) fConst=1;         break ;
                case  8 :   if ( $c0>=L'0' &&  $c0<=L'7') fConst=1; break ; 
                case  2 :   if ( $c0>=L'0' &&  $c0<=L'1') fConst=1; break ;
                default: break ;
            }
            if ( fConst )   
            { 
                $pushToken($c0)
                $next 
            } 
            else            
            { 
                break; 
            } ;
    } while(1) ;
    
    return 1;
}

sym_t lexerGetConst( plexer_t this , int base )
{
    sym_t sym = sym_integer ;

    if ( ! lexerGetConstNumber(this,base) ) return 0 ;

    if ( ( base == 2 || base == 8 ) && (  $c0=='.') ) $lexerError( scanNumber,invalid_argument ) ;

    if ( base != 2 && base != 8 ) // no virgola per binari e ottali
    {
        //..................................... [.]
        if ( $c0=='.' )
        {
            sym=sym_real ;
            $pushToken($c0) 
            $next ; // .
            if ( ! lexerGetConstNumber(this,base) ) return 0 ;
        }

        //..................................... [p/e]
        if (
            ( base==16 && towlower($c0)=='p' ) ||
            ( base==10 && towlower($c0)=='e' )
         ) {
            $pushToken(towlower($c0))
            $next ; // .
            if (  $c0=='+' ||  $c0=='-'  )
            {
         //..................................... [+/-]       
            $pushToken($c0)
            $next ; // +-
            }   
         //..................................... check digit after E/e
            if ( ! ( $c0>=L'0' && $c0>=L'0' ) ) $lexerError( scanNumber,expectedExponentDigit ) ;
        }


        if ( ! lexerGetConstNumber(this,10) ) {  return  0 ;}
    }    
   
    if ( $c0!=L'\n') 
    {

		$prev
	}

    lexerMakeToken( this,sym ) ;

    // converti 
    
    wchar_t* ps=&this->token[0] ;
    wchar_t* end=NULL; ;
    switch ( base ) 
    {
        case 16 :
            if ( sym == sym_integer ) { this->value.integer=wcstol(ps,&end,16) ;} ; 
            break ;
        case  2 :
            if ( sym == sym_integer ) { this->value.integer=wcstol(ps+2,&end,2) ;} ;
            break ;
        case 10 : 
            if (
                ( wcschr ( this->token , L'e' ) != NULL ) &&
                ( wcschr ( this->token , L'.' ) == NULL )
            )
            {
                this->value.integer=(uint64_t)wcstod(ps,&end) ; // fix 1e3
                break ;
            }
            if ( sym == sym_integer ) { this->value.integer=wcstol(ps,&end,10) ;} ; 
            if ( sym == sym_real    ) { this->value.real=wcstod(ps,&end) ; } ;
            break ;
        case  8 : 
            if ( sym == sym_integer ) { this->value.integer=wcstol(ps,&end,8) ;} ;
            break ;       
        default: 
            break ;
    } 
    // HUGE_VAL 34
    if ( errno == 34    )   $lexerError( convertToNumber,out_of_range ) ;
    if ( wcslen(end)!=0 )   $lexerError( convertToNumber,invalid_argument ) ; // ex 19e ( da errore )

    return sym ;
}

// ......................................................... get character

wchar_t lexerGetCharacter( plexer_t this )
{
	wchar_t C=0;
	
	if ( $c0 == L'\\') 
	{
		$pushToken($c0)
		$next ;
		switch ( $c0 )
		{
			case L'n' 	: $pushToken($c0) ; C = '\n' ; break;
			case L't' 	: $pushToken($c0) ; C = '\t' ; break;
			case L'v' 	: $pushToken($c0) ; C = '\v' ; break;
			case L'f' 	: $pushToken($c0) ; C = '\f' ; break;
			case L'"' 	: $pushToken($c0) ; C = '\"' ; break;
			case L'\''	: $pushToken($c0) ; C = '\'' ; break;
			case L'\\'	: $pushToken($c0) ; C = '\\' ; break;
			default :
			{
				wchar_t strErrTemp[2];
				strErrTemp[0] = $c0 ;
				strErrTemp[1] = 0 ;

				$lexerErrorExtra( tokenizing , unexpectedToken , strErrTemp ) ;
				break ;
			}
		}
	}
	else
	{
		C = $c0 ;
		$pushToken($c0)
	}

	return C ;
}

//***********
// Lexer Scan
//***********

int lexerScan( plexer_t this )
{
    // SHOW : lexer scanning

    if ( this->flexerScan==0 ) 
    {
        this->flexerScan=1;
        if ( this->fDebug ) fwprintf ( this->pFileOutputLexer,L"\n# Lexer  -> scanning ...\n\n"  );
    }

    // l'ordine di uscita arriva da 'return 0' && (sym_end ) ,
    // diversamente 'return 1' && ( sym_weof ) informerà che ci
    // sono ancora file da scannerizzare.

	for(;;) // scan
	{
        this->tokenSize=0; // reinizia con un nuovo token

		// ....................................... check file include
		
		if ( this->pfileInput==NULL ) 
		{
			this->sym = sym_end    ;
			return 0 ;
		}
		
		// ....................................... get char  

		lexerGetChar(this) ;

		// ....................................... buffer terminato 
		
		if ( $c0==_WEOF ) 
		{
			this->sym 		= sym_weof  ;
			this->token[0] 	= 0 		;
			this->tokenSize = 0 		;
			break ;
		}
		
		// ....................................... get start row , col  
		
		this->row_start = this->row ;
		this->col_start = this->col ; 
/*
        // ....................................... DEMO #include lexer
        
		if (  this->c0==L'X' )
		{
		   lexerInclude( "test/b.txt" ) ;
		   continue ;
		}
        if (  this->c0==L'Y' )
        {
           lexerInclude( "test/c.txt" ) ;
           continue ;
        }
*/
		//......................... skip blank

		if ( iswblank($c0) || iswcntrl(this->c0) || this->c0==L'\u2003' )
		{
			lexerSkipWhite(this); 

			continue ;
		}
        
		// ........................ skip remarks

		if ( $c0==L'*' && $c1==L'/' )   // ................... * /
		{
			$lexerWarning( scanComment,eoRem ) ;
			$next;
			$next;                                 
			continue ;
		}
		if ( $c0==L'/' && $c1==L'/' )   // ................... //
		{
			while ( $c1!=L'\n' )
			{ 
				if ( $c1 == _WEOF )
				{
					$lexerWarning( scanComment,eof ) ;
					break ;
				}
				$next;
			}
			continue ;
		}

		if ( $c0==L'/' && $c1==L'*' )   // ................... / * ... * /
		{
			$next;
			$next;

			while ( 1 )
			{
				if ( $c0==L'*' && $c1==L'/' ) { $next ; break ;}
				
				if ( ( $c0==_WEOF ) || ( $c1==_WEOF ) )
				{
					$lexerError( scanComment,eof ) ;
					return 0 ;
				}
				$next;                       
			}
			 
			continue ;
		}

        // ## ....................................... CONSTANT
 
        // HEX hexadecimal
        if ( $c0 == L'0' &&  towlower($c1) == L'x' )  
        {
            $pushToken($c0) 
            $next ; // 0
            $pushToken($c0) 
            $next ; // x          
     
            if ( ! lexerGetConst( this, 16 ) ) return 0;
            return 1 ;
        }
        // OCT octal
        if ( $c0 == L'0' &&  $c1 == L'b' )  
        {
            $pushToken($c0) 
            $next ; // 0
            $pushToken($c0) 
            $next ; // 7          
     
            if ( ! lexerGetConst( this, 2 ) ) return 0;
            return 1 ;
        }
        // BIN binary
        if ( $c0 == L'0' &&  ( $c1>=L'0' &&  $c1<=L'7') )  
        {
            $pushToken($c0) 
            $next ; // 0
            $pushToken($c0) 
            $next ; // 7          
     
            if ( ! lexerGetConst( this, 8 ) ) return 0;
            return 1 ;
        }

        // INTEGER / REAL
        if ( 
            ( iswdigit($c0) && $c1==L'\'' ) || // integer 1'000 or 1e3
            ( iswdigit($c0) && $c1==L'e' )
        )            
        {
            $pushToken($c0) 
            if ( towlower($c1)!='e') $next ; // consume '
            $next ;
            if ( ! lexerGetConst( this, 10 ) ) return 0;
            return 1 ;
        }         
        if ( $c0 == L'0' &&  $c1>=L'.' ) // real 0.
        {
            $pushToken($c0) 
            $next ; // 0
            if ( ! lexerGetConst( this, 10 ) ) return 0;
            return 1 ;
        }
        if ( $c0 == L'.' &&  iswdigit($c1) ) // real .0
        {
            if ( ! lexerGetConst( this, 10 ) ) return 0;
            return 1 ;
        } 
        if ( iswdigit($c0) ) // 1
        {
            if ( ! lexerGetConst( this, 10 ) ) return 0;
            return 1 ;
        }
      
        // ## .......................................  ID / KW

        if ( lexerIsID(this,$c0) && (!iswdigit($c0)) )  
        {
            while ( lexerIsID(this,$c1) )  
            {
                $pushToken($c0)
                $next; // lexerGetChar();
            }
            $pushToken($c0)
            
            // check kw or id
            
            lexerMakeToken( this,sym_id ) ;   
            
            whmapType(temp) ;
            temp =  whmapFind( this->mapKW, this->token ) ;
            // se trova la keyword, ringenera il token 
            // col simbolo trovato ( da 100 ... per le KW ) 
            if ( temp != NULL ) lexerMakeToken( this, *(sym_t*) temp ) ;
  
			this->value.id = gcWcsDup( this->token ) ;
			
            return 1 ;
        }
        
        // ## .......................................  CHAR
        
        if ( $c0 == L'\'' && $c1 == L'\'' )
        {
			$pushToken($c0); 
			$next;	// '
			$pushToken($c0); 
			$next;	// '
			if ( $c0 != L'\'' ) $lexerErrorExtra( tokenizing , invalid_argument , L"''" ) ;
			return 1 ;
		}
		
        if ( $c0 == L'\'' )
        {
			$pushToken($c0); 
			$next;	// '
			
			this->value.wchar = lexerGetCharacter(this);
			
			$next;	// '
			$pushToken($c0);
			
			if ( $c0 != L'\'' ) $lexerError( tokenizing , unexpectedToken ) ;

			lexerMakeToken( this, sym_char ) ;

			return 1 ;
		}
        
        // ## .......................................  STRING  

        if ( $c0 == L'\"' && $c1 == L'\"' )
        {
			$pushToken($c0);	// "
			 
			$next;				// "
			$pushToken($c0); 
			
			// here	// " character after ""
			
			this->value.wstring = gcWcsDup(L"\0") ; // to fix wcslen ( NULL )
			lexerMakeToken( this, sym_string ) ;

			return 1 ;
		}
		             
        if ( $c0 == L'\"' ) // L'"'
        {
			const int 	maxBuffer 	= maxTokenSize 	;
			int 		kBuffer		=	 0	;
			wchar_t buffer[maxTokenSize];
			
			$pushToken($c0); 
			$next;	// "
			do {
				this->value.wchar = lexerGetCharacter(this);
				
				if ($c0==L'\n')
				{
					$lexerErrorExtra( tokenizing , unexpectedToken , L"\\n") ;
					wcscpy( this->token , L"\\n"  ) ;
					this->value.wstring = gcWcsDup(L"\\n") ;
					lexerMakeToken( this, sym_string ) ;
					return 1 ;
				}
				
				if ( kBuffer<maxBuffer ) 
					buffer[kBuffer++] = this->value.wchar ;
				else
					$lexerInternal( tokenizing , overflowTokenText , L"[ lexer.c ] : string " , L"( kBuffer<maxBuffer )") ;
					
				$next

				if ( kError ) // kludge
				{
					$lexerInternal( tokenizing , unexpectedToken , L"[ lexer.c ] : string " , L"( token )") ;
					return 0 ;
				}
			} while ( $c0 != L'"' && $c0!=_WEOF ) ;
			buffer[kBuffer] = 0 ;

			this->value.wstring = gcWcsDup(buffer) ;
			$pushToken($c0)		; // "
			$pushToken(L'\0')	; // 0
			
			if ( this->token[0] == L'"' )
			{
				if (  this->token[wcslen(this->token)-1]  != L'"' ) // EOF ? End Of String
				{
					wcscpy( this->token , L"<<EOS>>"  ) ;
					this->value.wstring = gcWcsDup(L"<<EOS>>") ;
					$lexerErrorExtra( tokenizing , invalid_argument , L" string no close (\") .") ;
				} 
			}
			
			lexerMakeToken( this, sym_string ) ;

			return 1 ;
		}
		        
        // ## ....................................... OPERATOR2
        
        if ( ($c0 == L':') && ($c1 == L'=') ) // :=
        {
			$pushToken($c0) ; $next; 	// :
			$pushToken($c0) ;			// =
			lexerMakeToken( this, sym_assign ) ;
			
			return 1 ;
		}
        
        // ## ....................................... OPERATOR1

        int     fOp=1;
        sym_t   symOp=sym_end ;
        switch ( $c0 )
        {
            case L'+' : symOp=sym_add ; break; 
            case L'-' : symOp=sym_sub ; break;
            case L'*' : symOp=sym_mul ; break; 
            case L'/' : symOp=sym_div ; break;
            case L'%' : symOp=sym_mod ; break; 
            case L'(' : symOp=sym_p0  ; break;
            case L')' : symOp=sym_p1  ; break; 
            case L'!' : symOp=sym_not ; break; 
            case L';' : symOp=sym_pv  ; break; 
            case L'=' : symOp=sym_eq  ; break; 
            case L',' : symOp=sym_v   ; break;  
            case L':' : symOp=sym_dp  ; break;
            case L'[' : symOp=sym_pq0 ; break;
            case L']' : symOp=sym_pq1 ; break;  
            case L'{' : symOp=sym_pg0 ; break;
            case L'}' : symOp=sym_pg1 ; break;                                                                            
            // se non trovi opertore allora è 1 carattere
            default   : fOp=0;          break; 
        } ;

        if ( fOp )
        {
            fOp=0;
           
            $pushToken($c0);
            lexerMakeToken( this,symOp ) ;
            return 1 ; 
        } ;

        // ## .......................................  token lexema
        
        $pushToken($c0)
        
        lexerMakeToken( this, sym_lexema ) ;

		$lexerErrorExtra( tokenizing , unexpectedToken , this->token ) ;
		
		//

		return 1 ;
        
	} ;  

	//**********
	// SCAN END
	//**********
        
    // se le dimensioni dello stack sono uguali a 1 allora rimane 1 solo file quindi esci
    // altrimenti avendo incontrato WEOF
    // esegui un pop per il file precedente
    // ripristina i valori e torna a scannerizzare il file
    // l'ultimo WEOF determina END lexer -> sym_end ;

    // chiudi il file sullo stack
    fclose( this->pfileInput ) ; // 1234 to check
    
    if (stackSize(this->sLexBuffer)>=1)
    {
        this->row               =   stackTop(this->sLexBuffer)->row            ;
        this->col               =   stackTop(this->sLexBuffer)->col            ;


        if ( stackTop(this->sLexBuffer)->fileInputName != NULL ) // KLUDGE se ultimo carattere : il nome del file e' NULL
        this->fileInputName     =   stackTop(this->sLexBuffer)->fileInputName  ;

        this->pfileInput        =   stackTop(this->sLexBuffer)->pfileInput     ;

        // ritorna a leggere il vecchio file  

        stackPop(this->sLexBuffer) ; 
 
        // continua col carattere nuovo        
        if (stackSize(this->sLexBuffer)>=1) lexerScan(this);

        return 1 ;
    }

    this->sym = sym_end ;
	
    return 0 ;
}

// ......................................................... lexerGetToken

sym_t lexerGetToken ( plexer_t this )
{
    for(;;) // lexer
    {
        lexerScan(this);
        // salta WEOF dei file di inclusione
        if ( this->sym == sym_weof ) continue ;

        break ;
    } ;
   
   if ( this->fDebug ) lexerPrintToken(this);

   return this->sym ;
}

// ......................................................... lexer alloc

plexer_t  	lexerAlloc			( void )	
{
	plexer_t pLexer = gcMalloc( sizeof(lexer_t) ) ;
	
	if ( pLexer==NULL )
	{
		$lexerInternal( malloc , outOfMemory , NULL , NULL ) ;
	}
	else // init default parameter
	{
		pLexer->fDebug 				= 0 		;
		pLexer->flexerScan			= 0 		;
		pLexer->tabSize  			= 4 		;      
		pLexer->pFileOutputLexer 	= NULL   	;  
		pLexer->fileNameOutputLexer = NULL	    ; 
		pLexer->row 				= 0 		;
		pLexer->col 				= 0 		;
		pLexer->old_col 			= 0 		;
		pLexer->old_row 			= 0 		;
		pLexer->fileInputName 		= NULL		;
		pLexer->pfileInput 			= NULL 		;
		pLexer->c0 					= 0 		;
		pLexer->c1 					= 0 		;
		pLexer->row_start 			= 0 		;
		pLexer->col_start 			= 0 		;
		pLexer->sym 				= sym_end 	;
		pLexer->token[0] 			= 0			;
		pLexer->tokenSize			= 0			;
		pLexer->value.integer 		= 0 		; // union
	}

	return pLexer ;
}

// ......................................................... lexer dealloc

void  	lexerDealloc			( plexer_t this )	
{
	if ( this!=NULL )
	{
		gcFree(this);
	}
	else
	{
		$lexerInternal( dealloc , errUnknown , NULL , NULL );
	}
}

// ......................................................... lexer constructor

void lexerCtor( plexer_t this )
{
	// init stack
	
	stackNew(this->sLexBuffer,128);

	// init keyword map

	// GNU GCC 9.2 : lib/hmap.c:299:39: warning: 
	// ISO C forbids conversion of object pointer to function pointer type [-Wpedantic] C11
	#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wpedantic"
	#endif
	this->mapKW = whmapDef ( gcComparepWStrC );
	int count=0;
	while ( mapArrayKW[count].kw != NULL )
	{
	   // wprintf ( L"\n ## %ls %d.",mapArrayKW[count].kw,mapArrayKW[count].sym );
		whmapInsert( 
			this->mapKW,  
			(void*)gcWcsDup( mapArrayKW[count].kw  ) , 
			(void*)gcIntDup( mapArrayKW[count].sym )   
		);
		++count;
	}
	#ifdef __GNUC__
	#pragma GCC diagnostic pop
	#endif
	
}

// ......................................................... lexer destructor

void lexerDtor( plexer_t this )
{
    stackFree ( this->sLexBuffer ) ;

	whmapDelete ( this->mapKW ) ;
	
	if (this->fDebug) 	if ( this->pFileOutputLexer ) fclose(this->pFileOutputLexer);

}


/**/



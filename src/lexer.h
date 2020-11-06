#ifndef cdLexer
#define cdLexer

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#include "../lib/gc.h"
#include "../lib/mstack.h"
#include "../lib/hmap.h"
#include "token.h"
#include "error.h"

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#include <io.h>
#include <fcntl.h>

#endif

#include <wchar.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include <errno.h>



sym_t       sym ;

// questo per evitare warning comparazione ( signed wchar_t / unsigned wchar_t )
#define _WEOF (wchar_t)WEOF

// (not standard ) :error: use of undeclared identifier 'errno_t'
typedef int errno_t;

//

wchar_t*    cdConvertFromStringToWString     ( const char* _ansi_string ) ;
int         cdFileWOpen                      ( FILE** pf,char* fileName,const char* flag,const char* ccs ) ; 

#define $2WS(X) cdConvertFromStringToWString(X)

//*********
//  LEXER
//*********

// lexer : max Size

#define maxTokenSize    256
#define maxFileSize     128

// lexer : multiple file

struct lexerBuffer_s
{
    uint32_t        row ;
    uint32_t        col ;
    const wchar_t*  fileInputName ;
    FILE*           pfileInput ;
} ;

typedef struct lexerBuffer_s * lexerBuffer_t ;

// lexer : map keyword

typedef struct mapKW_s
{
  wchar_t*  kw  ;
  sym_t     sym ;  
} mapKW_t ;

extern struct mapKW_s mapArrayKW[] ;

// lexer : fields

struct lexer_s
{
    int             fDebug ;    // emit debug info
    int             flexerScan    ;
    
    //
    
    int             tabSize                 ; // size tab
    FILE            *pFileOutputLexer       ; // file debug lexer pointer
    char            *fileNameOutputLexer    ; // file debug lexer name     
    
    //
    
    uint32_t        row         ;    // posizione attuale di scansione, in genere ove si genera errore
    uint32_t        col         ;
    uint32_t        old_row     ;    // vecchia posizione del token prima di unget
    uint32_t        old_col     ;
        
    const wchar_t*  fileInputName     ;
    FILE*           pfileInput        ;
    
    wchar_t         c0 ;
    wchar_t         c1 ;
    uint32_t        row_start ;    // posizione di partenza del token
    uint32_t        col_start ;

    sym_t           sym ;
    wchar_t         token[maxTokenSize] ;
    size_t          tokenSize;
    
    //
    
    union // value token
    {
        int64_t     integer     ;    // integer
        double      real        ;    // float / double
        wchar_t*    id          ;    // id / keyword
        wchar_t     wchar       ;    // char
        wchar_t*    wstring     ;    // string
    } value ;
    
    //

    stackStruct(lexerBuffer_t,sLexBuffer) ; // stack include file
    
    whmapType(mapKW) ; // mappa keyword
    
} ;

typedef struct lexer_s lexer_t ;

typedef struct lexer_s* plexer_t ;

// lexer : macro

#define $next           lexerGetChar(this) ;
#define $c0             (this->c0)
#define $c1             (this->c1)
#define $pushToken(C)   if ( ! lexerPushToken(this,C) ) return 0;
#define $prev                do { ungetwc ( $c0    ,this->pfileInput ) ; this->col=this->old_col; this->row=this->old_row; } while(0);
#define lexerUnGetChar(C)    do { ungetwc ( C    ,this->pfileInput ) ; this->col=this->old_col; this->row=this->old_row;   } while(0);

// lexer : methods

int         lexerInclude         ( plexer_t this , char * fileInputName ) ;
wchar_t     lexerGetChar         ( plexer_t this ) ;
int         lexerPrintInfoChar   ( plexer_t this ) ;
int         lexerPrintToken      ( plexer_t this ) ;
int         lexerScan            ( plexer_t this ) ;
sym_t       lexerGetToken        ( plexer_t this ) ;
int         lexerEnd             ( plexer_t this ) ; 
int         lexerIsID            ( plexer_t this , wchar_t c ) ;
int         lexerPushToken       ( plexer_t this , wchar_t c ) ;
int         lexerMakeToken       ( plexer_t this , sym_t sym ) ;
int         lexerGetConstNumber  ( plexer_t this , int base  ) ;
sym_t       lexerGetConst        ( plexer_t this , int base  ) ;
plexer_t    lexerAlloc           ( void ) ;
void        lexerDealloc         ( plexer_t this ) ;
void        lexerCtor            ( plexer_t this ) ;
void        lexerDtor            ( plexer_t this ) ;

//

#endif



/**/



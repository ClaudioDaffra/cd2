#ifndef cdError
#define cdError

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif


#include <stdint.h>
#include <wchar.h>
#include <stdlib.h>
#include "../lib/mvector.h"
#include "global.h"

// ................................................... k error,warning

uint32_t kError     ;
uint32_t kWarning   ;

// ................................................... sender

typedef enum sender_e
{
    sender_loader   ,   // caricatore file
    sender_lexer    ,   // lexer
    sender_parser   ,   // parser
    sender_ast      ,   // ast
    sender_node     ,   // node 
    sender_asm      ,   // assembler 
    sender_vm       ,   // Virtual Machine 
    sender_scanner  ,   // analizzatore sematico           
} e_sender_t ;

typedef struct sender_s
{
    const wchar_t*      value;
} sender_t ;

#define $sender(x) sender[sender_##x].value

extern sender_t sender[]  ;

// ................................................... type

typedef enum type_e
{
    type_info       ,   // informativa
    type_warning    ,   // avvertimento
    type_error      ,   // errore
    type_critical   ,   // errore critico
    type_fatal      ,   // errore fatale 
    type_internal       // interno al compilatore   
} e_type_t ;

typedef struct type_s
{
    const wchar_t*      value;  
} type_t ;

#define $type(x) type[type_##x].value

extern type_t type[]  ;
 
// ................................................... action

typedef enum action_e
{
    action_noErr            ,   // nessuno errore = 0
    action_noAction         ,   // nessuna azione
    action_checkFileExists  ,   // controllo esistenza file
    action_scanComment      ,   // mentre scannerizzo commento multilinea
    action_pushToken        ,   // mentre metto carattere nel tokenText    
    action_scanNumber       ,   // mentre sto scannerizzando numeri esa/decimali
    action_convertToNumber  ,   // mentre sto convertendo una stringa ad un numero
    action_parseExpr        ,   // mentro sto parsando espressione
    action_openFile         ,   // mentre sto aprendo/creando un file sorgente
    action_parse            ,   // mentre sto effettuando l'analisi sintattica
    action_malloc           ,   // mentre sto allocando memoria
    action_dealloc          ,   // mentre sto allocando memoria     
    action_debug            ,   // mentre debug ( node o altro ) 
    action_running          ,   // mentre siamo in esecuzione del codice 
    action_assembling       ,   // mentre sto assemblando codice macchina 
    action_scanning       	,   // mentre sto analizzando semanticamente
    action_tokenizing		,	// metre sto analissado col lexer   
            
} e_action_t ;

typedef struct action_s
{
    const wchar_t*      value;  
} action_t ;

#define $action(x) action[action_##x].value

extern action_t action[]  ;

// ................................................... errMessage

typedef enum errMessage_e
{
    errMessage_noErr                   		,   // nessun errore
    errMessage_fileNotFound            		,   // file no trovato
    errMessage_noInputFiles            		,   // nessun file in input    
    errMessage_eof                     		,   // raggiunta la fine del file
    errMessage_eoRem                   		,   // trovato commento multilinea di chiusura, iniziale
    errMessage_overflowTokenText       		,   // la lunghezza del token eccede quella del buffer    
  //  errMessage_expectedNumericLiteral  	,   // atteso numero esadecimale
    errMessage_invalid_argument        		,   // argometo non valido
    errMessage_out_of_range            		,   // fuori range    
  //  errMessage_expectedExponent        	,   // hexadecimal floating constants require an exponent
    errMessage_expectedExponentDigit   		,   // exponent has no digit
  //  errMessage_outOfRangeNDX           	,   // indice vettore fouri dai limiti
  //  errMessage_unexpectedSym           	,   // simbolo inatteso
    errMessage_errUnknown              		,   // errore sconosciuto
    errMessage_unexpectedToken         		,   // token inatteso     
  //  errMessage_didYouMean              	,   // intendevi forse ?
    errMessage_syntaxError             		,   // errore di sintassi
    errMessage_outOfMemory             		,   // mancanza di memoria !  
    errMessage_notImplemetedYet        		,   // non ancora implementato 
    errMessage_division_by_zero        		,   // 1 / 0 ;
    errMessage_duplicateSymbolName	   		,   // identificare già presente nella tabella dei simboli
    errMessage_undeclaredIdentifier	   		,   // identificatore non dichiarato
    errMessage_LValueRequired		   		,   // e' richiesto un valore sinistro
    errMessage_expectedPrimaryExprBefore	,   // expected primary-expression before [token]
    errMessage_expectedPrimaryExprAfter   	,   // expected primary-expression before [token 
    errMessage_arrayBoundNotInteger			,	// array bound is not an integer constant before ']' 
    errMessage_typeVoid						,	// nessun campo trovato nella struttura       
} e_errMessage_t;

typedef struct errMessage_s
{
    const wchar_t*      value;  
} errMessage_t ;

// ........................................................  errMessage[]

#define $errMessage(x) errMessage[errMessage_##x].value

extern errMessage_t errMessage[]  ;

// ........................................................  err log Message

struct errLog_s
{
    e_sender_t          sender      ;
    e_type_t            type        ;
    e_action_t          action      ;
    e_errMessage_t      errMessage  ;
    uint32_t            rowTok      ;
    uint32_t            colTok      ;
    const wchar_t*      fileInput   ;
    const wchar_t*      extra       ;
} ;

typedef struct errLog_s * errLog_t ;

// ........................................................ vettore log

// field

vectorType(errLog_t,vErrLog);

// method

size_t pushErrLog
(
    e_sender_t          sender      ,
    e_type_t            type        ,
    e_action_t          action      ,
    e_errMessage_t      errMessage  ,
    uint32_t            rowTok      ,
    uint32_t            colTok      ,
    const wchar_t*      fileInput   ,
    const wchar_t*      extra        
) ;

void printErrLog(void); // error.printLog

// macro

// ...................................................................................... error

#define $pushErrLog( SENDER,TYPE,ACTION,ERRMESSAGE,ROW,COL,FILE,EXTRA )\
        pushErrLog(\
            sender_##SENDER,\
            type_##TYPE,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            ROW,\
            COL,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
 
// ...................................................................................... loader
  
#define $loader( TYPE,ACTION,ERRMESSAGE,ROW,COL,FILE,EXTRA )\
        pushErrLog(\
            sender_loader,\
            type_##TYPE,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            ROW,COL,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
        
// ...................................................................................... lexer

#define $lexer( TYPE,ACTION,ERRMESSAGE,ROW,COL,FILE,EXTRA )\
        pushErrLog(\
            sender_lexer,\
            type_##TYPE,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->row_start,\
			this->col_start,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup((wchar_t*)EXTRA)\
        ) ;

#define $lexerWarning( ACTION,ERRMESSAGE )\
        pushErrLog(\
            sender_lexer,\
            type_warning,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->row_start,\
			this->col_start,\
            gcWcsDup((wchar_t*)this->fileInputName),\
            NULL\
        ) ;
        
#define $lexerError( ACTION,ERRMESSAGE )\
        pushErrLog(\
            sender_lexer,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->row_start,\
			this->col_start,\
            gcWcsDup((wchar_t*)this->fileInputName),\
            NULL\
        ) ;
        
#define $lexerInternal( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_lexer,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;

#define $lexerErrorExtra( ACTION,ERRMESSAGE,EXTRA )\
        pushErrLog(\
            sender_lexer,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            this->row_start,\
            this->col_start,\
            gcWcsDup((wchar_t*)this->fileInputName),\
            gcWcsDup((wchar_t*)EXTRA)\
        ) ;
        
// ...................................................................................... parser

#define $parserInternal( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_parser,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;

#define $parserInternalExtra( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_parser,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->lexer->row_start,\
			this->lexer->col_start,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
        
#define $parserError( ACTION,ERRMESSAGE )\
        pushErrLog(\
            sender_parser,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->lexer->row_start,\
			this->lexer->col_start,\
            gcWcsDup((wchar_t*)this->lexer->fileInputName),\
            NULL\
        ) ;
        
#define $parserErrorExtra( ACTION,ERRMESSAGE,EXTRA )\
        pushErrLog(\
            sender_parser,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
			this->lexer->row_start,\
			this->lexer->col_start,\
            gcWcsDup((wchar_t*)this->lexer->fileInputName),\
            gcWcsDup(EXTRA)\
        ) ;
        
#define $syntaxError \
            pushErrLog(\
                sender_parser,\
                type_error,\
                action_parse,\
                errMessage_syntaxError,\
                this->lexer->row_start,\
                this->lexer->col_start,\
                gcWcsDup((wchar_t*)this->lexer->fileInputName),\
                gcWcsDup(this->lexer->token)\
           ) ;
 
#define $matchError( EXTRA )\
            pushErrLog(\
                sender_parser,\
                type_error,\
                action_parseExpr,\
                errMessage_unexpectedToken,\
                this->lexer->row_start,\
                this->lexer->col_start,\
                gcWcsDup((wchar_t*)this->lexer->fileInputName),\
                gcWcsDup(EXTRA)\
           ) ; 
                  
// ...................................................................................... ast

#define $astInternal( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_ast,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
        
 // ...................................................................................... node

#define $nodeInternal( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_node,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
         
// ...................................................................................... scanner

#define $scannerInternal( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_scanner,\
            type_internal,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
           
 #define $scannerErrorExtra( ACTION,ERRMESSAGE,FILE,EXTRA )\
        pushErrLog(\
            sender_scanner,\
            type_error,\
            action_##ACTION,\
            errMessage_##ERRMESSAGE,\
            0,\
            0,\
            gcWcsDup((wchar_t*)FILE),\
            gcWcsDup(EXTRA)\
        ) ;
           
                  
#endif



/**/



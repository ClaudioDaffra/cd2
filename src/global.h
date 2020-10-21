
#ifndef cdGlobal
#define cdGlobal

#include "../lib/gc.h"
#include "../lib/stdio.h"
#include "../lib/argParse.h"
#include <stdio.h>

char* 		makeFileWithNewExt			( char* pFileName , char* newExt ) ;
wchar_t		outputSpecialCharInChar		( wchar_t wchar  ) ;
wchar_t*	outputSpecialCharInString	( wchar_t* token ) ;

typedef struct global_s
{
	// FIELDs
	
	int 	fDebug;

	char*	fileInputName ;   // source coude
	FILE*	pFileInput    ;

	char*	fileOutputName;   // result
	FILE* 	pFileOutput   ;

	char*   fileOutputStream ; // force debug info to stream stdout/stderr
	
	// METHODs
	
	char* 		(*makeFileWithNewExt)			( char* pFileName , char* newExt ) ;
	wchar_t		(*outputSpecialCharInChar)		( wchar_t wchar  ) ;
	wchar_t*	(*outputSpecialCharInString)	( wchar_t* token ) ;
	
	
} global_t ;

// var global main.c

extern struct global_s g ;

#endif



/**/



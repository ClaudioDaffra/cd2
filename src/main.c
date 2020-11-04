
#include "../lib/gc.h"
#include "../lib/stdio.h"
#include "../lib/argParse.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
//#include "asm.h"
//#include "vm.h"
#include "symTable.h"
#include <string.h>

// *********
//  GLOBAL
// *********

global_t g = 
{
	0		,	// fDebug
	NULL	,	// file input name
	NULL	,	// *
	NULL	,	// file output name
	NULL	,	// *
	NULL	,	// file output stream
	//
	makeFileWithNewExt			,
	outputSpecialCharInChar		,
	outputSpecialCharInString	,
};

char* makeFileWithNewExt( char* pFileName , char* newExt )
{
	char* 	pFileNameExt 	= strrchr( pFileName,'.'	) ;
	int 	fileNameLen  	= strlen ( pFileName 	 	) ;
	if  ( pFileNameExt==NULL ) pFileNameExt = pFileName + fileNameLen ;

	int 	newExtLen 	 	= strlen ( newExt  	 		) ;
	int 	fileNameNewLen	= ( pFileNameExt - pFileName ) + 1 + newExtLen ;
	char*	pFileNameNew 	= gcMalloc( sizeof(char) * fileNameNewLen ) ;

	pFileNameNew[0] = '\0' ;
	strncat ( pFileNameNew , pFileName , ( pFileNameExt - pFileName ) ) ;
	strcat  ( pFileNameNew , newExt ) ;

	return pFileNameNew ;
}

wchar_t	outputSpecialCharInChar( wchar_t _wchar )
{
	if ( _wchar==0 ) return L'§';
	
	// \r viene saltato \f \v \t \f \' \" \\ vengono trasformati
	wchar_t wchar = _wchar ;
	
	switch ( _wchar ) 
	{
		case L'\n' 	: wchar = L'N'; break ;
		case L'\t' 	: wchar = L'T'; break ;
		case L'\v' 	: wchar = L'V'; break ;
		case L'\f' 	: wchar = L'F'; break ;
		case L'\'' 	: wchar = L'\''; break ;
		case L'\"' 	: wchar = L'\"'; break ;
		case L'\\' 	: wchar = L'\\'; break ;
		default		: break;
	} 

	return wchar ;
}

wchar_t*	outputSpecialCharInString( wchar_t* token )
{
	if ( token == NULL ) return gcWcsDup(L"{null}");
	
	for ( size_t i=0; i < wcslen( token ) ; i++ )
	{
		token[i] = outputSpecialCharInChar( token[i] ) ;
	}
	return token ;
}

// *********
//  MAIN
// *********

int main (int argc , const char** argv )
{
	gcStart();

    // *********
    //  ARGPARSE
    // *********
 
    argParseUsage ( usages ,
        "cd [options] [[-,/,--] args]",
        "cd [options]"
    ) ;
 
    argParseOption ( options ) 
    {
        OPT_HELP(),
        OPT_GROUP   ("Basic options"),
            OPT_STRING  ('i', "input"   , &g.fileInputName 		, "file input to read"  , NULL, 0, 0 ),
            OPT_STRING  ('o', "output"  , &g.fileOutputName 	, "file output to write", NULL, 0, 0 ),
            OPT_BOOLEAN ('d', "debug"   , &g.fDebug   			, "debug"               , NULL, 0, 0 ),
            OPT_STRING  ('$', "stream"  , &g.fileOutputStream 	, "file output stream"  , NULL, 0, 0 ),        
        OPT_END()
    };    
 
    argparse_t argparse;

    argParseInit( &argparse, options, usages, 0);

    argParseDescribe ( &argparse, 
        "\ncd intepreter.", 
        "\nby Claudio Daffra (2020)."
    );

    argParseStart ( &argparse ) ;

    // *********
    //  BEGIN
    // *********

    if ( g.fileInputName == NULL )
    {
    	$loader( fatal , checkFileExists , noInputFiles , 0 , 0 , NULL , NULL );
    }
    else
    {
		if ( g.fDebug ) fwprintf ( stdout,L"\n# main  -> initialising file ...\n"  );
		
		stdFileWOpen ( &g.pFileInput , g.fileInputName , "r","ccs=UTF-8") ;	// ................... file input

/*
        fileNameOutputAsm = makeFileWithNewExt( fileInputName , ".asm" ) ; // ................ file asm
        stdFileWOpen ( &pFileOutputAsm , fileNameOutputAsm , "w+","ccs=UTF-8") ;

        fileNameOutputVM = makeFileWithNewExt( fileInputName , ".vm" ) ; // .................. file vm
        stdFileWOpen ( &pFileOutputVM , fileNameOutputVM , "w+","ccs=UTF-8") ;


*/
    	if ( g.fileOutputName == NULL ) // ..................................................... file output
    	{
		g.fileOutputName 		 = makeFileWithNewExt( g.fileInputName , ".output" ) ;    		
    	}
		stdFileWOpen ( &g.pFileOutput , g.fileOutputName , "w+","ccs=UTF-8") ;
/*
        if ( fileOutputStream != NULL ) // TODO da fare nei rispettivi oggetti
        {
            if ( ! strcmp ( fileOutputStream , "stdout" ) )
            {
                pFileOutputLexer  = stdout ;
                pFileOutputParser = stdout ;
                pFileOutputAST    = stdout ;
                pFileOutputNode   = stdout ;  
                pFileOutputAsm    = stdout ;  
                pFileOutputVM     = stdout ;  
                pFileOutputST     = stdout ;                                                             
            }
            else
            {
               fwprintf ( stdout,L"[main.c  ] ?! argParse : scan parameter : wrong stream [%hs] -> 'stdout' instead",fileOutputStream) ; 
            }
        }
*/
	    if ( g.fDebug )
	    {
			fwprintf ( stdout , L"\n%-20ls : [%018p] -> [%-20hs]" ,L"file input name"	,g.pFileInput 		,g.fileInputName   	 ) ;
	    	fwprintf ( stdout , L"\n%-20ls : [%018p] -> [%-20hs]" ,L"file output name"	,g.pFileOutput 		,g.fileOutputName 	 ) ;                                                    
	    }

	    stdConsoleSetUTF8();
	    
	    if (g.fDebug) fwprintf ( stdout,L"\n\nConsole set to UTF-8\n") ;

 

/*
        // ................... PARSER

        node_t * n=NULL ;

        n=cdParserScan(fileInputName);

        // ................... Debug AST

        if (!kError && fDebug) astDebug(n);

        // ................... Assemble

        if (!kError) astAsm(n);

        if (!kError && fDebug) asmPrintVectorInstr();

        if (!kError) 
        {
            vmRun();
            vmPrintResult( stackTop(vmStack) ) ;
        }



        // ................... Close File
*/
		if (g.pFileInput) 			fclose(g.pFileInput);
		if (g.pFileOutput) 			fclose(g.pFileOutput);
/* 			
 
        if (pFileOutputAsm)     fclose(pFileOutputAsm); 
        if (pFileOutputVM)      fclose(pFileOutputVM);
*/ 

		// *********
		//  symbol Table
		// *********
		
		stInitSymTable();
		
		// *********
		//  PARSER
		// *********

		pparser_t parser = parserAlloc();

		parser->fileInputName = g.fileInputName ;
		parser->fDebug = 1 	;
		
		parserCtor(parser);

		if ( !kError ) 
		{
			pnode_t pn = NULL ;
			
			pn = parserScan(parser);
			
			astNodeDebug ( parser->ast , pn ) ; // esegue check null e fDebug

		}

		// show map

		stShowMap() ;

		parserDtor(parser);

		parserDealloc(parser);

	}
	
    // *********
    //  END
    // *********

    printErrLog();

	stDeInitSymTable();

	gcStop();
	
	fwprintf ( stdout,L"\n" ) ;
	
	return 0 ;
}



/**/



/*
 
		// *********
		//  LEXER
		// *********

		plexer_t lexer = lexerAlloc();
		
		lexerCtor(lexer);
		
		lexer->fDebug = 1 ;

			// LEXER
				lexerInclude ( lexer , g.fileInputName ) ;
				for(;;)
				{
					lexerGetToken(lexer) ; // ottien il token
					if ( lexer->sym == sym_end ) break; 
				}
			// END LEXER
			
		lexerDtor(lexer);
		
		lexerDealloc(lexer);
		 
*/



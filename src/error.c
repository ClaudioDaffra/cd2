#include "error.h"


// ................................................... sender

sender_t sender[] =
{
    { L"loader"     } ,
    { L"lexer"      } ,    
    { L"parser"     } , 
    { L"AST"        } , 
    { L"node"       } , 
    { L"assembler"  } ,  
    { L"VM"         } , 
    { L"scanner"    } ,         
} ;

// ................................................... type

type_t type[] =
{
    { L"! Info"       } ,
    { L"Warning !"    } , 
    { L"? Error"      } , 
    { L"?? Critical"  } , 
    { L"!! Fatal"     } ,   
    { L"## Internal"  } ,     
} ;

// ................................................... action

action_t action[] =
{
    { L"no Error"                 } ,
    { L"no Action"                } ,
    { L"check if file exists"     } ,
    { L"scan comment"             } ,
    { L"push token text"          } ,    
    { L"scanning Number"          } ,
    { L"convert token to number"  } ,
    { L"parsing expression"       } ,    
    { L"open/create file"         } ,
    { L"parsing ..."              } ,
    { L"malloc"                   } , 
    { L"dealloc"                  } ,    
    { L"debug"                    } , 
    { L"running ..."              } ,  
    { L"assembling ..."           } ,  
    { L"scanning ..."             } , 
    { L"tokenizing ..."           } ,              
} ;

// ................................................... errMessage

errMessage_t errMessage[] =
{
    { L"no Error"                                                                       } ,
    { L"file Not Found"                                                                 } ,
    { L"no input files"                                                                 } ,    
    { L"reached end of file"                                                            } ,
    { L"found end */  before begin /*"   												} ,
    { L"text length exceed token max size"                                              } ,    
  //  { L"unable to find numeric literal after"                                         } ,
    { L"invalid argument"                                                               } ,
    { L"out of range"                                                                   } ,
  //  { L"hexadecimal floating constants require an exponent"                           } ,
    { L"exponent has no digits"                                                         } ,
  //  { L"ndx >= source.size()"                                                         } ,
  //  { L"symbol unexpected"                                                            } ,
    { L"error unknown"                                                                  } ,
    { L"unexptected Token"                                                              } ,
  //  { L"Did you mean ?"                                                               } ,
    { L"syntax error"                                                                   } ,
    { L"out of memory"                                                                  } , 
    { L"not implemented yet"                                                            } , 
    { L"division by zero"                                                               } , 
    { L"duplicate symbol name"                                                          } , 
    { L"undeclared identifier"                                                          } , 
    { L"lvalue required as left operand of assignment"                					} , 
    { L"expected primary-expression before" 											} ,  
    { L"expected primary-expression after" 												}       
} ;

// ................................................... push err log


size_t pushErrLog
(
    e_sender_t          sender         ,
    e_type_t            type           ,
    e_action_t          action         ,
    e_errMessage_t      errMessage     ,
    uint32_t            rowTok         ,
    uint32_t            colTok         ,
    const wchar_t*      fileInput      ,
    const wchar_t*      extra        
)
{
    errLog_t err = gcMalloc( sizeof(struct errLog_s) ) ;

    static int fErroLogInit=0; // vector Error Log
    if ( fErroLogInit==0 )
    {
        fErroLogInit    = 1 ;
        vectorNew( vErrLog , 16 );
        kError          = 0 ;
        kWarning        = 0 ; 
    }    

    if ( err->type == type_warning ) 
        ++kWarning ;
    else
        ++kError ;       

    err->sender          = sender    ;
    err->type            = type      ;
    err->action          = action    ;
    err->errMessage      = errMessage;
    err->rowTok          = rowTok    ;
    err->colTok          = colTok    ;
    err->fileInput       = fileInput ;
    err->extra           = extra     ;
    
    vectorPushBack( vErrLog ,err ) ; 

    return vectorSize(vErrLog);
}

// ................................................... print err log

void printErrLog(void)
{
    itVector(vErrLog) itv ;

    for( itv = vectorBegin(vErrLog) ; itv != vectorEnd(vErrLog); itv++ )   
    {
        //if ( ! wcscmp ( (*itv)->extra , L"_WEOF" ) ) continue ;
            
        fwprintf ( stderr , L"\n" ) ; 
 
        if ( (*itv)->fileInput != NULL )
        {
			
        if ( wcslen((*itv)->fileInput) > 20 )
            fwprintf ( stderr,L"[%-17.17ls...]" ,(*itv)->fileInput ) ;
        else
            fwprintf ( stderr,L"[%-20.20ls]"    ,(*itv)->fileInput ) ;       
        
        if ( ( (*itv)->rowTok != 0 ) && ( (*itv)->colTok != 0 ) ) 
        fwprintf ( stderr , L" %03d / %03d :"   , (*itv)->rowTok,(*itv)->colTok - 1        ) ; // COL-1 !!!
        
        fwprintf ( stderr , L" " ) ;       
        }
        
        fwprintf ( stderr , L"%-12ls : "        , (wchar_t*) type      [(unsigned short)   (*itv)->type        ].value ) ; 
        fwprintf ( stderr , L"%-8ls : "         , (wchar_t*) sender    [(unsigned short)   (*itv)->sender      ].value ) ;
        fwprintf ( stderr , L"%ls -> "          , (wchar_t*) action    [(unsigned short)   (*itv)->action      ].value ) ;
        fwprintf ( stderr , L"%ls "             , (wchar_t*) errMessage[(unsigned short)   (*itv)->errMessage  ].value ) ;
        

        if ( (*itv)->extra != NULL )
        {
			wchar_t* temp = gcWcsDup( (wchar_t*) (*itv)->extra ) ;
			fwprintf ( stderr , L": (%ls)", g.outputSpecialCharInString( temp ) );
        }
        fwprintf ( stderr , L"." ) ;  
       
    }

    fwprintf (stderr,L"\n");
}



/**/



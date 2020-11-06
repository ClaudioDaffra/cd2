#ifndef cdToken
#define cdToken

#include <stdio.h>
#include <stdint.h>

#ifdef __linux__
#include <wchar.h>
#endif

typedef enum e_sym
{
    sym_end         ,   // 0
    sym_weof        ,   // 1
    sym_char        ,   // 2    //    chracter C ''    wchar_t
    sym_string      ,   // 3    //    string   C ""    wchar_t*
    sym_id          ,   // 4
    sym_integer     ,   // 5
    sym_real        ,   // 6
    
    //              keyword    
    
    sym_kw=100      ,
    sym_kw_while    ,   // +1
    sym_kw_for      ,   // +2
    sym_kw_const    ,   // +3
    sym_kw_var      ,   // +4
    sym_kw_integer  ,   // +5  
    sym_kw_real     ,   // +6      
    sym_kw_char     ,   // +7
    sym_kw_byte     ,   // +8    
    sym_kw_array    ,   // +9  
    sym_kw_type     ,   // +10      
    sym_kw_function ,   // +11   
                  
    //              special operator  
    sym_op0=150    , 
          
    sym_p0         ,    // +1    )
    sym_p1         ,    // +2    (
    sym_pv         ,    // +3    ;
    sym_v          ,    // +4    ,
    sym_dp         ,    // +5    :
    sym_pq0        ,    // +6    [
    sym_pq1        ,    // +7    ]
    sym_pg0        ,    // +8    {
    sym_pg1        ,    // +9    }  
       
    //              operator 2
    
    sym_op2=200    ,    
    
    sym_assign     ,    // :=
     
    //              operator 1
    
    sym_op1=250    ,
    
    sym_add        ,   // +1 binop/prefix   +
    sym_sub        ,   // +2 binop/prefix   -
    
    sym_not        ,   // +3 prefix         !    
        
    sym_mul        ,   // +4 binop          * / %
    sym_div        ,   // +5
    sym_mod        ,   // +6
        
    sym_eq         ,   // +7 =
 
    //
    
    sym_lexema      ,  // carattere non riconosciuto
      
} sym_t ; 



#endif



/**/

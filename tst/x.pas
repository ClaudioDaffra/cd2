
	// cazzone merdone progrmming language

    array a1 : [10][20] byte ;

    const c1 = "str" ;


    var v1 : integer := 100 ; 
    
    type point_t
    { 
        var     x : integer ;
        var     y : integer ; 
    } ; 

    type line_t
    { 
        var     start : point_t ;
        var     stop  : point_t ; 
    } ;
    type rect_t
    { 
        var     l1 : line_t ;
        var     l2  : line_t ; 
    } ; 


    var v2 : point_t ; 

    array linea : [20] line_t ;

    const   c2 = "a\nb" , 
    c3 = '\n';

    const ccc = 123 ;
    
    const  c4 = 'c' ;

    var v3 : real := 12.34 , v4 : char := 'x' ;

    var v5 : char := "stringa" ;


    var v6 : byte := 0b00000101 ; 

        
    var     p1 : point_t ;
    var     l1 : line_t ;
    var     r1 : rect_t ;
        
    function f1 
    ( 
        var f1_v7 : integer , f1_v8 : integer ; 
        array a  :[10] point_t ;
    ) integer 
    
    {
        //var f1_v9 : real ;
        //const f1_c5 = 0xffff ;
        var VVV : point_t ;
        
        8 ;
        9 ;
        VVV . y;

    } ;


    1 ;


    ! 123 ;
    456 ;

    "claudio" ; 1 ;  "" ; 42 ;3 ;

    arr[11][12] ;

    f1(100  ,   x[21*2][-4] ,   300 ); 

   // variabile ; // not declared


    ccc ;
    
    // lllvvv  ; // ERROR was not declared in this scope
   
    
    //p1 . y ;
    
    //l1 . stop . y ;
    
    r1 . l2 . start . x ; // p1y seg fault non c'è controllo su NULL printf
    
    
    //line_t ; // Error : invalid use of
    
     // qui puoi mettere o sym_pv o sym_end   

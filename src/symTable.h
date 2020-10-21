
#ifndef cdSymTable
#define cdSymTable

#ifdef __APPLE__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

#include "../lib/mvector.h"
#include "../lib/gc.h"
#include "../lib/hmap.h"
#include "error.h"

// .............................................. scope

enum stScope_e
{
	stScopeGlobal	,
	stScopeLocal	
} ;

typedef enum stScope_e stScope_t ;

// .............................................. kind

enum stKind_e
{
	stKindNull		,
	stKindConst		,	// 1
	stKindVar		,	// 2
	stKindArray		,
	stKindStruct	,
	stKindFunction	,
} ;

typedef enum stKind_e stKind_t ;

// .............................................. id

typedef wchar_t * wstring_t ;

// .............................................. nameSpace

vectorType(wstring_t,stNameSpace) ;

// .............................................. type
// to see : ast.h -> eNodeType_t

enum stType_e
{
	stTypeNull ,
	stTypeInteger ,
	stTypeReal	  ,
	stTypeID	  ,
	stTypeString  ,
	stTypeChar	  ,
	stTypeByte	  ,
	// ...
} ;

typedef enum stType_e stType_t ;

// .............................................. sym Table

struct symTable_s ; // forward declaration

typedef struct symTable_s 	 symTable_t ;

typedef struct symTable_s *	psymTable_t ; // puntatore alla tabella dei simboli

struct symTable_s
{
	stScope_t				scope		 ; // Global / Local
	wstring_t 				ns 	 		 ; // path
	wstring_t 				id 			 ; // name
	stKind_t				kind 		 ; // categoria 	: Const/Var/Array/Struct/Function
	stType_t 				type 		 ; // tipo	atomico	: Integer/Real/String/Char/Pointer
	
	uint32_t				nDim		 ; // numero delle dimensioni dell'array
	vectorStruct(uint32_t,  aDim)		 ; // dimensioni dell'array MAXX,MAXY,MAXZ ... ( DIM / NDX ) ;
	
	vectorStruct(psymTable_t,  member)   ; // membri della struttura.
	
 	size_t					size		 ; // dimesioni dell'oggetto
 	size_t					offset		 ; // posizione dell'oggetto
 	void*					address		 ; // indirizzo nello HEAP(global) / STACK(local)
	wstring_t 				typeID 	 	 ; // struct name
	
	/*
	union
	{
	  int64_t	integer 	;
	  double    real 		;
	  wchar_t*	string 		;
	  wchar_t	character 	;
	  uint8_t   byte 		;
	} value ;
	*/
} ;

// .............................................. tabella HASH stTable

typedef struct mapST_s
{
	
  wchar_t*  	fullName  	;	// nome completo da ricercare
  psymTable_t	pST 		;   // puntatore alla tabella dei simboli
  
} mapST_t ;

whmapType(mapST) ;


// prototype


void 		stInitSymTable			(void) ;
void 		stDeInitSymTable		(void) ;

wchar_t* 	stGet_nsid				(size_t level,wchar_t* id) ;
psymTable_t stMakeSymTable			(void) ;
void 		stDebugSymTableNode		(psymTable_t pst) ;
void 		stShowMap				(void) ;

#define 	stGetNameSpace(LEVEL) 	stGet_nsid(LEVEL,NULL)
#define 	stGetFullName(ID) 		stGet_nsid(0,ID)

psymTable_t stFindIDinMap			(wchar_t* id) ;

#endif



/**/



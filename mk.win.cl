TARGET =cd.exe
FLAG= /WX /utf-8 /MP

SOURCES = \
	src\main.c		\
	lib\argParse.c	\
	lib\stdio.c		\
	lib\gc.c		\
	src\lexer.c     \
	lib\hmap.c 		\
	src\ast.c		\
	src\parser.c	\
	src\expr.c		\
	src\symTable.c	\
	src\decl.c
	
all: $(TARGET)

$(TARGET):$(SOURCES)
	cl $(FLAG) /Febin\$(TARGET) $(SOURCES)
	
clean:
		del bin\*.exe & del obj\*.obj & del lib\*.obj
		del *.exe & del *.obj     
		del *.lexer & del tst\*.lexer
		del *.parser & del tst\*.parser        
		del *.output & del tst\*.output
		del *.ast & del tst\*.ast
		del *.node & del tst\*.node    
		del *.asm & del tst\*.asm        
		del *.vm & del tst\*.vm 
 		  

pascal:	pascal.l pascal.y ast_structs.h symtab.h semantic.h translate.h utility.h
				bison -d -t -v pascal.y && \
				flex -o pascal.lex.c pascal.l && \
				gcc -o $@ pascal.tab.c pascal.lex.c ast_structs.c symtab.c semantic.c translate.c utility.c -DYYDEBUG -ly
		
clean:  
	rm -f pascal pascal.tab.h pascal.tab.c pascal.lex.c pascal.output pascal.ll ir.ll

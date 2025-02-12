%option noyywrap 
%option yylineno 
%option bison-locations 
%option bison-bridge 

%{
	#include "utility.h"
    #include "ast_structs.h"
    #include "symtab.h"
	#include "pascal.tab.h"
	
	void yyerror(char *s, ...);
%}

EXP	([Ee][-+]?[0-9]+)

%%
"program"			{ return TOK_PROG; }
"type"				{ return TOK_TYPE; }
"var" 				{ return TOK_VAR; }
"function" 	    	{ return TOK_FUNC; }
"begin" 			{ return TOK_BEGIN; }
"end"				{ return TOK_END; }
"if"				{ return TOK_IF; }
"then"				{ return TOK_THEN; }
"else"				{ return TOK_ELSE; }
"for"				{ return TOK_FOR; }
"while"				{ return TOK_WHILE; }
"to"				{ return TOK_TO; }
"do"				{ return TOK_DO; }
"set"				{ return TOK_SET; }
"array"				{ return TOK_ARRAY; }
"of"				{ return TOK_OF; }

"Length"|"length"	{ return TOK_LENGTH; }
"setlength"			{ return TOK_SETLENGTH; }
"write"				{ return TOK_WRITE; }
"writeln"			{ return TOK_WRITELN; }
"readln"			{ return TOK_READLN; }
"chr"				{ return TOK_CHR; }
"ord"				{ return TOK_ORD; }
"inc"				{ return TOK_INC; }
"low"				{ return TOK_LOW; }
"high"				{ return TOK_HIGH; }
"Random"			{ return TOK_RANDOM; }
"Randomize"			{ return TOK_RANDOMIZE; }

"integer"|"longint"|"boolean"|"char"|"string" { 
	yylval->s_val = yytext; return TOK_ORD_TYPE; }
				
"real" 			{ yylval->s_val = yytext; return TOK_REAL_TYPE; }

"true"|"false" 	{ yylval->b_val = boolval(yytext); return TOK_BOOL; }

":="			{ yylval->s_val = cstrdup(yytext); return TOK_ASSIGN; }
"+"				{ yylval->s_val = cstrdup(yytext); return TOK_PLUS; }
"-"				{ yylval->s_val = cstrdup(yytext); return TOK_MINUS; }
"*"				{ yylval->s_val = cstrdup(yytext); return TOK_MUL; }
"/"				{ yylval->s_val = cstrdup(yytext); return TOK_DIV; }
"div"			{ yylval->s_val = cstrdup(yytext); return TOK_INTDIV; }
"and"			{ yylval->s_val = cstrdup(yytext); return TOK_AND; }
"or"			{ yylval->s_val = cstrdup(yytext); return TOK_OR; }
"in"			{ yylval->s_val = cstrdup(yytext); return TOK_IN; }
"="				{ yylval->s_val = cstrdup(yytext); return TOK_EQ; }
"<"				{ yylval->s_val = cstrdup(yytext); return TOK_LT; }
">"				{ yylval->s_val = cstrdup(yytext); return TOK_GT; }
"<="			{ yylval->s_val = cstrdup(yytext); return TOK_LTEQ; }
">="			{ yylval->s_val = cstrdup(yytext); return TOK_GTEQ; }

","				{ return TOK_COMMA; }
"."				{ return TOK_DOT; }
";"				{ return TOK_SEMICOLON; }
":"				{ return TOK_COLON; }
"'"				{ return TOK_SINGLEQUOTE; }
".."			{ return TOK_SUBRANGE; }
"("				{ return TOK_LPAREN; }
")"				{ return TOK_RPAREN; }
"["				{ return TOK_LBRACKET; }
"]"				{ return TOK_RBRACKET; }

[0-9]+	{ yylval->i_val = atoi(yytext); return TOK_INTEGER; }

[0-9]+"."[0-9]+|[0-9]+"."[0-9]*{EXP}?|"."?[0-9]+{EXP}? { 
	yylval->d_val = atoi(yytext); return TOK_REAL; }
	
[A-Za-z][A-Za-z0-9_]* { 
	yylval->s_val = cstrdup(yytext); return TOK_IDENTIFIER; }

\'.\'			{ yylval->c_val = yytext[1]; return TOK_CHAR; }

[']([^'\\\n]|\\.|\\\n)*['] { 
	yylval->s_val = cstrdup(yytext); return TOK_STRING; fprintf(stderr, "%s\n", yytext);}
	
"{".*"}"		; /* skip comments */
[ \t]+			; /* skip white spaces & tabs */
[\n]			; /* skipe new lines */

. { fprintf(stderr, "(L) Syntax ERR: line: %d, mystery character '%s'\n", 
				yylloc->first_line, yytext); }
%%

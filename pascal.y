%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdarg.h>
	#include "ast_structs.h"
	#include "symtab.h"
	#include "semantic.h"
	#include "translate.h"
	#include "utility.h"
	
	extern FILE *yyin;
	extern int yylex();
	int p;
	void yyerror(char *s, ...);
	
	static AST_prog root;
%}

%locations
%pure-parser /* thread safety */

/* The %union declaration specifies the entire collection of 
 * possible data types for semantic values. 
 * Lexer can access these union fields in yylval. 
 */

%union {
	char* s_val;			/* constant, oper-types, types */
	char c_val;				/* constant */
	int i_val;				/* integers */
	double d_val;			/* real numbers */
	int b_val;				/* bool values, 0 or 1 */ 
	
	AST_op op;
	
	AST_prog prog;
	AST_block block;
	
	AST_decl decl;
	AST_var var;
	AST_stmt stmt;
	AST_type ty;
	AST_expr expr;
	
	AST_stmt_seq_l stmtseq_l;
	
	AST_ty_decl_l tydecl_l;
	AST_ty_decl tydecl;
	
	AST_var_decl_l vardecl_l;
	AST_var_decl vardecl;
	
	AST_func_decl_l funcdecl_l;
	AST_func_decl funcdecl;
	
	AST_ident_l ident_l;
	AST_fparam_l fparam_l;
	AST_expr_l expr_l;
	
	Symbol sym;
}

/* The operator not shall have the highest precedence, 
 * followed by the multiplying-operators, then the adding-operators 
 * and signs, and finally, with the lowest precedence, 
 * the relational-operators. Sequences of two or more operators of 
 * the same precedence shall be left associative
 */

/* enforce left-associativity, with precendence from low to high */
%left <s_val> TOK_OR TOK_AND 
%left <s_val> TOK_EQ TOK_LT TOK_GT TOK_LTEQ TOK_GTEQ
%left <s_val> TOK_PLUS TOK_MINUS
%left <s_val> TOK_MUL TOK_DIV TOK_INTDIV
%left <s_val> TOK_IN

%precedence TOK_THEN
%precedence TOK_ELSE

/* Terminals in the grammar */
%token TOK_PROG TOK_VAR TOK_FUNC TOK_TYPE TOK_BEGIN TOK_END 
%token TOK_IF TOK_THEN TOK_ELSE
%token TOK_FOR TOK_WHILE TOK_TO TOK_DO
%token TOK_SET TOK_ARRAY TOK_OF
%token TOK_CHR TOK_ORD TOK_LENGTH TOK_SETLENGTH TOK_INC TOK_LOW TOK_HIGH 
%token TOK_RANDOM TOK_RANDOMIZE
%token TOK_WRITE TOK_WRITELN TOK_READLN
%token TOK_ASSIGN TOK_LPAREN TOK_RPAREN TOK_SEMICOLON TOK_COLON TOK_DOT
%token TOK_COMMA TOK_SINGLEQUOTE TOK_LBRACKET TOK_RBRACKET TOK_SUBRANGE

%token <s_val> TOK_STRING TOK_IDENTIFIER TOK_ORD_TYPE TOK_REAL_TYPE
%token <c_val> TOK_CHAR
%token <i_val> TOK_INTEGER
%token <d_val> TOK_REAL
%token <b_val> TOK_BOOL

/* Nonterminals in the grammar */ 
%type <prog> program 
%type <block> block

%type <decl> type_decl_part var_decl_part

%type <tydecl> type_decl
%type <tydecl_l> type_decl_l

%type <vardecl> var_decl 
%type <vardecl_l> var_decl_l

%type <funcdecl> func_decl 
%type <funcdecl_l> func_decl_l

%type <var> var entire_var indexed_var
%type <ty> simple_type type set_type arr_type

%type <stmt> compound_stmt
%type <stmt> stmt simple_stmt structured_stmt assign_stmt
%type <stmt> conditional_stmt for_stmt while_stmt procedure_stmt 
%type <stmt> write_stmt writeln_stmt readln_stmt
%type <stmt> inc_stmt randomize_stmt setlength_stmt
%type <stmtseq_l> stmt_seq

%type <expr> expr simple_expr factor term write_param
%type <expr> unsigned_const primary set_construct func_call subrange

%type <op> add_op mul_op rel_op

%type <ident_l> ident_list
%type <fparam_l> fparam_list
%type <expr_l> expr_list writeln_param

%type <sym> ident ord_type real_type

%start program

%%

/* The components matched by the rule can be accessed by $n, 
 * which stands for the value of the nth component. 
 * The semantic value for the grouping being constructed is $$.
 * Default rule is: $$ = $1 */
 
program: TOK_PROG TOK_IDENTIFIER TOK_SEMICOLON block TOK_DOT { 
		$$ = AST_Prog(cstrdup($2), $4); root = $$; }
;

block: type_decl_part func_decl_l var_decl_part compound_stmt {
		$$ = AST_Block($1, $2, $3, $4); }
;

type_decl_part: TOK_TYPE type_decl_l { $$ = AST_TyDec($2); }
              | { $$ = NULL; }
;

type_decl_l: type_decl TOK_SEMICOLON type_decl_l {
				$$ = AST_TypeDeclList($1, $3); }
		   | { $$ = NULL; }
;
			
type_decl: ident TOK_EQ type { $$ = AST_TyDecl($1, $3); }
	     
;
			 
var_decl_part: TOK_VAR var_decl_l { $$ = AST_VarDec($2); }
             | { $$ = NULL; }
;

var_decl_l: var_decl TOK_SEMICOLON var_decl_l {
			  $$ = AST_VarDeclList($1, $3); }		
		  | { $$ = NULL; }
;

var_decl: ident_list TOK_COLON type { $$ = AST_VarDecl($1, $3); }
;

ident_list: ident { $$ = AST_IdentList($1, NULL); }
          | ident TOK_COMMA ident_list { $$ = AST_IdentList($1, $3); }
          | { $$ = NULL; }
;
    
ident: TOK_IDENTIFIER { $$ = new_sym(cstrdup($1), NULL); }
;

type: simple_type { $$ = $1; }
    | set_type { $$ = $1; }		 /* e.g. set of char */  		
    | arr_type { $$ = $1; }		 /* e.g. array of integer, string[100] */	
; 	 	
      
simple_type: ord_type { $$ = AST_SimpleType($1); } 
           | real_type  { $$ = AST_SimpleType($1); } 
		   | ident { $$ = AST_NamedType($1); } 
;

ord_type: TOK_ORD_TYPE { $$ = new_sym(cstrdup($1), NULL); }
;

real_type: TOK_REAL_TYPE { $$ = new_sym(cstrdup($1), NULL); }
;
    
set_type: TOK_SET TOK_OF TOK_ORD_TYPE { $$ = AST_SetType(new_sym(cstrdup($3), NULL)); }
;

arr_type: ord_type TOK_LBRACKET expr TOK_RBRACKET { 
			$$ = AST_ArrayType($1, $3); }
        | TOK_ARRAY TOK_OF ord_type {
			$$ = AST_ArrayType($3, NULL); }
;

func_decl_l: func_decl func_decl_l {
				$$ = AST_FuncDeclList($1, $2); }
			| { $$ = NULL; }
;

func_decl: TOK_FUNC ident TOK_LPAREN fparam_list TOK_RPAREN TOK_COLON 
			type TOK_SEMICOLON block TOK_SEMICOLON { 
    $$ = AST_FuncDecl($2, $4, $7, $9); }
;
         
fparam_list: ident_list TOK_COLON type { 
				$$ = AST_FormalParamList($1, $3, NULL); }
		   | ident_list TOK_COLON type TOK_SEMICOLON fparam_list {
				$$ = AST_FormalParamList($1, $3, $5); }
		   | { $$ = NULL; }
;

expr_list: expr { 
			$$ = AST_ExprList($1, NULL); }
         | expr TOK_COMMA expr_list { 
			$$ = AST_ExprList($1, $3); }
		 | { $$ = NULL; }
;

compound_stmt: TOK_BEGIN stmt_seq TOK_END { 
				$$ = AST_CompoundStmt($2); }
			 | { $$ = NULL; }
;
			
stmt_seq: stmt { $$ = AST_SequenceStmt($1, NULL); }
		| stmt TOK_SEMICOLON stmt_seq { 
			$$ = AST_SequenceStmt($1, $3); }
;

stmt: simple_stmt { $$ = $1; }
    | structured_stmt { $$ = $1; }
;
	   
simple_stmt: assign_stmt { $$ = $1; }
           | procedure_stmt { $$ = $1; };
				  
assign_stmt: var TOK_ASSIGN expr { $$ = AST_AssignStmt($1, $3); } 
;
				 
var: entire_var { $$ = $1; }
   | indexed_var { $$ = $1; }
;
				
entire_var: ident { $$ = AST_EntireVar($1); }
;

indexed_var: entire_var TOK_LBRACKET expr TOK_RBRACKET { 
	$$ = AST_IndexedVar($1, $3); }
;
		 
expr: simple_expr { $$ = $1; }
    | simple_expr rel_op simple_expr { $$ = AST_OpExp($1, $2, $3); }
;
	   
simple_expr: term { $$ = $1; }
		   | simple_expr add_op term { $$ = AST_OpExp($1, $2, $3); }
;

term: factor { $$ = $1; }
    | term mul_op factor { $$ = AST_OpExp($1, $2, $3); }
;
		
factor: var { $$ = AST_VarExp($1); }
      | unsigned_const { $$ = $1; }
      | primary { $$ = $1; }
	  | set_construct { $$ = $1; }
	  | TOK_LPAREN expr TOK_RPAREN { $$ = $2; }
	  | func_call { $$ = $1; }
;
		  
unsigned_const: TOK_INTEGER { $$ = AST_IntExp($1); }
              | TOK_REAL { $$ = AST_RealExp($1); }
              | TOK_STRING { $$ = AST_StringExp(cstrdup($1)); }
              | TOK_CHAR { $$ = AST_CharExp($1); }
			  | TOK_BOOL { $$ = AST_BoolExp($1); }
;

primary: TOK_MINUS factor { $$ = AST_NegExp($2); } 
;

add_op: TOK_PLUS { $$ = AST_plusOp; }
      | TOK_MINUS { $$ = AST_minusOp; }
	  | TOK_OR { $$ = AST_orOp; }
;

mul_op: TOK_MUL { $$ = AST_mulOp; }
      | TOK_DIV { $$ = AST_divOp; }
      | TOK_INTDIV { $$ = AST_intDivOp; }
	  | TOK_AND { $$ = AST_andOp; }
;

rel_op: TOK_EQ { $$ = AST_eqOp; }
      | TOK_LT { $$ = AST_ltOp; }
	  | TOK_LTEQ { $$ = AST_lteqOp; }
	  | TOK_GT { $$ = AST_gtOp; }
	  | TOK_GTEQ { $$ = AST_gteqOp; }
	  | TOK_IN { $$ = AST_inOp; }
;
					   
set_construct: TOK_LBRACKET subrange TOK_RBRACKET { 
	$$ = $2;}
;

subrange: unsigned_const TOK_SUBRANGE unsigned_const { $$ = AST_SetExp($1, $3); }
;

func_call: ident TOK_LPAREN expr_list TOK_RPAREN { 
			$$ = AST_UFuncExp($1, $3); }
         | TOK_ORD TOK_LPAREN var TOK_RPAREN { 
			$$ = AST_OrdExp($3, new_sym(cstrdup("ord"), NULL)); }
		 | TOK_CHR TOK_LPAREN expr TOK_RPAREN { 
			$$ = AST_ChrExp($3, new_sym(cstrdup("chr"), NULL)); }
		 | TOK_LENGTH TOK_LPAREN expr TOK_RPAREN { 
			$$ = AST_LengthExp($3, new_sym(cstrdup("length"), NULL)); }
		 | TOK_LOW TOK_LPAREN var TOK_RPAREN {
			$$ = AST_LowExp($3, new_sym(cstrdup("low"), NULL)); }
		 | TOK_HIGH TOK_LPAREN var TOK_RPAREN {
			$$ = AST_HighExp($3, new_sym(cstrdup("high"), NULL)); }
		 | TOK_RANDOM TOK_LPAREN expr TOK_RPAREN {
			$$ = AST_RandomExp($3, new_sym(cstrdup("random"), NULL)); }
;
						   					
structured_stmt: compound_stmt { $$ = $1; };
               | conditional_stmt { $$ = $1; };
			   | for_stmt { $$ = $1; };
			   | while_stmt { $$ = $1; };
;
						
/* using different precedence rules to associate the dangling “else” 
 * with the nearest “if” to solve dangling-else problem */
conditional_stmt: TOK_IF expr TOK_THEN stmt { 
					$$ = AST_IfStmt($2, $4, NULL); } 
                | TOK_IF expr TOK_THEN stmt TOK_ELSE stmt { 
					$$ = AST_IfStmt($2, $4, $6); }
;

for_stmt: TOK_FOR assign_stmt TOK_TO expr TOK_DO stmt { 
	$$ = AST_ForStmt($2, $4, $6); }
;

while_stmt: TOK_WHILE expr TOK_DO stmt {
	$$ = AST_WhileStmt($2, $4); }
;
					 
procedure_stmt: write_stmt { $$ = $1; }
              | writeln_stmt { $$ = $1; }
			  | readln_stmt { $$ = $1; }
			  | inc_stmt { $$ = $1; }
			  | randomize_stmt { $$ = $1; }
			  | setlength_stmt { $$ = $1; }
;

write_stmt: TOK_WRITE TOK_LPAREN expr_list write_param TOK_RPAREN { 
	$$ = AST_WriteStmt($3, $4, new_sym(cstrdup("write"), NULL)); }
;

write_param: TOK_COLON expr { $$ = $2; }
           |  { $$ = NULL; }
;

writeln_stmt: TOK_WRITELN writeln_param { 
	$$ = AST_WritelnStmt($2, new_sym(cstrdup("writeln"), NULL)); }
;

writeln_param: TOK_LPAREN expr_list TOK_RPAREN { $$ = $2; }
	         | { $$ = NULL; }
			 ;

readln_stmt: TOK_READLN TOK_LPAREN var TOK_RPAREN { 
	$$ = AST_ReadlnStmt($3, new_sym(cstrdup("readln"), NULL)); }
;

inc_stmt: TOK_INC TOK_LPAREN var TOK_RPAREN {
	$$ = AST_IncStmt($3, new_sym(cstrdup("inc"), NULL)); }
;

randomize_stmt: TOK_RANDOMIZE {
	$$ = AST_RandomizeStmt(); }
;

setlength_stmt: TOK_SETLENGTH TOK_LPAREN expr TOK_COMMA expr TOK_RPAREN {
	$$ = AST_SetLengthStmt($3, $5, new_sym(cstrdup("setlength"), NULL)); }
;

%%

int main(int argc, char **argv){

	if(argc != 2){
		fprintf(stderr, "please specify a filename to parse...\n");
		exit(1);
	}

	yyin = fopen(argv[1], "r");
	if(NULL == yyin) {
		fprintf(stderr, "File does not exist...\n");
		exit(1);
		return 0;
	}
	
	#if YYDEBUG == 1
  	extern int yydebug;
    yydebug = 1;
	#endif
	
	/* reads tokens, executes actions, returns when end-of-input or 
	 * unrecoverable syntax error */
	p = yyparse();
	if(p == 0){
		printAST(root);
		if(semantic_validation(root) == 1){
			fprintf(stderr, "\n-----------------------------\n");
			fprintf(stderr, "| STARTING CODE TRANSLATION |\n");
			fprintf(stderr, "-----------------------------\n");
			translate_code(root);
		} else {
			fprintf(stderr, "------------------------------\n");
			fprintf(stderr, "| SEMANTIC VALIDATION FAILED |\n");
			fprintf(stderr, "------------------------------\n");
		}

		return EXIT_SUCCESS;
	} else { 
		fprintf(stderr, "\n------------------\n");
		fprintf(stderr, "| PARSING FAILED |\n");
		fprintf(stderr, "------------------\n");
	}
}

void yyerror(char *s, ...) {
  extern int yylineno;

  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d line: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

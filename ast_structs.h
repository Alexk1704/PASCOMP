#include "symtab.h"
/*
 * Each node has as many childern as the corresponding grammar production 
 * has RHS symbols. We translate the grammar directly into 
 * data structure defintions, each grammar symbol corresponds to a 
 * typedef in the data structures
 * 
 * For each grammar rule, there is one constructor that belongs to the 
 * union for its LHS symbol. Each grammar rule has RHS components that 
 * must be represented in the data structures.
 * 
 * Each grammar symbols struct contains a union to carry these values, 
 * and a kind field to indicate which variant of the union is valid.
 * 
 * Trees are described by grammar. A tree is described by one or more 
 * typedefs corresponding to a symbol in the grammar. 
 * Each typedef defines a pointer to a corresponding struct.
 * 
 * If there is more than one nontrivial (value-carrying) symbol in the 
 * RHS of a rule (ex. AST_stmt_seq) the union will have a 
 * component that is itself a struct comprising these values 
 * (e.g. seq. element of A_stm_ union).
 * 
 * If there is only one nontrivial symbol in the RHS of a rule, 
 * the union will have a component that is the value 
 * (ex. the num field of the AST_expr union).
 * 
 * Every class will have a constructor function that inits all fields. 
 * The malloc function shall never be called directly, 
 * except in these constructor functions.
 */

/* for structure purposes */
typedef struct AST_prog_ *AST_prog;
typedef struct AST_block_ *AST_block;

/* declarations in block node */
typedef struct AST_func_decl_ *AST_func_decl;
typedef struct AST_var_decl_ *AST_var_decl;
typedef struct AST_ty_decl_ *AST_ty_decl;

/* list nodes */
typedef struct AST_stmt_seq_l_ *AST_stmt_seq_l;

typedef struct AST_var_decl_l_ *AST_var_decl_l;
typedef struct AST_func_decl_l_ *AST_func_decl_l;
typedef struct AST_ty_decl_l_ *AST_ty_decl_l;

typedef struct AST_ident_l_ *AST_ident_l;
typedef struct AST_fparam_l_ *AST_fparam_l; 
typedef struct AST_expr_l_ *AST_expr_l;

/* main nodes with variants */
typedef struct AST_decl_ *AST_decl;
typedef struct AST_stmt_ *AST_stmt;
typedef struct AST_expr_ *AST_expr;
typedef struct AST_var_ *AST_var;
typedef struct AST_type_ *AST_type;

typedef enum { 
	AST_plusOp, AST_minusOp, AST_mulOp, AST_divOp, AST_intDivOp, 
	AST_eqOp, AST_ltOp, AST_lteqOp, AST_gtOp, AST_gteqOp, 
	AST_andOp, AST_orOp, AST_inOp
} AST_op;

struct AST_prog_ {
	char* name;
	AST_block block;
};

struct AST_block_ {
	AST_decl types;
	AST_decl vars; 
	AST_func_decl_l funcs; // list of function declarations
	AST_stmt compound;
};

struct AST_decl_ {
	enum { AST_varDecl, AST_typeDecl } kind;
	union {
		AST_var_decl_l var; // list of var declarations
		AST_ty_decl_l type; // list of type declarations
	}u;
};

struct AST_var_ {
	enum { AST_entireVar, AST_indexedVar } kind;
	union {
		Symbol entire;
		struct { AST_var var; AST_expr expr; } indexed;
	}u;
};
  
struct AST_type_ {
	enum { AST_simpleType, AST_arrayType, AST_setType, AST_namedType } kind;
	union {
		Symbol simple; // string
		Symbol set; // set of char
		struct { Symbol arr; AST_expr bounds; } arr; // string[100], array of string
	} u;
};

struct AST_stmt_ {
	enum {
		AST_compStmt, AST_assignStmt, AST_ifStmt, AST_forStmt, 
		AST_whileStmt, AST_writeStmt, AST_writelnStmt, AST_incStmt,
		AST_readlnStmt, AST_randomizeStmt, AST_setlengthStmt
	} kind;
	union {
		AST_stmt_seq_l sseq; // compound stmt
		struct { AST_var var; AST_expr expr; } assign;
		struct { AST_expr test; AST_stmt body; } whilee;
		struct { AST_expr test; AST_stmt then, elsee; } iff;
		struct { AST_stmt assign; AST_expr to; AST_stmt body; } forr;
		struct { AST_expr_l expr_list; AST_expr width; Symbol name; } write;
		struct { AST_expr_l expr_list; Symbol name; } writeln;
		struct { AST_var var; Symbol name; } varcall; // readln & inc
		struct { AST_expr left; AST_expr right; Symbol name; } setlength;
	} u;
};

struct AST_expr_ {
	enum { 
		AST_opExp, AST_varExp, AST_negExp, AST_intExp, AST_realExp,
		AST_stringExp, AST_charExp, AST_boolExp, AST_setExp, 
		AST_uFuncExp, AST_ordExp, AST_chrExp, AST_lengthExp, 
		AST_lowExp, AST_highExp, AST_randomExp
	} kind;
	union {
		AST_var var; /* var exp */
		AST_expr expr; /* neg exp */
		int integer; /* int exp */
		double real; /* real exp */
		char* str; /* string exp */
		char chr; /* char exp */
		int boolean; /* bool exp */
		struct { AST_expr left; AST_op op; AST_expr right; } op; /* operator expr */
		struct { AST_var var; Symbol name; } varfunc; /* low, high, ord */
		struct { AST_expr expr; Symbol name; } exprfunc; /* chr, length, random */
		struct { AST_expr lo; AST_expr hi; } set; /* set sub range */
		struct { Symbol func; AST_expr_l args; } fcall; /* user defined functions */
	} u;
};

/* Linked list data structure tail points to next entry */
struct AST_stmt_seq_l_ { 
	AST_stmt head; 
	AST_stmt_seq_l tail; 
};

struct AST_ty_decl_l_ {
	AST_ty_decl head;
	AST_ty_decl_l tail;
};

struct AST_ty_decl_ {
	Symbol name; /* Identifier of type (variable) */
	AST_type ty; /* type of defined type */
};

struct AST_var_decl_l_ { 
	AST_var_decl head; 
	AST_var_decl_l tail; 
};

struct AST_var_decl_ {
	AST_ident_l idents; /* identifier/s */
	AST_type ty; /* type */
};

struct AST_func_decl_l_ { 
	AST_func_decl head; 
	AST_func_decl_l tail; 
};

struct AST_func_decl_ {
	Symbol name; /* identifier of function (variable) */
	AST_fparam_l params; /* params to pass to use function */
	AST_type result; /* return type, can be std type or user type */
	AST_block block; /* function block immediately after declaration */
};

struct AST_ident_l_ { 
	Symbol sym; 
	AST_ident_l tail; 
};

struct AST_expr_l_ { 
	AST_expr head; 
	AST_expr_l tail; 
};

/* e.g. a,b,c: integer; x,y,z: string */
struct AST_fparam_l_ {
	AST_ident_l idents; 
	AST_type ty; 
	AST_fparam_l tail; 
};

/* function prototypes */
AST_prog AST_Prog(char* name, AST_block block);
AST_block AST_Block(
	AST_decl types, AST_func_decl_l funcs, AST_decl vars, AST_stmt comp);
/* decl subtypes */
AST_decl AST_VarDec(AST_var_decl_l var);
//AST_decl AST_FuncDec(AST_func_decl_l func);
AST_decl AST_TyDec(AST_ty_decl_l type);
/* specific declarations */
AST_ty_decl AST_TyDecl(Symbol name, AST_type ty);
AST_var_decl AST_VarDecl(AST_ident_l idents, AST_type ty);
AST_func_decl AST_FuncDecl(
	Symbol name, AST_fparam_l params, AST_type result, AST_block block);
/* var subtypes */
AST_var AST_EntireVar(Symbol sym);
AST_var AST_IndexedVar(AST_var var, AST_expr expr);
/* type subtypes */
AST_type AST_SimpleType(Symbol simple);
AST_type AST_NamedType(Symbol simple);
AST_type AST_SetType(Symbol set);
AST_type AST_ArrayType(Symbol arr, AST_expr bounds);
/* stmt subtypes */
AST_stmt AST_CompoundStmt(AST_stmt_seq_l sseq);
AST_stmt_seq_l AST_SequenceStmt(AST_stmt head, AST_stmt_seq_l tail);
AST_stmt AST_AssignStmt(AST_var var, AST_expr expr);
AST_stmt AST_IfStmt(AST_expr test, AST_stmt then, AST_stmt elsee);
AST_stmt AST_WhileStmt(AST_expr test, AST_stmt body);
AST_stmt AST_ForStmt(AST_stmt assign, AST_expr to, AST_stmt body);
AST_stmt AST_WriteStmt(AST_expr_l expr_list, AST_expr width, Symbol name);
AST_stmt AST_WritelnStmt(AST_expr_l expr_list, Symbol name);
AST_stmt AST_ReadlnStmt(AST_var var, Symbol name);
AST_stmt AST_IncStmt(AST_var var, Symbol name);
AST_stmt AST_RandomizeStmt();
AST_stmt AST_SetLengthStmt(AST_expr left, AST_expr right, Symbol name);
/* expr subtypes */
AST_expr AST_OpExp(AST_expr left, AST_op op, AST_expr right);
AST_expr AST_VarExp(AST_var var);
AST_expr AST_NegExp(AST_expr expr);
AST_expr AST_IntExp(int integer);
AST_expr AST_RealExp(double real);
AST_expr AST_StringExp(char* str);
AST_expr AST_CharExp(char chr);
AST_expr AST_BoolExp(int boolean);
AST_expr AST_SetExp(AST_expr lo, AST_expr hi);
AST_expr AST_UFuncExp(Symbol func, AST_expr_l args);
AST_expr AST_OrdExp(AST_var var, Symbol name);
AST_expr AST_ChrExp(AST_expr expr, Symbol name);
AST_expr AST_LengthExp(AST_expr expr, Symbol name);
AST_expr AST_LowExp(AST_var var, Symbol name);
AST_expr AST_HighExp(AST_var var, Symbol name);
AST_expr AST_RandomExp(AST_expr expr, Symbol name);
/* lists */
AST_var_decl_l AST_VarDeclList(AST_var_decl head, AST_var_decl_l tail);
AST_func_decl_l AST_FuncDeclList(AST_func_decl head, AST_func_decl_l tail);
AST_ty_decl_l AST_TypeDeclList(AST_ty_decl head, AST_ty_decl_l tail);
AST_ident_l AST_IdentList(Symbol sym, AST_ident_l tail);
AST_fparam_l AST_FormalParamList(AST_ident_l idents, AST_type ty, AST_fparam_l tail);
AST_expr_l AST_ExprList(AST_expr head, AST_expr_l tail);

void printAST(AST_prog prog_node);

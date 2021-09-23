/* Holds functions to create instances of abstract syntax rules */
#include <stdio.h>
#include <string.h>
#include "ast_structs.h"
#include "symtab.h"
#include "utility.h"


AST_prog AST_Prog(
	char* name, AST_block block) {
		AST_prog ptr = cmalloc(sizeof(*ptr));
		ptr->name = name;
		ptr->block = block;
		return ptr;
}

AST_block AST_Block
	(AST_decl types, AST_func_decl_l funcs, AST_decl vars, AST_stmt comp) {
		AST_block ptr = cmalloc(sizeof(*ptr));
		ptr->types = types;
		ptr->funcs = funcs;
		ptr->vars = vars;
		ptr->compound = comp;
		return ptr;
}

AST_decl AST_TyDec(AST_ty_decl_l type){
	AST_decl ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_typeDecl;
	ptr->u.type = type;
	return ptr;
}

AST_decl AST_VarDec(AST_var_decl_l var){
	AST_decl ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_varDecl;
	ptr->u.var = var;
	return ptr;
}

AST_ty_decl AST_TyDecl(Symbol name, AST_type ty){
	AST_ty_decl ptr = cmalloc(sizeof(*ptr));
	ptr->name = name;
	ptr->ty = ty;
	return ptr;
}

AST_var_decl AST_VarDecl(AST_ident_l idents, AST_type ty){
	AST_var_decl ptr = cmalloc(sizeof(*ptr));
	ptr->idents = idents;
	ptr->ty = ty;
	return ptr;
}

AST_func_decl AST_FuncDecl(
	Symbol name, AST_fparam_l params, AST_type result, AST_block block) {
		AST_func_decl ptr = cmalloc(sizeof(*ptr));
		ptr->name = name;
		ptr->params = params;
		ptr->result = result;
		ptr->block = block;
		return ptr;
}

AST_var AST_EntireVar(Symbol sym){
	AST_var ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_entireVar;
	ptr->u.entire = sym;
	return ptr;
}

AST_var AST_IndexedVar(AST_var var, AST_expr expr){
	AST_var ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_indexedVar;
	ptr->u.indexed.var = var;
	ptr->u.indexed.expr = expr;
	return ptr;
}


AST_type AST_SimpleType(Symbol simple){
	AST_type ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_simpleType;
	ptr->u.simple = simple;
	return ptr;
}

AST_type AST_NamedType(Symbol simple){
	AST_type ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_namedType;
	ptr->u.simple = simple;
	return ptr;
}

AST_type AST_SetType(Symbol set){
	AST_type ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_setType;
	ptr->u.set = set;
	return ptr;
}

AST_type AST_ArrayType(Symbol arr, AST_expr bounds){
	AST_type ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_arrayType;
	ptr->u.arr.arr = arr;
	ptr->u.arr.bounds = bounds;
	return ptr;
}

AST_stmt AST_CompoundStmt(AST_stmt_seq_l sseq){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_compStmt;
	ptr->u.sseq = sseq;
	return ptr;
}

AST_stmt_seq_l AST_SequenceStmt(AST_stmt head, AST_stmt_seq_l tail){
	AST_stmt_seq_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

AST_stmt AST_AssignStmt(AST_var var, AST_expr expr){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_assignStmt;
	ptr->u.assign.var = var;
	ptr->u.assign.expr = expr;
	return ptr;
}

AST_stmt AST_IfStmt(AST_expr test, AST_stmt then, AST_stmt elsee){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_ifStmt;
	ptr->u.iff.test = test;
	ptr->u.iff.then = then;
	ptr->u.iff.elsee = elsee;
	return ptr;
}

AST_stmt AST_WhileStmt(AST_expr test, AST_stmt body){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_whileStmt;
	ptr->u.whilee.test = test;
	ptr->u.whilee.body = body;
	return ptr;
}

AST_stmt AST_ForStmt(AST_stmt assign, AST_expr to, AST_stmt body){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_forStmt;
	ptr->u.forr.assign = assign;
	ptr->u.forr.to = to;
	ptr->u.forr.body = body;
	return ptr;
}

AST_stmt AST_WriteStmt(AST_expr_l expr_list, AST_expr width, Symbol name){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_writeStmt;
	ptr->u.write.expr_list = expr_list;
	ptr->u.write.width = width;
	ptr->u.write.name = name;
	return ptr;
}

AST_stmt AST_WritelnStmt(AST_expr_l expr_list, Symbol name){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_writelnStmt;
	ptr->u.writeln.expr_list = expr_list;
	ptr->u.writeln.name = name;
	return ptr;
}

AST_stmt AST_ReadlnStmt(AST_var var, Symbol name){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_readlnStmt;
	ptr->u.varcall.var = var;
	ptr->u.varcall.name = name;
	return ptr;
}

AST_stmt AST_IncStmt(AST_var var, Symbol name){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_incStmt;
	ptr->u.varcall.var = var;
	ptr->u.varcall.name = name;
	return ptr;
}

AST_stmt AST_RandomizeStmt(){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_randomizeStmt;
	return ptr;
}

AST_stmt AST_SetLengthStmt(AST_expr left, AST_expr right, Symbol name){
	AST_stmt ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_setlengthStmt;
	ptr->u.setlength.left = left;
	ptr->u.setlength.right = right;
	ptr->u.setlength.name = name;
	return ptr;
}

AST_expr AST_OpExp(AST_expr left, AST_op op, AST_expr right){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_opExp;
	ptr->u.op.left = left;
	ptr->u.op.op = op;
	ptr->u.op.right = right;
	return ptr;
}

AST_expr AST_VarExp(AST_var var){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_varExp;
	ptr->u.var = var;
	return ptr;
}

AST_expr AST_NegExp(AST_expr expr){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_negExp;
	ptr->u.expr = expr;
	return ptr;
}

AST_expr AST_IntExp(int integer){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_intExp;
	ptr->u.integer = integer;
	return ptr;
}

AST_expr AST_RealExp(double real){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_realExp;
	ptr->u.real = real;
	return ptr;
}

AST_expr AST_StringExp(char* str){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_stringExp;
	ptr->u.str = cstrdup(str);
	return ptr;
}

AST_expr AST_CharExp(char chr){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_charExp;
	ptr->u.chr = chr;
	return ptr;
}

AST_expr AST_BoolExp(int boolean){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_boolExp;
	ptr->u.boolean = boolean;
	return ptr;
}

AST_expr AST_SetExp(AST_expr lo, AST_expr hi){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_setExp;
	ptr->u.set.lo = lo;
	ptr->u.set.hi = hi;
	return ptr;
}

AST_expr AST_UFuncExp(Symbol func, AST_expr_l args){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_uFuncExp;
	ptr->u.fcall.func = func;
	ptr->u.fcall.args = args;
	return ptr;
}

AST_expr AST_OrdExp(AST_var var, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_ordExp;
	ptr->u.varfunc.var = var;
	ptr->u.varfunc.name = name;
	return ptr;
}

AST_expr AST_ChrExp(AST_expr expr, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_chrExp;
	ptr->u.exprfunc.expr = expr;
	ptr->u.exprfunc.name = name;
	return ptr;
}

AST_expr AST_LengthExp(AST_expr expr, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_lengthExp;
	ptr->u.exprfunc.expr = expr;
	ptr->u.exprfunc.name = name;
	return ptr;
}

AST_expr AST_LowExp(AST_var var, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_lowExp;
	ptr->u.varfunc.var = var;
	ptr->u.varfunc.name = name;
	return ptr;
}

AST_expr AST_HighExp(AST_var var, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_highExp;
	ptr->u.varfunc.var = var;
	ptr->u.varfunc.name = name;
	return ptr;
}

AST_expr AST_RandomExp(AST_expr expr, Symbol name){
	AST_expr ptr = cmalloc(sizeof(*ptr));
	ptr->kind = AST_randomExp;
	ptr->u.exprfunc.expr = expr;
	ptr->u.exprfunc.name = name;
	return ptr;
}

AST_ty_decl_l AST_TypeDeclList(AST_ty_decl head, AST_ty_decl_l tail){
	AST_ty_decl_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

AST_var_decl_l AST_VarDeclList(AST_var_decl head, AST_var_decl_l tail){
	AST_var_decl_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

AST_func_decl_l AST_FuncDeclList(AST_func_decl head, AST_func_decl_l tail){
	AST_func_decl_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

AST_ident_l AST_IdentList(Symbol sym, AST_ident_l tail){
	AST_ident_l ptr = cmalloc(sizeof(*ptr));
	ptr->sym = sym;
	ptr->tail = tail;
	return ptr;
}

AST_fparam_l AST_FormalParamList(
	AST_ident_l idents, AST_type ty, AST_fparam_l tail){
		AST_fparam_l ptr = cmalloc(sizeof(*ptr));
		ptr->idents = idents;
		ptr->ty = ty;
		ptr->tail = tail;;
		return ptr;
}

AST_expr_l AST_ExprList(AST_expr head, AST_expr_l tail){
	AST_expr_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

/* forward declaration of print funcs */
static void pr_block(AST_block block, int ind);
static void pr_decls(AST_decl decl, int ind);
static void pr_vdecll(AST_var_decl_l vdecll, int ind);
static void pr_fdecll(AST_func_decl_l fdecll, int ind);
static void pr_tdecll(AST_ty_decl_l tdecll, int ind);
static void pr_vdecl(AST_var_decl vdecl, int ind);
static void pr_fdecl(AST_func_decl fdecl, int ind);
static void pr_tdecl(AST_ty_decl tdecl, int ind);
static void pr_identl(AST_ident_l identl, int ind);
static void pr_sym(Symbol sym, int ind);
static void pr_fparaml(AST_fparam_l fparaml, int ind);
static void pr_stmts(AST_stmt stmt, int ind);
static void pr_exprs(AST_expr expr, int ind);
static void pr_exprl(AST_expr_l exprl, int ind);
static void pr_vars(AST_var var, int ind);
static void pr_types(AST_type type, int ind);
static void pr_sseql(AST_stmt_seq_l sseql, int ind);

static const char* str_oper[] = {
   "+", "-", "*", "/", "DIV", "=", "<", "<=", ">", ">=", "AND", "OR", "IN" };

static void indent(int i){
	for(int j = 0; j < i; j++) fprintf(stderr, " ");
}

void printAST(AST_prog prog_node){
	fprintf(stderr, "---------------\n"); fprintf(stderr, "| DUMPING AST |\n"); 
	fprintf(stderr, "---------------\n");
	if(prog_node != NULL) {
		fprintf(stderr, "PROGRAM (%s)\n", prog_node->name);
		pr_block(prog_node->block, 1);
	}
}

static void pr_block(AST_block block, int ind){
	if(block != NULL) {
		indent(ind); fprintf(stderr, "| BLOCK\n");
		pr_decls(block->types, ind+1);
		pr_decls(block->vars, ind+1);
		pr_fdecll(block->funcs, ind+1);
		pr_stmts(block->compound, ind+1);
	}
}

static void pr_decls(AST_decl decl, int ind){
	if(decl != NULL) {
		switch(decl->kind){
			case AST_typeDecl:
				indent(ind); fprintf(stderr, "| TYPE DECLS\n");
				pr_tdecll(decl->u.type, ind+1);
				break;
			case AST_varDecl:
				indent(ind); fprintf(stderr, "| VAR DECLS\n");
				pr_vdecll(decl->u.var, ind+1);
				break;
			default:
				fprintf(stderr, "UNKNOWN DECL TYPE!");
				break;
		}
	}
}

static void pr_tdecll(AST_ty_decl_l tdecll, int ind){
	if(tdecll != NULL) {
		indent(ind); fprintf(stderr, "| TYPE DECL LIST\n");
		AST_ty_decl_l temp = tdecll;
		int i = ind+1;
		while(temp->tail) {
			pr_tdecl(temp->head, i); fprintf(stderr, "\n");
			temp = temp->tail;
		}
		pr_tdecl(temp->head, i); fprintf(stderr, "\n");
	}	
}

static void pr_vdecll(AST_var_decl_l vdecll, int ind){
	if(vdecll != NULL) {
		indent(ind); fprintf(stderr, "| VAR DECL LIST\n");
		AST_var_decl_l temp = vdecll;
		int i = ind+1;
		while(temp->tail) {
			pr_vdecl(temp->head, i); fprintf(stderr, "\n");
			temp = temp->tail;
		}
		pr_vdecl(temp->head, i); fprintf(stderr, "\n");
	}	
}

static void pr_fdecll(AST_func_decl_l fdecll, int ind){
	if(fdecll != NULL) {
		indent(ind); fprintf(stderr, "| FUNC DECL LIST\n");
		AST_func_decl_l temp = fdecll;
		int i = ind+1;
		while(temp->tail) {
			pr_fdecl(temp->head, i); fprintf(stderr, "\n");
			temp = temp->tail;
		}
		pr_fdecl(temp->head, i); fprintf(stderr, "\n");
	}
}

static void pr_tdecl(AST_ty_decl tdecl, int ind){
	if(tdecl != NULL) {
		indent(ind); fprintf(stderr, "| TYPE DECL\n");
		pr_sym(tdecl->name, ind+1);
		pr_types(tdecl->ty, ind+1);
	}
}

static void pr_vdecl(AST_var_decl vdecl, int ind){
	if(vdecl != NULL) {
		indent(ind); fprintf(stderr, "| VAR DECL\n");
		pr_identl(vdecl->idents, ind+1);
		pr_types(vdecl->ty, ind+1);

	}	
}

static void pr_fdecl(AST_func_decl fdecl, int ind){
	if(fdecl != NULL) {
		indent(ind); fprintf(stderr, "| FUNC DECL\n");
		pr_sym(fdecl->name, ind+1);
		pr_fparaml(fdecl->params, ind+1);
		pr_types(fdecl->result, ind+1);
		pr_block(fdecl->block, ind+1);
	}	
}

static void pr_identl(AST_ident_l identl, int ind){
	if(identl != NULL) {
		indent(ind); fprintf(stderr, "| IDENTIFIER LIST\n");
		AST_ident_l temp = identl;
		int i = ind+1;
		while(temp->tail) {
			pr_sym(temp->sym, i);	
			temp = temp->tail;
		}
		pr_sym(temp->sym, i);
	}
}

static void pr_types(AST_type type, int ind){
	if(type != NULL){
		switch(type->kind){
			case AST_simpleType:
				indent(ind); fprintf(stderr, "| SIMPLE TYPE\n");
				pr_sym(type->u.simple, ind+1);
				break;
			case AST_arrayType:
				indent(ind); fprintf(stderr, "| ARRAY TYPE\n");
				pr_sym(type->u.arr.arr, ind+1);
				pr_exprs(type->u.arr.bounds, ind+1);
				break;
			case AST_setType:
				indent(ind); fprintf(stderr, "| SET TYPE\n");
				pr_sym(type->u.set, ind+1);
				break;
			case AST_namedType:
				indent(ind); fprintf(stderr, "| NAME TYPE\n");
				pr_sym(type->u.simple, ind+1);
				break;
			default:
				fprintf(stderr, "WRONG TYPE NODE");
				break;
		}
	}
}

static void pr_sym(Symbol sym, int ind){
	if(sym != NULL){
		indent(ind); fprintf(stderr, "| SYMBOL (%s)\n", get_sym(sym));
	}
}

static void pr_fparaml(AST_fparam_l fparaml, int ind){
	if(fparaml != NULL) {
		indent(ind); fprintf(stderr, "| FPARAM LIST\n");
		AST_fparam_l temp = fparaml;
		int i = ind+1;
		while(temp->tail){	
			pr_identl(temp->idents, ind+1); 
			pr_types(temp->ty, ind+1);
			temp = temp->tail;
		}
		pr_identl(temp->idents, ind+1); pr_types(temp->ty, ind+1);
	}
}

static void pr_stmts(AST_stmt stmt, int ind){
	if(stmt != NULL) {
		switch(stmt->kind){
			case AST_compStmt:
				indent(ind); fprintf(stderr, "| COMPOUND STMT\n");
				pr_sseql(stmt->u.sseq, ind+1);
				break;
			case AST_assignStmt:
				indent(ind); fprintf(stderr, "| ASSIGN STMT\n");
				pr_vars(stmt->u.assign.var, ind+1);
				pr_exprs(stmt->u.assign.expr, ind+1);
				break;
			case AST_ifStmt:
				indent(ind); fprintf(stderr, "| IF STMT\n");
				pr_exprs(stmt->u.iff.test, ind+1);
				pr_stmts(stmt->u.iff.then, ind+1);
				pr_stmts(stmt->u.iff.elsee, ind+1);
				break;
			case AST_forStmt:
				indent(ind); fprintf(stderr, "| FOR STMT\n");
				pr_stmts(stmt->u.forr.assign, ind+1);
				pr_exprs(stmt->u.forr.to, ind+1);
				pr_stmts(stmt->u.forr.body, ind+1);
				break;
			case AST_whileStmt:
				indent(ind); fprintf(stderr, "| WHILE STMT\n");
				pr_exprs(stmt->u.whilee.test, ind+1);
				pr_stmts(stmt->u.whilee.body, ind+1);
				break;
			case AST_writeStmt:
				indent(ind); fprintf(stderr, "| WRITE STMT\n");
				pr_exprl(stmt->u.write.expr_list, ind+1);
				pr_exprs(stmt->u.write.width, ind+1);
				pr_sym(stmt->u.write.name, ind+1);
				break;
			case AST_writelnStmt:
				indent(ind); fprintf(stderr, "| WRITELN STMT\n");
				pr_exprl(stmt->u.writeln.expr_list, ind+1);
				pr_sym(stmt->u.writeln.name, ind+1);
				break;
			case AST_readlnStmt:
				indent(ind); fprintf(stderr, "| READLN STMT\n");
				pr_vars(stmt->u.varcall.var, ind+1);
				pr_sym(stmt->u.varcall.name, ind+1);
				break;
			case AST_randomizeStmt:
				indent(ind); fprintf(stderr, "| RANDOMIZE STMT\n");
				break;
			case AST_setlengthStmt:
				indent(ind); fprintf(stderr, "| SETLENGTH STMT\n");
				pr_exprs(stmt->u.setlength.left, ind+1);
				pr_exprs(stmt->u.setlength.right, ind+1);
				pr_sym(stmt->u.setlength.name, ind+1);
				break;
			case AST_incStmt:
				indent(ind); fprintf(stderr, "| INC STMT\n");
				pr_vars(stmt->u.varcall.var, ind+1);
				pr_sym(stmt->u.varcall.name, ind+1);
				break;
			default:
				fprintf(stderr, "UNKNOWN STMT NODE");
				break;
		}
	}
}

static void pr_exprs(AST_expr expr, int ind){
	if(expr != NULL) {
		switch(expr->kind){
			case AST_opExp:
				indent(ind); fprintf(stderr, "| OP EXPR\n");
				pr_exprs(expr->u.op.left, ind+1);
				indent(ind); fprintf(stderr, "| OPER: %s\n", str_oper[expr->u.op.op]);
				pr_exprs(expr->u.op.right, ind+1);
				break;
			case AST_varExp:
				indent(ind); fprintf(stderr, "| VAR EXPR\n");
				pr_vars(expr->u.var, ind+1);
				break;
			case AST_negExp:
				indent(ind); fprintf(stderr, "| NEG EXPR\n");
				pr_exprs(expr->u.expr, ind+1);
				break;
			case AST_chrExp:
				indent(ind); fprintf(stderr, "| CHR EXPR\n");
				pr_exprs(expr->u.exprfunc.expr, ind+1);
				pr_sym(expr->u.exprfunc.name, ind+1);
				break;
			case AST_lengthExp:
				indent(ind); fprintf(stderr, "| LENGTH EXPR\n");
				pr_exprs(expr->u.exprfunc.expr, ind+1);
				pr_sym(expr->u.exprfunc.name, ind+1);
				break;
			case AST_randomExp:
				indent(ind); fprintf(stderr, "| RANDOM EXPR\n");
				pr_exprs(expr->u.exprfunc.expr, ind+1);
				pr_sym(expr->u.exprfunc.name, ind+1);
				break;	
			case AST_intExp:
				indent(ind); fprintf(stderr, "| INT EXPR (%d)\n", expr->u.integer);				
				break;
			case AST_realExp:
				indent(ind); fprintf(stderr, "| REAL EXPR (%f)\n", expr->u.real);	
				break;
			case AST_stringExp:
				indent(ind); fprintf(stderr, "| STRING EXPR (%s)\n", expr->u.str);	
				break;
			case AST_charExp:
				indent(ind); fprintf(stderr, "| CHAR EXPR (%c)\n", expr->u.chr);	
				break;
			case AST_boolExp:
				indent(ind); fprintf(stderr, "| BOOL EXPR (%d)\n", expr->u.boolean);	
				break;
			case AST_setExp:
				indent(ind); fprintf(stderr, "| SET EXPR\n");
				pr_exprs(expr->u.set.lo, ind+1);
				pr_exprs(expr->u.set.hi, ind+1);
				break;
			case AST_lowExp:
				indent(ind); fprintf(stderr, "| LOW EXP\n");
				pr_vars(expr->u.varfunc.var, ind+1);
				pr_sym(expr->u.varfunc.name, ind+1);
				break;
			case AST_highExp:
				indent(ind); fprintf(stderr, "| HIGH EXPR\n");
				pr_vars(expr->u.varfunc.var, ind+1);
				pr_sym(expr->u.varfunc.name, ind+1);
				break;
			case AST_ordExp:
				indent(ind); fprintf(stderr, "| ORD EXPR\n");
				pr_vars(expr->u.varfunc.var, ind+1);
				pr_sym(expr->u.varfunc.name, ind+1);
				break;
			case AST_uFuncExp:
				indent(ind); fprintf(stderr, "| USER FUNC CALL EXPR\n");
				pr_sym(expr->u.fcall.func, ind+1);
				pr_exprl(expr->u.fcall.args, ind+1);
				break;
			default:
				fprintf(stderr, "UNKNOWN EXPR NODE");
				break;
		}
	}
}

static void pr_vars(AST_var var, int ind){
	if(var != NULL) {
		switch(var->kind){
			case AST_entireVar:
				indent(ind); fprintf(stderr, "| ENTIRE VAR\n"); 
				pr_sym(var->u.entire, ind+1);
				break;
			case AST_indexedVar:
				indent(ind); fprintf(stderr, "| INDEXED VAR\n");
				pr_vars(var->u.indexed.var, ind+1);
				pr_exprs(var->u.indexed.expr, ind+1);
				break;
			default:
				fprintf(stderr, "UNKNOWN VAR NODE");
				break;
		}
	}
}

static void pr_exprl(AST_expr_l exprl, int ind){
	if(exprl != NULL) {
		indent(ind); fprintf(stderr, "| EXPR LIST\n");
		AST_expr_l temp = exprl;
		int i = ind+1;
		while(temp->tail){
			pr_exprs(temp->head, i); temp = temp->tail;
			
		}
		pr_exprs(temp->head, i);
	}
}

static void pr_sseql(AST_stmt_seq_l sseql, int ind){
	if(sseql != NULL) {
		indent(ind); fprintf(stderr, "| STMT SEQ LIST\n");
		AST_stmt_seq_l temp = sseql;
		int i = ind+1;
		while(temp->tail){
			pr_stmts(temp->head, i); temp = temp->tail;
		}
		pr_stmts(temp->head, i);
	}
}

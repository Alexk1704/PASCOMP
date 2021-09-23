#include <stdlib.h>
#include <stdio.h>
#include "semantic.h"
#include "ast_structs.h"
#include "symtab.h"
#include "utility.h"

Sym_tab curr_scope; // table for the current scope we are in
static int curr_scope_level = 0; // counter for current scope
static int failed = 0;

/* Types are needed to check what type an expression represents. */
static const char* str_ty[] = {
   "INT", "LONGINT", "REAL", "STRING", "CHAR", 
   "BOOL", "ARRAY", "SET", "NAME", "VOID" };

static struct Type_ty_ tyint = { Type_int};
Type_ty Type_Int(void) { return &tyint; }

static struct Type_ty_ tylongint = { Type_longint};
Type_ty Type_Longint(void) { return &tylongint; }

static struct Type_ty_ tyreal = { Type_real};
Type_ty Type_Real(void) { return &tyreal; }

static struct Type_ty_ tystring = { Type_string};
Type_ty Type_String(void) { return &tystring; }

static struct Type_ty_ tychar = { Type_char};
Type_ty Type_Char(void) { return &tychar; }

static struct Type_ty_ tybool = { Type_bool};
Type_ty Type_Bool(void) { return &tybool; }

static struct Type_ty_ tyvoid = { Type_void};
Type_ty Type_Void(void) { return &tyvoid; }

Type_ty Type_Array(Type_ty ty) {
	Type_ty ptr = cmalloc(sizeof(*ptr));
	ptr->kind = Type_arr;
	ptr->u.ty = ty;
	return ptr;
}

Type_ty Type_Set(Type_ty ty) {
	Type_ty ptr = cmalloc(sizeof(*ptr));
	ptr->kind = Type_set;
	ptr->u.ty = ty;
	return ptr;
}

Type_ty Type_Name(Symbol sym, Type_ty ty) {
	Type_ty ptr = cmalloc(sizeof(*ptr));
	ptr->kind = Type_name;
	ptr->u.name.sym = sym;
	ptr->u.name.ty = ty;
	return ptr;
}

Type_ty_l Type_TyList(Type_ty head, Type_ty_l tail) {
	Type_ty_l ptr = cmalloc(sizeof(*ptr));
	ptr->head = head;
	ptr->tail = tail;
	return ptr;
}

/* TABLE ENVIRONMENT: We bind the environment entry to a symbol in
 * our table, so we have detailed type information when needed. 
 * Environment is later used in type-checking phase. */

Env_entry Env_VarEntry(Type_ty ty){
	Env_entry final = cmalloc(sizeof(*final));
	final->kind = Env_varEntry;
	final->u.var.ty = ty;
	return final;
}

Env_entry Env_FuncEntry(Type_ty_l fparams, Type_ty result){
	Env_entry final = cmalloc(sizeof(*final));
	final->kind = Env_funcEntry;
	final->u.func.fparams = fparams;
	final->u.func.result = result;
	return final;
}

/* Env base contains value "base" with "predefined" things. 
 * Mappings of symbol integer to Type_int, etc. */

Sym_tab Env_base(void) {
	/* Create global scope table */
	Sym_tab tab = create_tab(curr_scope_level);

	/* Insert primitive data types */
	insert_sym(tab, new_sym("integer", Type_Int()));
	insert_sym(tab, new_sym("longint", Type_Longint()));
	insert_sym(tab, new_sym("real", Type_Real()));
	insert_sym(tab, new_sym("string", Type_String()));
	insert_sym(tab, new_sym("char", Type_Char()));
	insert_sym(tab, new_sym("boolean", Type_Bool()));
		
	return tab;
}

/* SEMANTIC VAL PART */
static void check_block(void* node, int node_ty);

static Type_ty check_var(Sym_tab tab, AST_var var);
static Type_ty check_stmt(Sym_tab tab, AST_stmt stmt);
static Type_ty check_expr(Sym_tab tab, AST_expr expr);

static void check_decl(Sym_tab tab, AST_decl decl);
static void check_funcdecl(Sym_tab tab, AST_func_decl_l fdecll);

static Type_ty_l create_tylist(Sym_tab tab, AST_fparam_l fpl);
static Symbol get_ty_sym(AST_type tynode);
static int check_args(Sym_tab tab, AST_expr_l expl, Type_ty_l tyl, AST_expr func);
static Type_ty check_ty(Sym_tab tab, AST_type ty);
static int match_type(Type_ty a, Type_ty b);
static Type_ty actual_type(Type_ty ty);
static Type_ty ordinal_type(Type_ty ty);
static int check_ioproc(Type_ty ty);

int semantic_validation(void* prog_node) {
	AST_prog prog = (AST_prog) prog_node;
	fprintf(stderr, "\n----------------------\n");
	fprintf(stderr, "| SEMANTIC VALIDATOR |\n");
	fprintf(stderr, "----------------------\n");
	curr_scope = Env_base();
	curr_scope = begin_scope(curr_scope); // enter global scope
	fprintf(stderr, "-------------------------------------\n");
	fprintf(stderr, "| ENTERED SCOPE LEVEL: (%d) (GLOBAL) |\n", curr_scope_level);
	fprintf(stderr, "-------------------------------------\n");
	check_block(prog_node, 0); // semantic check global scope
	if(failed > 0) return 0;
	return 1;
	
}

static void check_block(void* node, int node_ty){
	AST_prog prog_node;
	AST_func_decl func_node;
	Type_ty block_res;
	
	if(node_ty == 0) { 
		prog_node = (AST_prog) node;
		check_decl(curr_scope, prog_node->block->types);
		check_decl(curr_scope, prog_node->block->vars);
		check_funcdecl(curr_scope, prog_node->block->funcs); 
		block_res = check_stmt(curr_scope, prog_node->block->compound);
	}
	else if(node_ty == 1) {
		func_node = (AST_func_decl) node;
		check_decl(curr_scope, func_node->block->types); 
		check_decl(curr_scope, func_node->block->vars); 
		check_funcdecl(curr_scope, func_node->block->funcs); 
		block_res = check_stmt(curr_scope, func_node->block->compound); 
	}
}

static Type_ty check_var(Sym_tab tab, AST_var var) {
	Env_entry v;
	Type_ty expr_e, check, set_e;

	switch(var->kind){
		case AST_entireVar:
			/* Get Env_entry for symbol and lookup variable */
			if(global_lookup(var->u.entire) == NULL) v = NULL;
			else v = get_symvalue(global_lookup(var->u.entire)); 
	
			if(v && v->kind == Env_varEntry) {
				/* fprintf(stderr, "CALLED check_var for %s, type: %s\n", 
					 get_sym(var->u.entire), str_ty[v->u.var.ty->kind]); */
				if(actual_type(v->u.var.ty)->kind == Type_set){
					set_e = v->u.var.ty;
					return Type_Set(actual_type(set_e)->u.ty);
				} // special case for string[]
				if(actual_type(v->u.var.ty)->kind == Type_arr) { 
					if(actual_type(v->u.var.ty->u.ty)->kind == Type_string) {
						return Type_String();
					}
				}
				return actual_type(v->u.var.ty);
			} 
			else if(v && v->kind == Env_funcEntry) {
				return actual_type(v->u.func.result);
			}
			else {
				fprintf(stderr, "ERR | Var %s is undefined.\n",
					get_sym(var->u.entire)); failed++; return Type_Int();
			}
			break;
		case AST_indexedVar:
			check = check_var(tab, var->u.indexed.var); // lookup var symbol
			if(check->kind == Type_arr){
				/* Check if expression for index is of integer type */
				expr_e = check_expr(tab, var->u.indexed.expr);
				if(actual_type(expr_e)->kind != Type_int) {
					failed++; fprintf(stderr, "ERR | Array index needs to be int type.\n");
				} else {
					if(actual_type(check->u.ty)->kind == Type_string) {
						return Type_Char(); // special case for string array
					}
					return actual_type(check->u.ty);
				}
			} else if(actual_type(check)->kind == Type_string) { // string array case
				return Type_Char();
			} else {
				failed++; fprintf(stderr, "ERR | Var is not an array type.\n");
			}
			return Type_Int();
			break;
		default:
			fprintf(stderr, "ERR | Valid kind of var AST node expected.\n");
			exit(1);
			break;
	}
}
 
static Type_ty check_stmt(Sym_tab tab, AST_stmt stmt) {
	AST_stmt_seq_l temp;
	AST_stmt lo_asgn;
	AST_expr_l expl;
	Type_ty test1, test2, then, elsee, asgn1, asgn2, asgn3, 
			to, body, lo_v, lo_e, v1, v2, left, right, wr, wrl, rvar;
	
	switch(stmt->kind) {
		case AST_compStmt:
			temp = stmt->u.sseq;
			if(!temp) return Type_Void();
			while(temp->tail) { 
				check_stmt(tab, temp->head);
				temp = temp->tail;				
			}
			if(temp->head != NULL) {
				check_stmt(tab, temp->head);
			}
			return Type_Void();
			break;
		case AST_assignStmt:
			asgn1 = check_var(tab, stmt->u.assign.var);
			asgn2 = check_expr(tab, stmt->u.assign.expr);
			
			/* if array or set, get underlying type */
			if(asgn1->kind == Type_arr || asgn1->kind == Type_set) 
				asgn1 = asgn1->u.ty;
			if(asgn2->kind == Type_arr || asgn2->kind == Type_set) 
				asgn2 = asgn2->u.ty;
				
			//fprintf(stderr, "ASGN1 TYPE: %s \n", str_ty[asgn1->kind]);
			//fprintf(stderr, "ASGN2 TYPE: %s \n", str_ty[asgn2->kind]);
			if(match_type(asgn1, asgn2) == 0) {
				failed++;
				fprintf(stderr, "ERR | Types for assign expr are unmatched.\n");		
			}
			return Type_Void();
			break;
		case AST_ifStmt:
			test1 = check_expr(tab, stmt->u.iff.test);
			then = check_stmt(tab, stmt->u.iff.then);
			if(stmt->u.iff.elsee){
				elsee = check_stmt(tab, stmt->u.iff.elsee);
				if(!match_type(then, elsee)) {
					failed++;
					fprintf(stderr, "ERR | If-else must return same type.\n");
				}
			}
			if(test1->kind != Type_bool) {
				failed++;
				fprintf(stderr, "ERR | Test condition of if stmt is not a bool type.\n");
			}
			return then;
			break;
		case AST_forStmt:
			asgn3 = check_stmt(tab, stmt->u.forr.assign);
			to = check_expr(tab, stmt->u.forr.to);
			lo_asgn = stmt->u.forr.assign;
			lo_v = check_var(tab, lo_asgn->u.assign.var);
			lo_e = check_expr(tab, lo_asgn->u.assign.expr);
			if(stmt->u.forr.assign->kind != AST_assignStmt) {
				failed++;
				fprintf(stderr, "ERR | Lower range stmt must be an assign stmt.\n");
			} else {
				if(lo_v->kind != Type_int && lo_v->kind != Type_longint) {
					failed++;
					fprintf(stderr, "ERR | Lower range var must be int type.\n");
				}
				if(lo_v->kind != Type_int && lo_v->kind != Type_longint) {
					failed++;
					fprintf(stderr, "ERR | Lower range expr must be int type.\n");
				}
				if(to->kind != Type_int) {
					failed++;
					fprintf(stderr, "ERR | High range expr must be int type.\n");
				}
				body = check_stmt(tab, stmt->u.forr.body);
			}
			return Type_Void();
			break;
		case AST_whileStmt:
			test2 = check_expr(tab, stmt->u.whilee.test);
			if (test2->kind != Type_int && test2->kind != Type_bool) {
				failed++;
				fprintf(stderr, "ERR | Test expr of while-loop is not an integer type.\n");
			}
			check_stmt(tab, stmt->u.whilee.body); 
			return Type_Void();
			break;
		case AST_writeStmt:
			expl = stmt->u.write.expr_list;
			if(expl){
				while(expl->tail){
					wr = check_expr(tab, expl->head);
					if(check_ioproc(wr) == 0) {
						failed++; 
						fprintf(stderr, "Write only accepts numeric, string & char exprs.\n");
					}
					expl=expl->tail;
				} 
				if(expl->head != NULL) {
					wr = check_expr(tab, expl->head);
					if(check_ioproc(wr) == 0) {
						failed++; 
						fprintf(stderr, "Write only accepts numeric, string & char exprs.\n");
					}
				}
			}
			return Type_Void();
			break;
		case AST_writelnStmt:
			expl = stmt->u.writeln.expr_list;
			if(expl){
				while(expl->tail){
					wrl = check_expr(tab, expl->head);
					if(check_ioproc(wrl) == 0) {
						failed++; 
						fprintf(stderr, "Writeln only accepts numeric, string & char exprs.\n");
					}
					expl=expl->tail;
				} 
				if(expl->head != NULL) {
					wrl = check_expr(tab, expl->head);
					if(check_ioproc(wrl) == 0) {
						failed++; 
						fprintf(stderr, "Writeln only accepts numeric, string & char exprs.\n");
					}
				}
			}
			return Type_Void();
			break;
		case AST_readlnStmt:
			 rvar = check_var(tab, stmt->u.varcall.var);
			 if(check_ioproc(rvar) == 0){
					failed++;
					fprintf(stderr, "ERR | Readln expects var to be a num, char or string type.\n");
				}
			 return Type_Void();
			break;
		case AST_incStmt:
			v1 = check_var(tab, stmt->u.varcall.var);
			if(match_type(actual_type(v1), Type_Int()) == 0) {
				failed++;
				fprintf(stderr, "ERR | Expected integer type for inc statement call.\n"); 
			}
			set_symvalue(stmt->u.varcall.name, Env_FuncEntry(
				Type_TyList(actual_type(v1), NULL), 
				Type_Void()));
			return Type_Void();
			break;
		case AST_randomizeStmt:
			return Type_Void();
			break;
		case AST_setlengthStmt:
			left = check_expr(tab, stmt->u.setlength.left);
			right = check_expr(tab, stmt->u.setlength.right);
			if(left->kind != Type_arr) {
				failed++;
				fprintf(stderr, "ERR | Left expr of setlength has to be a dynamic array.\n");
			}
			if(right->kind != Type_int && right->kind != Type_longint) {
				failed++; 
				fprintf(stderr, "ERR | Right expr of setlength has to be an integer type.\n");
			}
			set_symvalue(stmt->u.setlength.name, Env_FuncEntry(
				Type_TyList(left, NULL), 
				Type_Void()));
			return Type_Void();
			break;
		default:
			fprintf(stderr, "ERR | Valid kind of stmt AST node expected.\n");
			exit(1);
			break;
	}
}

static Type_ty check_expr(Sym_tab tab, AST_expr expr){
	Type_ty left, right, lo, hi, rand, chr, ord, le, lowv, highv;
	AST_expr lo_e, hi_e;
	AST_op op;
	AST_var v;
	Env_entry func_entry;
	
	switch(expr->kind) {
		case AST_opExp:
			left = check_expr(tab, expr->u.op.left);
			op = expr->u.op.op;
			right = check_expr(tab, expr->u.op.right);
			/* Arithmetic Operators +, -, *, /, div or string concat */
			if(0 <= op && op < 5) {
				if((left->kind == Type_string) && (right->kind == Type_string || right->kind == Type_char)) {
					return Type_String(); // special case of string concatenation
				} else {
					if(left->kind != Type_int && left->kind != Type_longint && left->kind != Type_real) {
						failed++;
						fprintf(stderr, "ERR | Arithmetic op expr(%d) requires num type as l-expr.\n", op);
					}
					if(right->kind != Type_int && right->kind != Type_longint && right->kind != Type_real) {
						failed++; 
						fprintf(stderr, "ERR | Arithmetic op expr(%d) requires num type as r-expr.\n", op);
					}
				}
				if(op == 4) { // int division 
					if(left->kind != Type_longint && left->kind != Type_int){
						failed++;
						fprintf(stderr, "ERR | Numerical division only possible with integer expr.\n");
					}
					if(right->kind != Type_longint && right->kind != Type_int){
						failed++;
						fprintf(stderr, "ERR | Numerical division only possible with integer expr.\n");
					}
					if(left->kind == Type_longint || right->kind == Type_longint){
						return Type_Longint();
					} else {
						return Type_Int();
					}
				} else { // +, -, *, / check types and return correct type
					if(left->kind == Type_longint || right->kind == Type_longint){
						return Type_Longint();
					} else if (left->kind == Type_real || right->kind == Type_real){
						return Type_Real();
					} else {
						return Type_Int();
					}
				}
			}
			/* Relational Operators: =, >, <, >=, <= */
			else if(op >= 5 && op <= 9) {
				if(left->kind != Type_int && left->kind != Type_longint && left->kind != Type_real) {
					failed++;
					fprintf(stderr, "ERR | Rel op expr(%d) requires num type as l-expr.\n", op);
				}
				if(right->kind != Type_int && right->kind != Type_longint && right->kind != Type_real) {
					failed++;
					fprintf(stderr, "ERR | Rel op expr(%d) requires num type as r-expr.\n", op);
				}
				return Type_Bool();
			}
			/* Boolean Operators: and, or */
			else if(op == 10 || op == 11) {
				if(left->kind != Type_bool || right->kind != Type_bool) {
					failed++;
					fprintf(stderr, "ERR | Bool op expr(%d), requires bool types as exprs.\n", op);
				}
				return Type_Bool();
			}
			/* Set Operator: in */
			else if(op == 12) {	
				if(right->kind == Type_set) {
					return Type_Bool();
				} else {
					failed++; fprintf(stderr, "ERR | In op expr requires r-expr to be a set var.\n");
				}
				return Type_Bool();
			}			
			break;
		case AST_varExp:
			return check_var(tab, expr->u.var);
			break;
		case AST_negExp:
			return check_expr(tab, expr->u.expr);
			break;
		case AST_intExp:
			return Type_Int();
			break;
		case AST_realExp:
			return Type_Real();
			break;
		case AST_stringExp:
			return Type_String();
			break;
		case AST_charExp:
			return Type_Char();
			break;
		case AST_boolExp:
			return Type_Bool();
			break;
		case AST_setExp:
			lo = check_expr(tab, expr->u.set.lo);
			hi = check_expr(tab, expr->u.set.hi);
			if(lo->kind != hi->kind) {
				failed++;
				fprintf(stderr, "ERR | Low & high of set expr have to be of same type.\n");
			}
			lo_e = expr->u.set.lo;
			hi_e = expr->u.set.hi;
			if(lo_e->kind == AST_charExp && lo_e->kind == AST_charExp){
				if(lo_e->u.chr && hi_e->u.chr && lo_e->u.chr < hi_e->u.chr){
					return lo;
				} else {
					failed++;
					fprintf(stderr, "ERR | Low has to be of lower value than hi expr.\n");
				}
			}
			return Type_Void();
			break;
		case AST_uFuncExp:
			func_entry = get_symvalue(global_lookup(expr->u.fcall.func));
			if(func_entry && func_entry->kind == Env_funcEntry) {
				/* Check if arguments match to params */
				if(check_args(tab, expr->u.fcall.args, 
					func_entry->u.func.fparams, expr) == 1){
					return actual_type(func_entry->u.func.result);
				} else {
					if(!func_entry){
						failed++;
						fprintf(stderr, "ERR | Called a function '%s' that does not exist.\n", 
							get_sym(expr->u.fcall.func));
					}
				}
			}
			return Type_Void();
			break;
		case AST_ordExp:
			ord = check_var(tab, expr->u.varfunc.var);
			if(ordinal_type(ord) == NULL) {
				failed++;
				fprintf(stderr, "ERR | Ord needs an ordinal type variable.\n");
			}
			set_symvalue(expr->u.varfunc.name, 
				Env_FuncEntry(Type_TyList(ord, NULL), Type_Int()));
			return Type_Int();
			break;
		case AST_chrExp:
			chr = check_expr(tab, expr->u.exprfunc.expr);
			if(ordinal_type(chr) == NULL) {
				failed++;
				fprintf(stderr, "ERR | Chr needs an ordinal type expression.\n");
			}
			set_symvalue(expr->u.exprfunc.name, 
				Env_FuncEntry(Type_TyList(chr, NULL), Type_Char()));
			return Type_Char();
			break;
		case AST_lengthExp:
			le = check_expr(tab, expr->u.exprfunc.expr);
			if(actual_type(le)->kind != Type_string && actual_type(le)->kind != Type_arr) {
				failed++;
				fprintf(stderr, "ERR | Length needs an array or string type expression.\n");
			}
			return Type_Int();
			break;
		case AST_lowExp:
			lowv = check_var(tab, expr->u.varfunc.var);
			if(actual_type(lowv)->kind != Type_arr) {
				failed++;
				fprintf(stderr, "ERR | Low needs an array type expression as arg.\n");
				return Type_Int();
			}
			return actual_type(lowv->u.ty);
			break;
		case AST_highExp:
			highv = check_var(tab, expr->u.varfunc.var);
			if(actual_type(highv)->kind != Type_arr) {
				failed++;
				fprintf(stderr, "ERR | High needs an array type expression as arg.\n");
				return Type_Int();
			}
			return actual_type(highv->u.ty);
			return Type_Int();
			break;
		case AST_randomExp:
			rand = check_expr(tab, expr->u.exprfunc.expr);
			if(actual_type(rand)->kind != Type_int && actual_type(rand)->kind != Type_longint) {
				failed++;
				fprintf(stderr, "ERR | Expected int/longint type for random expr call.\n"); 
				return Type_Void();
			}
			set_symvalue(expr->u.exprfunc.name, 
				Env_FuncEntry(Type_TyList(actual_type(rand), NULL), actual_type(rand)));
			return actual_type(rand);
			break;
		default:
			fprintf(stderr, "ERR | Valid kind of expr AST node expected.\n");
			exit(1);
			break;
	}
}

/* As types, vars and functions are declared, type checker augments the 
 * environments, they are consulted for each identifier that is found 
 * during processing of expressionss (type checking, translation, ..) */

/* Saves the symbols into the current symbol table */
static void check_decl(Sym_tab tab, AST_decl decl) {
	if(decl){
		AST_ty_decl_l tdec_l;
		AST_var_decl_l tvar_l;
		AST_ident_l id_l;
		Type_ty result_ty, name_ty;
		Symbol sym;
		int cycle;
		
		switch(decl->kind){
			case AST_typeDecl:	
				for(tdec_l = decl->u.type; tdec_l; tdec_l = tdec_l->tail) {
					if(local_lookup(tab, tdec_l->head->name) == NULL) {
						/* Set value of symbol to name type with NULL type first */
						set_symvalue(tdec_l->head->name, Type_Name(tdec_l->head->name, NULL));
						/* Insert sym into current scope */
						name_ty = get_symvalue(tdec_l->head->name);
						fprintf(stderr, "INFO| Insert TYPE: %s | type: %s, to scope (%d).\n", 
							get_sym(tdec_l->head->name), str_ty[name_ty->kind], curr_scope_level);
						insert_sym(tab, tdec_l->head->name);
					} else {
						failed++;
						fprintf(stderr, "ERR | Multiple declared type for symbol: '%s'.\n", 
							get_sym(tdec_l->head->name));
					}
				}
				cycle = 1;
				for(tdec_l = decl->u.type; tdec_l; tdec_l = tdec_l->tail) {
					/* Get type of current named type node */
					result_ty = check_ty(NULL, tdec_l->head->ty);
					if(cycle == 1) {
						if (result_ty->kind != Type_name)  { /* got real type? */
							cycle = 0;
						}
					}
					sym = local_lookup(tab, tdec_l->head->name);
					set_symvalue(sym, result_ty); /* set real type of name type symbol here */
					fprintf(stderr, "INFO| Setting real result type: '%s' for sym: '%s'.\n", 
						str_ty[result_ty->kind], get_sym(sym));
				}
				if(cycle == 1) { failed++; fprintf(stderr, "ERR | Illegal type cycle.\n"); }
				break;
			case AST_varDecl:
				for(tvar_l = decl->u.var; tvar_l; tvar_l = tvar_l->tail) {
					result_ty = check_ty(NULL, tvar_l->head->ty);
					if(!result_ty) {
						failed++;
						fprintf(stderr, "ERR | Type of var to be declared is undefined.\n");
					} else {
						for(id_l = tvar_l->head->idents; id_l; id_l = id_l->tail) {
							if(local_lookup(tab, id_l->sym) == NULL) {
								//fprintf(stderr, "CURRENT SCOPE LEVEL FOR LOCAL LOOKUP: %d\n", curr_scope_level);
								set_symvalue(id_l->sym, Env_VarEntry(result_ty));
								fprintf(stderr, "INFO| Insert VAR: %s | type: %s to scope (%d).\n",
									get_sym(id_l->sym), str_ty[result_ty->kind], curr_scope_level);
								insert_sym(tab, id_l->sym);
							} else {
								failed++;
								fprintf(stderr, "ERR | Multiple declared variable for symbol: '%s'.\n", 
									get_sym(id_l->sym));
							}
						}
					}
				}
				break;
			default:
				fprintf(stderr, "ERR | Valid kind of decl AST node expected.\n");
				exit(1);
				break;
		}
	}
}

/* Saves func symbols into the current symbol table and inits a new table 
 * as a new scope for each function declared */
static void check_funcdecl(Sym_tab tab, AST_func_decl_l fdecll) {
	if(fdecll){	
		int final;
		AST_func_decl fcd;
		AST_func_decl_l fdl;
		AST_fparam_l fpl;
		AST_ident_l il;
		Type_ty result_ty;
		Type_ty_l formal_tys, curr_tyl;
		
		/* This part saves all symbols for enclosing scope */
		for(fdl = fdecll; fdl; fdl = fdl->tail){ // loop through all func declars
			// get result type symbol and check if defined
			result_ty = get_symvalue(global_lookup(get_ty_sym(fdl->head->result)));
			if(!result_ty) {
				fprintf(stderr, "ERR | Undefined type for function's return type.\n");
				failed++; result_ty = Type_Void();
			}
			if(local_lookup(tab, fdl->head->name) == NULL){
				// Go through all formal parameters and create a type list for curr scope
				formal_tys = create_tylist(tab, fdl->head->params);
				// Set symbol value to Function Entry with types of all formal params and result type
				set_symvalue(fdl->head->name, Env_FuncEntry(formal_tys, result_ty));
				insert_sym(tab, fdl->head->name); // insert function symbol to curr scope
				fprintf(stderr, "INFO| Insert FUNC: %s | type: %s to scope (%d).\n",
					get_sym(fdl->head->name), str_ty[result_ty->kind], curr_scope_level);
			} else {
				failed++;
				fprintf(stderr, "ERR | Multiple declared variable for symbol: '%s'.\n", 
					get_sym(fdl->head->name));
			}
		}
		/* This part introduces a new scope which lies within the function */
		for(fdl = fdecll; fdl; fdl = fdl->tail) { // again loop through all func declars
			fcd = fdl->head; // get function	
			curr_scope_level++; // increment scope level and create new table
			Sym_tab new_tab = create_tab(curr_scope_level);
			curr_scope = begin_scope(new_tab); // push new table on the stack and set curr scope
			fprintf(stderr, "----------------------------\n");
			fprintf(stderr, "| ENTERED SCOPE LEVEL: (%d) |\n", curr_scope_level);
			fprintf(stderr, "----------------------------\n");
			formal_tys = create_tylist(tab, fcd->params);
			// insert variables from formal params into new table
			curr_tyl = formal_tys;
			for(fpl = fcd->params; fpl; fpl=fpl->tail){
				for(il = fpl->idents; il; il=il->tail){
					if(local_lookup(curr_scope, il->sym) == NULL){
						//fprintf(stderr, "CURR SCOPE LEVEL FOR LOCAL LOOKUP: %d\n", curr_scope_level);
						set_symvalue(il->sym, Env_VarEntry(curr_tyl->head));
						insert_sym(curr_scope, il->sym);
						fprintf(stderr, "INFO| Insert (func) VAR: %s | type: %s to scope (%d).\n",
							get_sym(il->sym), str_ty[curr_tyl->head->kind], curr_scope_level);
					} else {
						failed++;
						fprintf(stderr, "ERR | Multiple declared func-var for symbol: '%s'.\n", 
							get_sym(il->sym));
					}
					curr_tyl = curr_tyl->tail;
				}
			}
			/* Now eval body expression */
			check_block(fcd, 1); // check entire block inside function
			/* TO-DO???: Get resulting expression type from body and compare to
			 * func = local_lookup(curr_scope, fcd->name);
			 * to see if return type is correct for func body. */	 
			fprintf(stderr, "----------------------------\n");
			fprintf(stderr, "| LEAVING SCOPE LEVEL: (%d) |\n", curr_scope_level);
			fprintf(stderr, "----------------------------\n");
			curr_scope = end_scope(curr_scope); // end scope after func handled
			curr_scope_level--;		
		}
	}
}

/* Returns Type_ty for each Type AST node after symbol lookup, 
 * if tab not specified, we lookup the global scope, else local lookup. */
static Type_ty check_ty(Sym_tab tab, AST_type ty){
	Type_ty res = NULL;
	Symbol sym;
	switch(ty->kind) {
		case AST_simpleType:
			sym = global_lookup(ty->u.simple);
			if(!sym) {
				fprintf(stderr, "ERR | Simple type \"%s\" is undefined.\n", 
					get_sym(ty->u.simple));
				failed++; return Type_Int();
			}
			res = get_symvalue(sym);
			return res;
			break;
		case AST_arrayType:
			sym = global_lookup(ty->u.arr.arr);
			if(!sym) {
				fprintf(stderr, "ERR | Array type \"%s\" is undefined.\n", 
					get_sym(ty->u.arr.arr));
				failed++; return Type_Int();
			}
			res = get_symvalue(sym);
			return Type_Array(res);
			break;
		case AST_setType:
			sym = global_lookup(ty->u.set);
			if(!sym) {
				fprintf(stderr, "ERR | Set type \"%s\" is undefined.\n", 
					get_sym(ty->u.set));
				failed++;; return Type_Int();
			}
			res = get_symvalue(sym);
			return Type_Set(res);
			break;
		case AST_namedType:
			sym = global_lookup(ty->u.simple);
			if(!sym) {
				fprintf(stderr, "ERR | Name type \"%s\" is undefined.\n", 
					get_sym(ty->u.simple));
				failed++; return Type_Int();
			}
			res = get_symvalue(sym);
			return res;
			break;
		default:
			fprintf(stderr, "ERR | Valid kind of type AST node expected.\n");
			exit(1);
			break;
	}
}
/* go through all formal parameters and return types of identifiers */
static Type_ty_l create_tylist(Sym_tab tab, AST_fparam_l fpl){
	Type_ty_l final = NULL;
	Type_ty_l head = final;
	AST_ident_l id_l;
	Type_ty ty;
	
	for(; fpl; fpl = fpl->tail){
		// lookup type symbol for each formal parameter
		ty = check_ty(tab, fpl->ty);	
		// create type entries for each identifier inside a formal parameter
		for(id_l = fpl->idents; id_l; id_l = id_l->tail){
			if(!ty) {
				failed++; ty = Type_Int();
				fprintf(stderr, "ERR | Undefined type for formal parameter.\n");
			}
			/* fprintf(stderr, "INFO| Creating formalp type: Identifier: %s with type %s.\n",
				 get_sym(id_l->sym), str_ty[ty->kind]); */
			if(!final) {
				final = Type_TyList(ty, NULL);
				head = final;
			} else {
				final->tail = Type_TyList(ty, NULL);
				final = final->tail;
			}
		}
	}
	return head;
}

/* match types of a func call with expected arguments of that function. */
static int check_args(Sym_tab tab, AST_expr_l expl, Type_ty_l tyl, AST_expr func) {
	AST_expr_l el = expl;
	Type_ty_l tl = tyl;
	Type_ty expr;
	AST_expr fu = func;
	int el_c = 0; int tl_c = 0;

	while(el->tail){
		el_c++;
		el=el->tail;
	} if (!el->head) el_c++;
	
	while(tl->tail){
		tl_c++;
		tl=tl->tail;
	} if (!tl->head) tl_c++;
	
	if(el_c > tl_c) {
		fprintf(stderr, "ERR | Too many expr (%d) for this func-call, expected: %d.\n", 
			el_c, tl_c);
		failed++; return 0;
	}
	if(tl_c > el_c) {
		fprintf(stderr, "ERR | Too few expr (%d) for this func-call, expected: %d.\n", 
			el_c, tl_c);
		failed++; return 0;
	}
	
	while(el && tl) {
		expr = check_expr(tab, el->head);
		if (match_type(expr, tl->head) == 0) {
			fprintf(stderr, "ERR | Unmatched parameter in function call (%s).\n", 
				get_sym(func->u.fcall.func));
			failed++; return 0;
		}
		el = el->tail; tl = tl->tail;
	}
	return 1;
}

/* match actual type of two types passed. */
static int match_type(Type_ty a, Type_ty b){
	if(actual_type(a) == actual_type(b)){
		return 1;
	}
	else {
		/* Longint / int grouped to integer type */
		if(actual_type(a)->kind == Type_longint && actual_type(b)->kind == Type_int){
			return 1;
		}
		else if(actual_type(b)->kind == Type_longint && actual_type(a)->kind == Type_int){
			return 1;
		} else {
			return 0;
		}
	}
}

/* returns the type symbol from a type node */
static Symbol get_ty_sym(AST_type tynode){
	Symbol sym;
	
	switch(tynode->kind){
		case AST_simpleType:
			sym = tynode->u.simple;
			break;
		case AST_namedType:
			sym = tynode->u.simple;
			break;
		case AST_arrayType:
			sym = tynode->u.arr.arr;
			break;
		case AST_setType:
			sym = tynode->u.set;
			break;
		default:
			fprintf(stderr, "ERR | Valid kind of type AST node expected.\n");
			exit(1);
			break;
	}
	return sym;
}

/* get the actual type of a type, if its a named type we call this recursively. */
static Type_ty actual_type(Type_ty ty){
	if (!ty) 
		return ty;
	if (ty->kind == Type_name) 
		actual_type(ty->u.name.ty);
	else 
		return ty;
}

/* check if given type is ordinal -> type: countable and ordable */
static Type_ty ordinal_type(Type_ty ty){
	if (!ty)
		return ty;
	if(ty->kind == Type_name)
		ordinal_type(ty->u.name.ty);
	if(ty->kind != Type_real && ty->kind != Type_string)
		return ty;
	else return NULL;
}

/* returns 1 if check passed, 0 if failed */
static int check_ioproc(Type_ty ty){
	if (!ty) return 0;
	if(ty->kind == Type_arr) return 0;
	if(ty->kind == Type_set) return 0;
	if(ty->kind == Type_bool) return 0;
	if(ty->kind == Type_void) return 0;
	return 1;
}

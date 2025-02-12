#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ast_structs.h"
#include "symtab.h"
#include "semantic.h"
#include "utility.h"
#include <string.h>


static FILE* filehandler();
static void close_file(FILE* fp);
static void codegen_expr(AST_expr exp);
static void codegen_decl(AST_decl decl, int scope, int clear);
static void codegen_funcdecl(AST_func_decl_l fdecll);
static void codegen_stmt(AST_stmt stmt);
static void codegen_var(AST_var var);
static void codegen_type(AST_type type);
static void codegen_block(AST_block block, int scope);
static char* get_alignment(Symbol sym, int init);
static void set_ty_size(Symbol sym);
static void set_var_size(Symbol sym);
static char *get_ty_size(Symbol sym);
static char *get_ty_exp(AST_expr exp);
static char* get_alignment_exp(AST_expr expr);
static int check_prim(AST_expr expr);
static void load_gvar(Symbol sym);


static const char* str_size_ty[] = {
	"i32", "i64", "double", "i8*", "i8", "i8", "void" };

static int stack_counter = 0;
static int label_counter = 0;
static char* lastexptype;


static void codegen_expr(AST_expr exp)
{
    int len = 0; int lctr;
	Symbol fsym, argsym, fcsym;
	AST_expr_l expl;
	Env_entry fe;
	Type_ty fe_ty;

    switch (exp->kind) {
        case AST_opExp:
            switch (exp->u.op.op) {
                case AST_plusOp:
                if (exp->u.op.left->kind == AST_opExp) {
                    codegen_expr(exp->u.op.left);
                }
                if (exp->u.op.right->kind == AST_opExp) {
                    codegen_expr(exp->u.op.right);
                }
                //add both Expressions
                stack_counter++;
                fprintf(stderr, "%s%d%s","%",stack_counter," = add nsw ");
                fprintf(filehandler(), "%s%d%s","%",stack_counter," = add nsw ");
                if (exp->u.op.right->kind == AST_varExp) {
                    lastexptype = get_ty_exp(exp->u.op.right);
                }else if (check_prim(exp->u.op.right) == 1) {
                    lastexptype = get_ty_exp(exp->u.op.right);
                }
                if(check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp)
                {
                    fprintf(stderr, "%s ", lastexptype);
                    fprintf(filehandler(), "%s ", lastexptype);
                    codegen_expr(exp->u.op.left);
                }
                else if(check_prim(exp->u.op.right) == 1)
                {
                    fprintf(stderr, "%s ", lastexptype);
                    fprintf(filehandler(), "%s ", lastexptype);
                    fprintf(stderr, "%s%d","%", stack_counter-1);
                    fprintf(filehandler(), "%s%d","%", stack_counter-1);
                }
                else if(check_prim(exp->u.op.right) != 1)
                {
                    fprintf(stderr, "%s ", lastexptype);
                    fprintf(filehandler(), "%s ", lastexptype);
                    fprintf(stderr, "%s%d","%", stack_counter-2);
                    fprintf(filehandler(), "%s%d","%", stack_counter-2);
                }
                fprintf(stderr, "%s", ", ");
                fprintf(filehandler(), "%s", ", ");
                if(check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp)
                {
                    codegen_expr(exp->u.op.right);
                    fprintf(stderr, "\n");
                    fprintf(filehandler(), "\n");
                }
                else
                {
                    fprintf(stderr, "%s%d\n","%", stack_counter-1 );
                    fprintf(filehandler(), "%s%d\n","%", stack_counter-1 );
                }
                break;
                case AST_minusOp:
                    if (exp->u.op.left->kind == AST_opExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    //sub both Expressions
                    stack_counter++;
                    fprintf(stderr, "%s%d%s","%",stack_counter," = sub nsw ");
                    fprintf(filehandler(), "%s%d%s","%",stack_counter," = sub nsw ");
                    if (exp->u.op.right->kind == AST_varExp) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }else if (check_prim(exp->u.op.right) == 1) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    if(check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        codegen_expr(exp->u.op.left);
                    }
                    else if(check_prim(exp->u.op.right) == 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-1);
                        fprintf(filehandler(), "%s%d","%", stack_counter-1);
                    }
                    else if(check_prim(exp->u.op.right) != 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-2);
                        fprintf(filehandler(), "%s%d","%", stack_counter-2);
                    }
                    fprintf(stderr, "%s", ", ");
                    fprintf(filehandler(), "%s", ", ");
                    if(check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp)
                    {
                        codegen_expr(exp->u.op.right);
                        fprintf(stderr, "\n");
                        fprintf(filehandler(), "\n");
                    }
                    else
                    {
                        fprintf(stderr, "%s%d\n","%", stack_counter-1 );
                        fprintf(filehandler(), "%s%d\n","%", stack_counter-1 );
                    }
                    break;
                case AST_mulOp:
                    if (exp->u.op.left->kind == AST_opExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    //mul both Expressions
                    stack_counter++;
                    fprintf(stderr, "%s%d%s","%",stack_counter," = mul nsw ");
                    fprintf(filehandler(), "%s%d%s","%",stack_counter," = mul nsw ");
                    if (exp->u.op.right->kind == AST_varExp) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }else if (check_prim(exp->u.op.right) == 1) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    if(check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        codegen_expr(exp->u.op.left);
                    }
                    else if(check_prim(exp->u.op.right) == 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-1);
                        fprintf(filehandler(), "%s%d","%", stack_counter-1);
                    }
                    else if(check_prim(exp->u.op.right) != 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-2);
                        fprintf(filehandler(), "%s%d","%", stack_counter-2);
                    }
                    fprintf(stderr, "%s", ", ");
                    fprintf(filehandler(), "%s", ", ");
                    if(check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp)
                    {
                        codegen_expr(exp->u.op.right);
                        fprintf(stderr, "\n");
                        fprintf(filehandler(), "\n");
                    }
                    else
                    {
                        fprintf(stderr, "%s%d\n","%", stack_counter-1 );
                        fprintf(filehandler(), "%s%d\n","%", stack_counter-1 );
                    }
                    break;
                case AST_divOp:
                    if (exp->u.op.left->kind == AST_opExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    //div both Expressions
                    stack_counter++;
                    fprintf(stderr, "%s%d%s","%",stack_counter," = div nsw ");
                    fprintf(filehandler(), "%s%d%s","%",stack_counter," = div nsw ");
                    if (exp->u.op.right->kind == AST_varExp) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }else if (check_prim(exp->u.op.right) == 1) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    if(check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        codegen_expr(exp->u.op.left);
                    }
                    else if(check_prim(exp->u.op.right) == 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-1);
                        fprintf(filehandler(), "%s%d","%", stack_counter-1);
                    }
                    else if(check_prim(exp->u.op.right) != 1)
                    {
                        fprintf(stderr, "%s ", lastexptype);
                        fprintf(filehandler(), "%s ", lastexptype);
                        fprintf(stderr, "%s%d","%", stack_counter-2);
                        fprintf(filehandler(), "%s%d","%", stack_counter-2);
                    }
                    fprintf(stderr, "%s", ", ");
                    fprintf(filehandler(), "%s", ", ");
                    if(check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp)
                    {
                        codegen_expr(exp->u.op.right);
                        fprintf(stderr, "\n");
                        fprintf(filehandler(), "\n");
                    }
                    else
                    {
                        fprintf(stderr, "%s%d\n","%", stack_counter-1 );
                        fprintf(filehandler(), "%s%d\n","%", stack_counter-1 );
                    }
                    break;
                case AST_intDivOp:
                    // NOT IMPLEMENTED
                    fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
                    break;
                case AST_eqOp:
                    if (exp->u.op.left->kind == AST_uFuncExp || exp->u.op.left->kind == AST_opExp ) {
                        codegen_expr(exp->u.op.left);
                        lctr = stack_counter;
                    }
                    if (exp->u.op.right->kind == AST_uFuncExp || exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    stack_counter++;
                    if (lastexptype == NULL) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    fprintf(stderr, "%s%d%s%s ", "%", stack_counter, " = icmp eq ", lastexptype);
                    fprintf(filehandler(), "%s%d%s%s ", "%", stack_counter, " = icmp eq ",lastexptype );
                    if (check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (check_prim(exp->u.op.left) != 1 && check_prim(exp->u.op.right) != 1 && exp->u.op.left->kind != AST_varExp && exp->u.op.right->kind != AST_varExp)
                    {
                        fprintf(stderr, "%s%d","%",lctr);
                        fprintf(filehandler(), "%s%d","%",lctr );
                    }

                    fprintf(stderr, "%s", ", ");
                    if (check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    else if (check_prim(exp->u.op.right) == 0 || exp->u.op.right->kind != AST_varExp) {
                        fprintf(stderr, "%s%d","%",stack_counter-1 );
                        fprintf(filehandler(), "%s%d","%",stack_counter-1 );
                    }
                    fprintf(stderr, "\n");
                    break;
                case AST_ltOp:
                    if (exp->u.op.left->kind == AST_uFuncExp || exp->u.op.left->kind == AST_opExp ) {
                        codegen_expr(exp->u.op.left);
                        lctr = stack_counter;
                    }
                    if (exp->u.op.right->kind == AST_uFuncExp || exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    stack_counter++;
                    if (lastexptype == NULL) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    fprintf(stderr, "%s%d%s%s ", "%", stack_counter, " = icmp slt ", lastexptype);
                    fprintf(filehandler(), "%s%d%s%s ", "%", stack_counter, " = icmp slt ",lastexptype );
                    if (check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (check_prim(exp->u.op.left) != 1 && check_prim(exp->u.op.right) != 1 && exp->u.op.left->kind != AST_varExp && exp->u.op.right->kind != AST_varExp)
                    {
                        fprintf(stderr, "%s%d","%",lctr);
                        fprintf(filehandler(), "%s%d","%",lctr );
                    }

                    fprintf(stderr, "%s", ", ");
                    if (check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    else if (check_prim(exp->u.op.right) == 0 || exp->u.op.right->kind != AST_varExp) {
                        fprintf(stderr, "%s%d","%",stack_counter-1 );
                        fprintf(filehandler(), "%s%d","%",stack_counter-1 );
                    }
                    fprintf(stderr, "\n");
                    break;
                case AST_lteqOp:
                    if (exp->u.op.left->kind == AST_uFuncExp || exp->u.op.left->kind == AST_opExp ) {
                        codegen_expr(exp->u.op.left);
                        lctr = stack_counter;
                    }
                    if (exp->u.op.right->kind == AST_uFuncExp || exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    stack_counter++;
                    if (lastexptype == NULL) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    fprintf(stderr, "%s%d%s%s ", "%", stack_counter, " = icmp sle ", lastexptype);
                    fprintf(filehandler(), "%s%d%s%s ", "%", stack_counter, " = icmp sle ",lastexptype );
                    if (check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (check_prim(exp->u.op.left) != 1 && check_prim(exp->u.op.right) != 1 && exp->u.op.left->kind != AST_varExp && exp->u.op.right->kind != AST_varExp)
                    {
                        fprintf(stderr, "%s%d","%",lctr);
                        fprintf(filehandler(), "%s%d","%",lctr );
                    }

                    fprintf(stderr, "%s", ", ");
                    if (check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    else if (check_prim(exp->u.op.right) == 0 || exp->u.op.right->kind != AST_varExp) {
                        fprintf(stderr, "%s%d","%",stack_counter-1 );
                        fprintf(filehandler(), "%s%d","%",stack_counter-1 );
                    }
                    fprintf(stderr, "\n");
                    break;
                case AST_gtOp:
                    if (exp->u.op.left->kind == AST_uFuncExp || exp->u.op.left->kind == AST_opExp ) {
                        codegen_expr(exp->u.op.left);
                        lctr = stack_counter;
                    }
                    if (exp->u.op.right->kind == AST_uFuncExp || exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    stack_counter++;
                    if (lastexptype == NULL) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    fprintf(stderr, "%s%d%s%s ", "%", stack_counter, " = icmp sgt ", lastexptype);
                    fprintf(filehandler(), "%s%d%s%s ", "%", stack_counter, " = icmp sgt ",lastexptype );
                    if (check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (check_prim(exp->u.op.left) != 1 && check_prim(exp->u.op.right) != 1 && exp->u.op.left->kind != AST_varExp && exp->u.op.right->kind != AST_varExp)
                    {
                        fprintf(stderr, "%s%d","%",lctr);
                        fprintf(filehandler(), "%s%d","%",lctr );
                    }

                    fprintf(stderr, "%s", ", ");
                    if (check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    else if (check_prim(exp->u.op.right) == 0 || exp->u.op.right->kind != AST_varExp) {
                        fprintf(stderr, "%s%d","%",stack_counter-1 );
                        fprintf(filehandler(), "%s%d","%",stack_counter-1 );
                    }
                    fprintf(stderr, "\n");
                    break;
                case AST_gteqOp:
                    if (exp->u.op.left->kind == AST_uFuncExp || exp->u.op.left->kind == AST_opExp ) {
                        codegen_expr(exp->u.op.left);
                        lctr = stack_counter;
                    }
                    if (exp->u.op.right->kind == AST_uFuncExp || exp->u.op.right->kind == AST_opExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    stack_counter++;
                    if (lastexptype == NULL) {
                        lastexptype = get_ty_exp(exp->u.op.right);
                    }
                    fprintf(stderr, "%s%d%s%s ", "%", stack_counter, " = icmp sge ", lastexptype);
                    fprintf(filehandler(), "%s%d%s%s ", "%", stack_counter, " = icmp sge ",lastexptype );
                    if (check_prim(exp->u.op.left) == 1 || exp->u.op.left->kind == AST_varExp) {
                        codegen_expr(exp->u.op.left);
                    }
                    if (check_prim(exp->u.op.left) != 1 && check_prim(exp->u.op.right) != 1 && exp->u.op.left->kind != AST_varExp && exp->u.op.right->kind != AST_varExp)
                    {
                        fprintf(stderr, "%s%d","%",lctr);
                        fprintf(filehandler(), "%s%d","%",lctr );
                    }

                    fprintf(stderr, "%s", ", ");
                    if (check_prim(exp->u.op.right) == 1 || exp->u.op.right->kind == AST_varExp) {
                        codegen_expr(exp->u.op.right);
                    }
                    else if (check_prim(exp->u.op.right) == 0 || exp->u.op.right->kind != AST_varExp) {
                        fprintf(stderr, "%s%d","%",stack_counter-1 );
                        fprintf(filehandler(), "%s%d","%",stack_counter-1 );
                    }
                    fprintf(stderr, "\n");
                    break;
                case AST_andOp:
                    // NOT IMPLEMENTED
                    fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
                    break;
                case AST_orOp:
					// NOT IMPLEMENTED
                    fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
					break;
                case AST_inOp:
                    // NOT IMPLEMENTED
                    fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
                    break;
            }
            break;
        case AST_varExp:
            codegen_var(exp->u.var);
            break;
        case AST_negExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_intExp:
			fprintf(stderr, "%d", exp->u.integer);
            fprintf(filehandler(), "%d", exp->u.integer);
            break;
        case AST_realExp:
			fprintf(stderr, "%f", exp->u.real);
            fprintf(filehandler(), "%f", exp->u.real);
            break;
        case AST_stringExp:
            len = strlen(exp->u.str);
            fprintf(stderr, "%s%d%s%d%s", "getelementptr inbounds ([",
				len, " x i8], [",len," x i8]*");
            fprintf(filehandler(), "%s%d%s%d%s", "getelementptr inbounds ([",
				len, " x i8], [",len," x i8]*");
            break;
        case AST_charExp:
            fprintf(stderr, "%c", exp->u.chr);
            fprintf(filehandler(), "%c", exp->u.chr);
            break;
        case AST_boolExp:
            fprintf(stderr, "%d", exp->u.boolean);
            fprintf(filehandler(), "%d", exp->u.boolean);
            break;
        case AST_uFuncExp:
            //%x/@a = call i32 @mul(i32 %x/@x, i32 %x/@x)
            fsym = global_lookup(exp->u.fcall.func);
            if(fsym != NULL){ // no nested functions supported
				int findex[1000] =  { -1 };
				int loopc = 0;

				expl = exp->u.fcall.args;
				while(expl->tail){
					if(expl->head != NULL){
						// save stack counter where fcall is saved to
						if(expl->head->kind == AST_uFuncExp) {
							codegen_expr(expl->head);
							findex[loopc] = stack_counter;
							loopc++; stack_counter++;
						}
						// save stack counter where gvar is loaded to
						if(expl->head->kind == AST_varExp) {
							if(get_scope(expl->head->u.var->u.entire) == 0) {
								load_gvar(expl->head->u.var->u.entire);
								findex[loopc] = stack_counter;
								loopc++;
							}
						}
					}
					expl = expl->tail;
				}
				if(expl->head != NULL){
					if(expl->head->kind == AST_uFuncExp) {
						codegen_expr(expl->head);
						findex[loopc] = stack_counter;
						loopc++; stack_counter++;
					}
					if(expl->head->kind == AST_varExp) {
							if(get_scope(expl->head->u.var->u.entire) == 0) {
								load_gvar(expl->head->u.var->u.entire);
								findex[loopc] = stack_counter;
								loopc++;
							}
					}
				}
				stack_counter++;
				fprintf(stderr, "%s%d %s %s %s %s%s%s", "%", stack_counter, "=",
					"call", get_ty_size(fsym), "@", get_sym(fsym), "(");
				fprintf(filehandler(), "%s %s %s%s%s", "call", get_ty_size(fsym),
					"@", get_sym(fsym), "(");
				expl = exp->u.fcall.args;
				loopc = 0;
				while(expl->tail){ // generate code for func args
					if(expl->head != NULL){
						if(check_prim(expl->head) == 1) {
							fprintf(stderr, "%s ", get_ty_exp(expl->head));
							fprintf(filehandler(), "%s ", ",");
							codegen_expr(expl->head);
							fprintf(stderr, "%s ", ",");
							fprintf(filehandler(), "%s ", ",");
						}
						else {
							if(expl->head->kind == AST_varExp){
								argsym = expl->head->u.var->u.entire;
								if(argsym != NULL) {
									fprintf(stderr, "%s ", get_ty_size(global_lookup(argsym)));
									fprintf(filehandler(), "%s ", get_ty_size(global_lookup(argsym)));
									if(findex[loopc] != -1) {
										fprintf(stderr, "%s%d", "%", findex[loopc]);
									} else {
										codegen_var(expl->head->u.var);
									}
									fprintf(stderr, "%s ", ",");
									fprintf(filehandler(), "%s ", ",");
									loopc++;

								}
							}
							else if(expl->head->kind == AST_uFuncExp){
								fcsym = global_lookup(expl->head->u.fcall.func);
								if(fcsym != NULL) {
									fe = (Env_entry) get_symvalue(fcsym);
									fe_ty = fe->u.func.result;
									if(findex[loopc] != -1){
										fprintf(stderr, "%s %s%d%s ", str_size_ty[fe_ty->kind],
											"%", findex[loopc], ",");
										fprintf(filehandler(), "%s %s%d%s ", str_size_ty[fe_ty->kind],
											"%", findex[loopc], ",");
									}
									loopc++;
								}
							}
							else { // only op expr left
								codegen_expr(expl->head);
								fprintf(stderr, "%s %s%d", lastexptype, "%", stack_counter);
								fprintf(filehandler(), "%s %s%d", lastexptype, "%", stack_counter);
							}
						}
					}
					expl = expl->tail;
				}
				if(expl->head != NULL) {
					if(check_prim(expl->head) == 1) {
						fprintf(stderr, "%s ", get_ty_exp(expl->head));
						fprintf(filehandler(), "%s ", get_ty_exp(expl->head));
						codegen_expr(expl->head);
					}
					else {
						if(expl->head->kind == AST_varExp){
							argsym = global_lookup(expl->head->u.var->u.entire);
							if(argsym != NULL) {
								fprintf(stderr, "%s ", get_ty_size(argsym));
								fprintf(filehandler(), "%s ", get_ty_size(argsym));
								if(findex[loopc] != -1) {
									fprintf(stderr, "%s%d", "%", findex[loopc]);
								} else {
									codegen_var(expl->head->u.var);
								}
								loopc++;
							}
						}
						else if(expl->head->kind == AST_uFuncExp){
							fcsym = global_lookup(expl->head->u.fcall.func);
							if(fcsym != NULL) {
								fe = (Env_entry) get_symvalue(fcsym);
								fe_ty = fe->u.func.result;
								if(findex[loopc] != -1){
									fprintf(stderr, "%s %s%d", str_size_ty[fe_ty->kind],
										"%", findex[loopc]);
									fprintf(filehandler(), "%s %s%d", str_size_ty[fe_ty->kind],
										"%", findex[loopc]);
								}
								loopc++;
							}
						}
						else { // only op exp left
							codegen_expr(expl->head);
							fprintf(stderr, "%s %s%d", lastexptype, "%", stack_counter);
							fprintf(filehandler(), "%s %s%d", lastexptype, "%", stack_counter);
						}
					}
				}
			}
			fprintf(stderr, "%s", ")\n"); fprintf(filehandler(), "%s", ")\n");
            break;
        case AST_setExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_ordExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_chrExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_lengthExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_lowExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_highExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_randomExp:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
    }
}

static void codegen_stmt(AST_stmt stmt)
{
	AST_stmt_seq_l sseql;
	Symbol sym;

	int if_label, else_label, end_label, while_test_label, 
		while_body_label, while_exit_label, compare_label, body_label,
			endf_label;

    switch (stmt->kind) {
        case AST_compStmt:
            sseql = stmt->u.sseq;
            while(sseql->tail) {
				codegen_stmt(sseql->head);
				sseql = sseql->tail;
			}
			if(sseql->head != NULL) {
				codegen_stmt(sseql->head);
			}
            break;
        case AST_assignStmt:
			sym = global_lookup(stmt->u.assign.var->u.entire); // lookup sym

			if(check_prim(stmt->u.assign.expr) == 1){ // is primitive ty
				fprintf(stderr, "%s %s ", "store",
					get_ty_exp(stmt->u.assign.expr));
				fprintf(filehandler(), "%s %s ", "store",
					get_ty_exp(stmt->u.assign.expr));
				codegen_expr(stmt->u.assign.expr); // get result from expression
			} else {
				if(stmt->u.assign.expr->kind == AST_varExp){ // exp is var
					load_gvar(stmt->u.assign.expr->u.var->u.entire);
					fprintf(stderr, "%s %s " ,"store", get_ty_size(sym));
					fprintf(filehandler(), "%s %s " ,"store", get_ty_size(sym));
					if(get_scope(stmt->u.assign.expr->u.var->u.entire) == 0){
						fprintf(stderr, "%s%d ", "%", stack_counter);
						fprintf(filehandler(), "%s%d ", "%", stack_counter);
					} else {
						codegen_expr(stmt->u.assign.expr);

					}
				} else if(stmt->u.assign.expr->kind == AST_uFuncExp){ // exp is fcall
					codegen_expr(stmt->u.assign.expr);
					fprintf(stderr, "%s %s %s%d", "store",
						get_ty_size(sym), "%", stack_counter);
					fprintf(filehandler(), "%s %s %s%d", "store",
						get_ty_size(sym), "%", stack_counter);
				} else { // everything else
					codegen_expr(stmt->u.assign.expr);
					fprintf(stderr, "%s %s %s%d", "store",
					lastexptype, "%", stack_counter);
					fprintf(filehandler(), "%s %s %s%d", "store",
					lastexptype, "%", stack_counter);
				}
			}
			fprintf(stderr, "%s %s%s ", ",", get_ty_size(sym), "*");
            fprintf(filehandler(), "%s %s%s ", ",", get_ty_size(sym), "*");
            codegen_var(stmt->u.assign.var);
            fprintf(stderr, "%s %s\n", ",", get_alignment(sym, 0));
            fprintf(filehandler(), "%s %s\n", ",", get_alignment(sym, 0));
			break;
        case AST_ifStmt:
            codegen_expr(stmt->u.iff.test); // bool op exp
            fprintf(stderr, "%s %s%d%s ", "br i1", "%", stack_counter, ",");
            fprintf(filehandler(), "%s %s%d%s ", "br i1", "%", stack_counter, ",");
            /* SET LABELS */
			if_label = label_counter; label_counter++;
			if(stmt->u.iff.elsee != NULL) {
				else_label = label_counter; label_counter++;
				fprintf(stderr,"%s %s%d%s ", "label", "%L", if_label, ",");
				fprintf(filehandler(),"%s %s%d%s ", "label", "%L", if_label, ",");
				fprintf(stderr,"%s %s%d", "label", "%L", else_label);
				fprintf(filehandler(),"%s %s%d", "label", "%L", else_label);
				end_label = label_counter; label_counter++;
			} else {
				end_label = label_counter; label_counter++;
				fprintf(stderr,"%s %s%d", "label", "%L", end_label);
				fprintf(filehandler(),"%s %s%d", "label", "%L", end_label);
			}

			/* IF-PART */
            fprintf(stderr, "%s%s%s%d%s%s","\n\n", "; <label>:", "L", if_label, ":", "\n");
            fprintf(filehandler(), "%s%s%s%d%s%s","\n\n", "; <label>:", "L", if_label, ":", "\n");
            codegen_stmt(stmt->u.iff.then); // generate if-part
            fprintf(stderr, "%s %s%d%s", "br label", "%L", end_label, "\n");
            fprintf(filehandler(), "%s %s%d%s", "br label", "%L", end_label, "\n");

			/* ELSE PART */
            if(stmt->u.iff.elsee != NULL){
				fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", else_label, ":", "\n");
				fprintf(filehandler(), "%s%s%s%d%s%s","\n", "; <label>:", "L", else_label, ":", "\n");
				codegen_stmt(stmt->u.iff.elsee);
				fprintf(stderr, "%s %s%d%s", "br label", "%L", end_label, "\n");
				fprintf(filehandler(), "%s %s%d%s", "br label", "%L", end_label, "\n");
			}
			fprintf(stderr, "%s%s%s%d%s%s", "\n", "; <label>:", "L", end_label, ":", "\n");
			fprintf(filehandler(), "%s%s%s%d%s%s", "\n", "; <label>:", "L", end_label, ":", "\n");
            break;
        case AST_forStmt:
            /* ASSIGN, store loop var */
            codegen_stmt(stmt->u.forr.assign);
            /* COMPARISON */
            compare_label = label_counter;
            fprintf(stderr, "%s%d\n", "br label %L", compare_label);
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", compare_label, ":", "\n");
            label_counter++;
            body_label = label_counter; label_counter++;
            endf_label = label_counter; label_counter++;
            
            // GENERATE ICMP SLT ASSIGN <= TO
            // codegen_expr(stmt->u.forr.to);
            // stmt->u.forr.assign->u.assign.var->u.entire
            /*
            if(stmt->u.forr.assign->u.assign.expr->kind == AST_varExp) {
				if(get_scope(stmt->u.forr.to->u.var->u.entire) == 0){
					fprintf(stderr, "%s%d", "%", stack_counter);
				}
				codegen_var(stmt->u.forr.assign->u.assign.expr->u.var);
			}
			else if(check_prim(stmt->u.forr.assign->u.assign.expr) == 1) {
				fprintf(stderr, "%s%d", "%", stack_counter);
			}
			else if(stmt->u.forr.assign->u.assign.expr->kind == AST_uFuncExp) {
				codegen_expr(stmt->u.forr.assign->u.assign.expr);
				fprintf(stderr, "%s%d", "%", stack_counter);
			}
			else {
				codegen_expr(stmt->u.forr.assign->u.assign.expr);
				fprintf(stderr, "%s%d", "%", stack_counter);
			}
            
            if(stmt->u.forr.to->kind == AST_varExp) {
				if(get_scope(stmt->u.forr.to->u.var->u.entire) == 0){
					load_gvar(stmt->u.forr.to->u.var->u.entire);
					fprintf(stderr, "%s%d", "%", stack_counter);
				}
				codegen_expr(stmt->u.forr.to);
			}
			else if(check_prim(stmt->u.forr.to) == 1) {
				codegen_expr(stmt->u.forr.to);
			}
			else if(stmt->u.forr.to->kind == AST_uFuncExp) {
				codegen_expr(stmt->u.forr.to);
				fprintf(stderr, "%s%d", "%", stack_counter);
			}
			else {
				codegen_expr(stmt->u.forr.to);
				fprintf(stderr, "%s%d", "%", stack_counter);
			}
            */
            fprintf(stderr, "\n%s %s%d%s %s%d%s%d%s", "br i1", "%", stack_counter, ",",
            "label %L", body_label, ", label %L", endf_label, "\n");
            
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", body_label, ":", "\n"); 
            
            /* BODY & COUNT INDEX */
            codegen_stmt(stmt->u.forr.body);
            fprintf(stderr, "%s%d\n", "br label %L", compare_label);
            
            /* END */
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", end_label, ":", "\n");
            
            break;
        case AST_whileStmt:
            while_test_label = label_counter; label_counter++;
            fprintf(stderr, "%s%d\n","br label %L", while_test_label );
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", while_test_label, ":", "\n");
            fprintf(filehandler(), "%s%d\n","br label %L", while_test_label );
            fprintf(filehandler(), "%s%s%s%d%s%s","\n", "; <label>:", "L", while_test_label, ":", "\n");
            codegen_expr(stmt->u.whilee.test);
            while_body_label = label_counter; label_counter++;
            while_exit_label = label_counter; label_counter++;
            fprintf(stderr, "%s%d%s%d%s%d\n","br i1 %",stack_counter,", label %L",while_body_label, ", %L",while_exit_label );
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", while_body_label, ":", "\n");
            fprintf(filehandler(), "%s%d%s%d%s%d\n","br i1 %",stack_counter,", label %L",while_body_label, ", %L",while_exit_label );
            fprintf(filehandler(), "%s%s%s%d%s%s","\n", "; <label>:", "L", while_body_label, ":", "\n");;
            codegen_stmt(stmt->u.whilee.body);
            fprintf(stderr, "%s%d\n", "br label %L",while_test_label);
            fprintf(stderr, "%s%s%s%d%s%s","\n", "; <label>:", "L", while_exit_label, ":", "\n");
            fprintf(filehandler(), "%s%d\n", "br label %L",while_test_label);
            fprintf(filehandler(), "%s%s%s%d%s%s","\n", "; <label>:", "L", while_exit_label, ":", "\n");
            break;
        case AST_writeStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_writelnStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_incStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_readlnStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_randomizeStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
        case AST_setlengthStmt:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
    }
}

static void codegen_decl(AST_decl decl, int scope, int clear)
{
	AST_var_decl_l vdecl;
	AST_ident_l id_l;
	Symbol sym;

	if(decl != NULL){
		switch (decl->kind) {
			case AST_varDecl:
				if(scope == 0) { // global vars
					for(vdecl = decl->u.var; vdecl; vdecl = vdecl->tail) {
						for(id_l = vdecl->head->idents; id_l; id_l = id_l->tail) {
							sym = global_lookup(id_l->sym);
							if(sym != NULL) {
								set_var_size(sym);

								fprintf(stderr, "%s%s %s %s %s %s", "@",
									get_sym(sym), "= common global",
									get_ty_size(sym), get_alignment(sym, 1), "\n");
								fprintf(filehandler(), "%s%s %s %s %s %s", "@",
									get_sym(sym), "= common global",
									get_ty_size(sym), get_alignment(sym, 1), "\n");
								}
						}
					}
				} else { // local scope for decls inside func
					if(clear == 1) { // reset local scopes
						for(vdecl = decl->u.var; vdecl; vdecl = vdecl->tail) {
							for(id_l = vdecl->head->idents; id_l; id_l = id_l->tail) {
								sym = global_lookup(id_l->sym);
								if(sym != NULL) {
									set_scope(sym, 0);
								}
							}
						}
					} else {
						for(vdecl = decl->u.var; vdecl; vdecl = vdecl->tail) {
							for(id_l = vdecl->head->idents; id_l; id_l = id_l->tail) {
								sym = global_lookup(id_l->sym);
								if(sym != NULL) {
									set_var_size(sym); set_scope(sym, 1);

									fprintf(stderr, "%s%s %s %s%s %s%s", "%", get_sym(sym),
										"= alloca", get_ty_size(sym), ",", get_alignment(sym, 0), "\n");
									fprintf(filehandler(), "%s%s %s %s%s %s%s", "%", get_sym(sym),
										"= alloca", get_ty_size(sym), ",", get_alignment(sym, 0), "\n");
								}
							}
						}
					}
				}
				break;
			case AST_typeDecl:
				// NOT IMPLEMENTED
                fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
				break;
		}
	}
}

static void codegen_funcdecl(AST_func_decl_l fdecll)
{
	if(fdecll != NULL){
		AST_func_decl fcd;
		AST_func_decl_l fdl;
		AST_fparam_l fpl;
		AST_ident_l il;
		Symbol fsym, fpsym;

		// define i32 @test(i32, i32) { ... }
		for(fdl = fdecll; fdl; fdl = fdl->tail) { // function decl
			fcd = fdl->head;
			fsym = global_lookup(fcd->name);
			if(fsym != NULL) {
				set_var_size(fsym); // set size_ty of func result
				set_scope(fsym, 1); // set scope of func result to local
				fprintf(stderr, "%s %s %s%s%s", "\ndefine",
					get_ty_size(fsym), "@", get_sym(fsym), "(");
				fprintf(filehandler(), "%s %s %s%s%s", "\ndefine",
					get_ty_size(fsym), "@", get_sym(fsym), "(");

				int pcount = 0;
				// get formal param types and ret type
				for(fpl = fcd->params; fpl; fpl=fpl->tail){ //fparams
					for(il = fpl->idents; il; il=il->tail){ //idents
						fpsym = global_lookup(il->sym);
						if(fpsym != NULL) {
							set_var_size(fpsym);

							if(pcount > 0) fprintf(stderr, ", ");
							fprintf(stderr, "%s", get_ty_size(fpsym));
							fprintf(filehandler(), "%s", get_ty_size(fpsym));
							pcount++;
						}
					}
				}
				fprintf(stderr, "%s", ") {\n");
				fprintf(filehandler(), "%s", ") {\n");

				stack_counter = 0; // reset stack counter
				// save function variable inside function
				fprintf(stderr, "%s%s %s %s%s %s%s", "%", get_sym(fsym),
					"= alloca", get_ty_size(fsym), ",",
					get_alignment(fpsym, 0), "\n");
				fprintf(filehandler(), "%s%s%s%s%s%s%s", "%", get_sym(fsym),
					"= alloca", get_ty_size(fsym), ",",
					get_alignment(fpsym, 0), "\n");
				// local alloca
				for(fpl = fcd->params; fpl; fpl=fpl->tail){
					for(il = fpl->idents; il; il=il->tail){
						fpsym = global_lookup(il->sym);
						if(fpsym != NULL) {
							set_scope(fpsym, 1); // set scope of sym to local

							fprintf(stderr, "%s%s %s %s%s %s%s", "%", get_sym(fpsym),
								"= alloca", get_ty_size(fpsym), ",",
								get_alignment(fpsym, 0), "\n");
							fprintf(filehandler(), "%s%s%s%s%s%s%s", "%", get_sym(fpsym),
								"= alloca", get_ty_size(fpsym), ",",
								get_alignment(fpsym, 0), "\n");
						}
					}
				}

				// local alloc + store of function local vars from AST
				codegen_decl(fcd->block->vars, 1, 0);

				// store
				for(fpl = fcd->params; fpl; fpl=fpl->tail){
					for(il = fpl->idents; il; il=il->tail){
						fpsym = global_lookup(il->sym);
						if(fpsym != NULL) {
							fprintf(stderr, "%s %s %s%d%s %s%s %s%s%s %s%s", "store",
								get_ty_size(fpsym), "%", stack_counter, ",", get_ty_size(fpsym),
								"*", "%", get_sym(fpsym), ",", get_alignment(fpsym, 0), "\n");
							fprintf(filehandler(), "%s %s %s%d%s %s%s %s%s%s %s%s", "store",
								get_ty_size(fpsym), "%", stack_counter, ",", get_ty_size(fpsym),
								"*", "%", get_sym(fpsym), ",", get_alignment(fpsym, 0), "\n");
							stack_counter++;
						}
					}
				}
				// process statement sequence inside function and return from function
				codegen_stmt(fcd->block->compound);
				// TO-DO: return value at end of function, save it inside global func var
				fprintf(stderr, "%s %s %s%s%s", "ret", get_ty_size(fsym), "%", get_sym(fsym), "\n");
				fprintf(filehandler(), "%s %s %s%s%s", "ret", get_ty_size(fsym), "%", get_sym(fsym), "\n");
				fprintf(stderr, "%s", "}\n"); fprintf(filehandler(), "%s", "}\n");

				// clear local scopes
				for(fpl = fcd->params; fpl; fpl=fpl->tail){
					for(il = fpl->idents; il; il=il->tail){
						fpsym = global_lookup(il->sym);
						if(fpsym != NULL && get_scope(fpsym) == 1) {
							set_scope(fpsym, 0);
						}
					}
				}
				codegen_decl(fcd->block->vars, 1, 1);
			}
		}
	}
}

static void codegen_var(AST_var var)
{
    switch (var->kind) {
        case AST_entireVar:
			if(global_lookup(var->u.entire) != NULL){
				// @ for global scope
				if(get_scope(global_lookup(var->u.entire)) == 0) {
					fprintf(stderr, "%s%s", "@",
						get_sym(global_lookup(var->u.entire)));
					fprintf(filehandler(), "%s%s ", "@",
						get_sym(global_lookup(var->u.entire)));
				} else { // # for local scope vars
					fprintf(stderr, "%s%s", "%",
						get_sym(global_lookup(var->u.entire)));
					fprintf(filehandler(), "%s%s ", "%",
						get_sym(global_lookup(var->u.entire)));
				}
			}
            break;
        case AST_indexedVar:
            // NOT IMPLEMENTED
            fprintf(stderr, "%s", "ERR - NOT IMPLEMENTED\n"); fprintf(filehandler(), "%s", "ERR - NOT IMPLEMENTED\n");
            break;
    }
}

/* load if its a global variable */
static void load_gvar(Symbol sym){
	Symbol load_sym;
	load_sym = global_lookup(sym);
	if(load_sym != NULL && (get_scope(load_sym) == 0)) {
		stack_counter++;
		fprintf(stderr, "%s%d %s %s%s %s%s %s%s%s %s%s", "%", stack_counter,
			"= load", get_ty_size(load_sym), ",", get_ty_size(load_sym),
			"*", "@", get_sym(load_sym), ",", get_alignment(load_sym, 0), "\n");
		fprintf(filehandler(), "%s%d %s %s%s %s%s %s%s%s %s%s", "%", stack_counter,
			"= load", get_ty_size(load_sym), ",", get_ty_size(load_sym),
			"*", "@", get_sym(load_sym), ",", get_alignment(load_sym, 0), "\n");
		lastexptype = cstrdup(get_ty_size(load_sym));
	}
}

// ONLY NEEDED IF WE DEAL WITH ARRAY/SET/NAMED TYPES
static void codegen_type(AST_type type)
{
    switch(type->kind){
		case AST_simpleType:
				// NOT IMPLEMENTED
			break;
		case AST_arrayType:
			if(type->u.arr.bounds != NULL) { // fixed array size
				// NOT IMPLEMENTED
			} else  // dynamic array size
			break;
		case AST_setType:
			// NOT IMPLEMENTED
			break;
		case AST_namedType:
			// NOT IMPLEMENTED
			break;
		default:
			break;
	}
}

static void codegen_block(AST_block block, int scope)
{
    //codegen_decl(block->types);
    codegen_decl(block->vars, scope, 0);
    codegen_stmt(block->compound);
    fprintf(stderr, "%s", "}\n");
    fprintf(filehandler(), "%s", "}\n");
    codegen_funcdecl(block->funcs);
}

void translate_code(AST_prog node)
{
	fprintf(stderr, "%s", "define i32 @main() {\n");
    fprintf(filehandler(), "%s", "define i32 @main() {\n");
    codegen_block(node->block, 0);
    close_file(filehandler());
}

static int check_prim(AST_expr expr){
    switch(expr->kind){
        case AST_intExp:
            return 1; break;
        case AST_realExp:
            return 1; break;
        case AST_stringExp:
            return 1; break;
        case AST_charExp:
            return 1; break;
        case AST_boolExp:
            return 1; break;
        default:
            return 0; break;
    }
}

/* Get alignment for all types if init == 1, initialize global vars */
static char* get_alignment(Symbol sym, int init) {
	if(init == 1){
		if(get_type(sym) == 32) return "0, align 4";
		else if(get_type(sym) == 64) return "0, align 8";
		else if(get_type(sym) == 2) return "0.000000e+00, align 8";
		else if(get_type(sym) == 8) return "0, align 1";
		else if(get_type(sym) == 16) return "null, align 8";
	} else {
		if(get_type(sym) == 32) return "align 4";
		else if(get_type(sym) == 64) return "align 8";
		else if(get_type(sym) == 2) return "align 8";
		else if(get_type(sym) == 8) return "align 1";
		else if(get_type(sym) == 16) return "align 8";
	}
}

static char* get_alignment_exp(AST_expr expr)
{
    switch (expr->kind) {
        case AST_intExp:
            return "align 4";
            break;
        case AST_realExp:
            return "align 8";
            break;
        case AST_stringExp:
            return "align 8";
            break;
        case AST_charExp:
            return "align 1";
            break;
        case AST_boolExp:
            return "align 1";
            break;
        case AST_varExp:
            return get_alignment(global_lookup(expr->u.var->u.entire), 0);
            break;
    }
}

/* Saves size of type into the symbol for allocation purposes
 * Mapping:
 * 32 -> i32 (integer)
 * 64 -> i64 (longint)
 * 2 -> double (real)
 * 8 -> i8 (char, bool)
 * 16 = i8* (char*) for strings
 * -1 = no type (void)
 */

static char *get_ty_size(Symbol sym) {
	if(get_type(sym) == 32) return "i32";
	else if(get_type(sym) == 64) return "i64";
	else if(get_type(sym) == 2) return "double";
	else if(get_type(sym) == 8) return "i8";
	else if(get_type(sym) == 16) return "i8*";
	else return "void";
}

static void set_ty_size(Symbol sym) {
	if(strcmp(get_sym(sym), "integer") == 0) set_type(sym, 32);
	else if(strcmp(get_sym(sym), "longint") == 0) set_type(sym, 64);
	else if(strcmp(get_sym(sym), "real") == 0) set_type(sym, 2);
	else if(strcmp(get_sym(sym), "boolean") == 0) set_type(sym, 8);
	else if(strcmp(get_sym(sym), "char") == 0) set_type(sym, 8);
	else if(strcmp(get_sym(sym), "string") == 0) set_type(sym, 16);
	else set_type(sym, -1);
}

static void set_var_size(Symbol sym) {
	Env_entry envent;
	Type_ty envty;
	envent = (Env_entry) get_symvalue(sym);
	if(envent->kind == Env_varEntry){
		if(envent->u.var.ty->kind == Type_int) set_type(sym, 32);
		else if(envent->u.var.ty->kind == Type_longint) set_type(sym, 64);
		else if(envent->u.var.ty->kind == Type_real) set_type(sym, 2);
		else if(envent->u.var.ty->kind == Type_string) set_type(sym, 16);
		else if(envent->u.var.ty->kind == Type_char) set_type(sym, 8);
		else if(envent->u.var.ty->kind == Type_bool) set_type(sym, 8);
		else set_type(sym, -1);
	}
	if(envent->kind == Env_funcEntry) {
		if(envent->u.func.result->kind == Type_int) set_type(sym, 32);
		else if(envent->u.func.result->kind == Type_longint) set_type(sym, 64);
		else if(envent->u.func.result->kind == Type_real) set_type(sym, 2);
		else if(envent->u.func.result->kind == Type_string) set_type(sym, 16);
		else if(envent->u.func.result->kind == Type_char) set_type(sym, 8);
		else if(envent->u.func.result->kind == Type_bool) set_type(sym, 8);
		else set_type(sym, -1);
	}
}

static char *get_ty_exp(AST_expr exp)
{
    switch (exp->kind) {
        case AST_intExp:
            return "i32";
            break;
        case AST_realExp:
            return "double";
            break;
        case AST_stringExp:
            return "i8*";
            break;
        case AST_charExp:
            return "i8";
            break;
        case AST_boolExp:
            return "i8";
            break;
        case AST_varExp:
            return get_ty_size(global_lookup(exp->u.var->u.entire));
            break;
    }
}

static FILE* filehandler()
{
    FILE * fp;
    fp = fopen("pascal.ll", "a");
    return fp;
}

static void close_file(FILE* fp)
{
    fclose(fp);
}

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "symtab.h"
#include "ast_structs.h"
#include "utility.h"

#define MAXSCOPE 100

Sym_tab stack[MAXSCOPE];

struct Symbol_ {
  char *name;
  void *value;
  int scope;
  int type;
  int stack_count;
  Symbol next;
};

struct Sym_table_ {
  Symbol first;
  int level;
};

Symbol new_sym(char* name, void* value){
	Symbol sym = cmalloc(sizeof(*sym));
	sym->name = name;
	sym->next = NULL;
	sym->value = value;
	sym->scope = 0;
	sym->stack_count = 0;
	sym->type = -1;
	return sym;
}

void set_symvalue(Symbol sym, void *value){
	sym->value = value;
}

void* get_symvalue(Symbol sym){
	return sym->value;
}

char* get_sym(Symbol sym){
	return sym->name;
}

Sym_tab create_tab(int level){
	Sym_tab sym_tab = cmalloc(sizeof(*sym_tab));
	sym_tab->first = NULL;
	sym_tab->level = level;
	return sym_tab;
}

void insert_sym(Sym_tab tab, Symbol sym){
	Symbol current;
	if(!tab) fprintf(stderr, "Scope table doesn't exist!\n");
	if(!tab->first) tab->first = sym;
	else {
		current = tab->first;
		while(current->next) {
			current = current->next;
		}
		current->next = sym;	
	}
}

void* global_lookup(Symbol sym){
	if(!sym) return NULL;
	char* name = get_sym(sym);
	for(int i = 0; i < MAXSCOPE; i++){
		Sym_tab curr = stack[i];
		if(curr != NULL){
			Symbol curr_sym = curr->first;
			while (curr_sym) {
				if(strcmp(curr_sym->name, name) == 0){
					return curr_sym;
				}
				curr_sym = curr_sym->next;
			}
		}
	}
	return NULL;
}

void* local_lookup(Sym_tab tab, Symbol sym){
	if(!tab || !sym) return NULL;
	char* name = get_sym(sym);
	Symbol current = tab->first;
	while (current) {
		if(strcmp(current->name, name) == 0)
			return current;
		current = current->next;
	}
	return NULL;
}

Sym_tab pop(void) {
	Sym_tab tmp;
	for(int i = 0; i < MAXSCOPE; i++) {
		if(stack[i] == NULL) {
			tmp = stack[i-1];
			stack[i-1] == NULL;
			return tmp;
		}
	}
}

Sym_tab begin_scope(Sym_tab tab) {
	for(int i = 0; i < MAXSCOPE; i++){
		if(stack[i] == NULL) {
			if(i >= 1) {
				stack[i] = stack[i-1];
				stack[i-1] = tab;
				return tab;
			}
			stack[i] = tab;
			return tab;
		}
  }
}

Sym_tab end_scope(Sym_tab tab){
	for(int i = 0; i < MAXSCOPE; i++){
		if(stack[i] == tab){
			return stack[i+1];
		}
	}
}

int get_scope(Symbol sym){
	return sym->scope;
}

void set_scope(Symbol sym, int scope){
	sym->scope = scope;
}

int get_stackc(Symbol sym){
	return sym->stack_count;
}

void set_stackc(Symbol sym, int stackc){
	sym->stack_count = stackc;
}

int get_type(Symbol sym){
	return sym->type;
}

void set_type(Symbol sym, int type){
	sym->type = type;
}

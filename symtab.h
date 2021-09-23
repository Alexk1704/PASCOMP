typedef struct Symbol_ *Symbol;
typedef struct Sym_table_ *Sym_tab;

/* Make a symbol out of a string */
Symbol new_sym(char* name, void* value);
/* Return string of symbol */
char* get_sym(Symbol sym);
/* Set type information value of symbol (environment) */
void set_symvalue(Symbol sym, void *value);
/* Get type information value of symbol (environment) */
void* get_symvalue(Symbol sym);
/* Create a new table mapping keys to values. */
Sym_tab create_tab(int level);
/* Insert binding "sym->value" into table, shadowing but not deleting
 * any previous binding of "sym". */
void insert_sym(Sym_tab tab, Symbol sym);
/* Look in one scope only */
void* local_lookup(Sym_tab tab, Symbol sym);
/* Look in all scopes */
void* global_lookup(Symbol sym);
/* Pop scope from stack */
Sym_tab pop(void);
/* Start a new scope in table (nested scope). */
Sym_tab begin_scope(Sym_tab tab);
/* Remove any entries inserted since current scope
 * began and end current scope. */
Sym_tab end_scope(Sym_tab tab);
/* get scope level of symbol for ir code generation */
int get_scope(Symbol sym);
/* set scope level of symbol for ir code generation */
void set_scope(Symbol sym, int scope);
/* get stack counter for ir code gen */
int get_stackc(Symbol sym);
/* set stack counter for ir code gen */
void set_stackc(Symbol sym, int stackc);
/* get type for ir code gen */
int get_type(Symbol sym);
/* set type for ir code gen */
void set_type(Symbol sym, int type);

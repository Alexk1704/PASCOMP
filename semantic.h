#include "symtab.h"

typedef struct Type_ty_ *Type_ty;
typedef struct Type_ty_l_ *Type_ty_l;

struct Type_ty_ {
	enum { Type_int, Type_longint, Type_real, Type_string, Type_char, 
		Type_bool, Type_arr, Type_set, Type_name, Type_void 
	} kind;
	union {
		Type_ty ty; // arrays and sets
		struct { Symbol sym; Type_ty ty; } name;
	} u;
};

struct Type_ty_l_ { Type_ty head; Type_ty_l tail; };

Type_ty Type_Int(void);
Type_ty Type_Longint(void);
Type_ty Type_Real(void);
Type_ty Type_String(void);
Type_ty Type_Char(void);
Type_ty Type_Bool(void);
Type_ty Type_Void(void);

Type_ty Type_Array(Type_ty ty);
Type_ty Type_Set(Type_ty ty);
Type_ty Type_Name(Symbol sym, Type_ty ty);

Type_ty_l Type_TyList(Type_ty head, Type_ty_l tail);

typedef struct Env_entry_ *Env_entry;

struct Env_entry_ {
	enum { Env_varEntry, Env_funcEntry } kind;
	union {
		struct { Type_ty ty; } var;
		struct { Type_ty_l fparams; Type_ty result; } func;
	} u;
};

Env_entry Env_VarEntry(Type_ty ty);
Env_entry Env_FuncEntry(Type_ty_l fparams, Type_ty result); 

Sym_tab Env_base(void);

int semantic_validation(void* node);





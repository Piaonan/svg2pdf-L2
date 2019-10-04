#ifndef ARGS_H_DU_PROJET
#define ARGS_H_DU_PROJET

typedef struct option_s {
  const char* keyword;
  enum { OptVoid, OptInt, OptString, OptFloat, OptFloatFloat, OptIntFloat, OptIntFloatFloat } spec;
  union {
    void (*opt_void)();
    void (*opt_int)(int);
    void (*opt_str)(const char*);
    void (*opt_float)(float);
	void (*opt_floatfloat)(float, float);
	void (*opt_intfloat)(int, float);
	void (*opt_intfloatfloat)(int, float, float);
  } fct;
  struct option_s* next;
} option_t;

option_t* opt_void(option_t* l, const char* kw, void (*f)());
option_t* opt_int(option_t* l, const char* kw, void (*f)(int));
option_t* opt_float(option_t* l, const char* kw, void (*f)(float));
option_t* opt_string(option_t* l, const char* kw, void (*f)(const char*));
option_t* opt_floatfloat(option_t* l, const char* kw, void (*f)(float, float));
option_t* opt_intfloat(option_t* l, const char* kw, void (*f)(int, float));
option_t* opt_intfloatfloat(option_t* l, const char* kw, void (*f)(int, float, float));
void opt_delete(option_t* l);
void process_options(option_t* l, int argc, char* *argv);







#endif
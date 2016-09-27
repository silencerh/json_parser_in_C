#include <stdlib.h>
#include "mlnjson.h"
#include <assert.h>
#include <errno.h>
#include <math.h>

#ifndef MLN_PARSE_STACK_INIT_SIZE
#define MLN_PARSE_STACK_INIT_SIZE 256
#endif

#define  EXPECT(c, ch) do{\
  assert(*c->json ==(ch));\
  c->json++;\
}while(0)

#define ISDIGIT(ch) ((ch>='0')&&(ch<='9'))
#define ISDIGIT1TO9(ch) ((ch>='1')&&(ch<='9'))
#define PUTC(c, ch) do { *(char*) mln_context_push(c, sizeof(char))=(ch);}while(0)
#define mln_init(v) do { (v)->type = MLN_NULL; } while(0)


typedef struct {
  const char* json;
  char* stack;
  size_t size, top;
}mln_context;

/*

*/
static void* mln_context_push(mln_context* c, size_t size){
  void* ret;
  assert(size > 0);
  if (c->top + size >= c->size) {
    if (c->size == 0)
      c->size = MLN_PARSE_STACK_INIT_SIZE;
    while (c->top + size >= c->size)
      c->size +=c->size >>1;
    c->stack = (char*)realloc(c->stack, c->size);
  }
  ret = c->stack + c->top;
  c->top += size;
  return ret;
}

static void* mln_context_pop(mln_context* c, size_t size){
  assert(c->top >= size);
  return c->stack + (c->top -=size);
}
static void mln_parse_whitespace(mln_context* c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
    p++;
  c->json = p;
}

static int mln_parse_literal(mln_context* c, mln_value* v){
  if (c->json[0]=='n'){
    if(c->json[1] != 'u' || c->json[2] != 'l' || c->json[3] != 'l')
      return MLN_PARSE_INVALID_VALUE;
    c->json +=4;
    v->type = MLN_NULL;
  }else if(c->json[0]=='t'){
    if(c->json[1] != 'r' || c->json[2] != 'u' || c->json[3] != 'e')
      return MLN_PARSE_INVALID_VALUE;
    c->json +=4;
    v->type = MLN_TRUE;
  }else{
    if(c->json[1] != 'a' || c->json[2] != 'l' || c->json[3] != 's'|| c->json[4] != 'e')
      return MLN_PARSE_INVALID_VALUE;
    c->json +=5;
    v->type = MLN_FALSE;
  }
  return MLN_PARSE_OK;
}

static int mln_parse_number(mln_context* c, mln_value* v){
  errno = 0;
  const char* p = c->json;
	if (*p == '-') p++;
	if (*p == '0') p++;
	else {
		if (!ISDIGIT1TO9(*p)) return MLN_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}
	if (*p == '.'){
		p++;
		if (!ISDIGIT(*p)) return MLN_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}
	if (*p == 'e' || *p == 'E') {
		p++;
		if (*p == '+' || *p == '-') p++;
		if (!ISDIGIT(*p)) return MLN_PARSE_INVALID_VALUE;
		for (p++; ISDIGIT(*p); p++);
	}
	v->u.n = strtod(c->json, NULL);
  if (errno ==ERANGE && (v->u.n == HUGE_VAL||-v->u.n == HUGE_VAL))
    return MLN_PARSE_NUMBER_TOO_BIG;
	v->type = MLN_NUMBER;
	c -> json = p;
  return MLN_PARSE_OK;
}

void mln_free(mln_value* v){
    assert(v != NULL);
    if (v->type == MLN_STRING)
      free(v->u.s.s);
    v->type = MLN_NULL;
}

static int mln_parse_string(mln_context* c, mln_value* v){
  size_t head = c->top, len;
  const char* p;
  EXPECT(c, '\"');
  p = c->json;
  for(;;){
    char ch = *p++;
    switch(ch){
      case '\"':
        len = c->top - head;
        mln_set_string(v, mln_context_pop(c, len), len);
        c->json = p;
        return MLN_PARSE_OK;
      case '\0':
        c->top = head;
        return MLN_PARSE_MISS_QUOTATION_MARK;
      default:
        PUTC(c, ch);
    }
  }
}




static int mln_parse_value(mln_context* c, mln_value* v){
  switch (*c ->json){
    case 'n': case 't': case 'f': return mln_parse_literal(c, v);
    case '\0': return MLN_PARSE_EXPECT_VALUE;
    default: return mln_parse_number(c, v);
  }
}

int mln_parse(mln_value* v, const char* json) {
	mln_context c;
	int ret;
	assert(v != NULL);
	c.json = json;
  c.stack = NULL;
  c.size = c.top = 0;
  mln_init(v);
	v->type = MLN_NULL;
	mln_parse_whitespace(&c);
	if ((ret = mln_parse_value(&c, v)) == MLN_PARSE_OK) {
		mln_parse_whitespace(&c);
		if (*c.json != '\0')
			return MLN_PARSE_ROOT_NOT_SINGULAR;
	}
	return ret;
}

mln_type mln_get_type(const mln_value* v) {
	assert(v != NULL);
	return v->type;
}

double mln_get_number(const mln_value* v){
  assert(v !=NULL && v->type==MLN_NUMBER);
  return v->u.n;
}

size_t mln_get_string_length(const mln_value* v){
  return v->u.s.len;
}

void mln_set_string(mln_value* v, const char* s, size_t len){
  assert(v!=NULL && (s!=NULL||len==0));
  mln_free(v);
  v->u.s.s = (char*)malloc(len +1);
  memcpy(v->u.s.s, s, len);
  v->u.s.s[len] = '\0';
  v->u.s.len = len;
  v->type = MLN_STRING;
}

const char*  mln_get_string(mln_value* v){
  return v->u.s.s;
}

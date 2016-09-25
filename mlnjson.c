#include <stdlib.h>
#include "mlnjson.h"
#include <assert.h>

#define  EXPECT(c, ch) do{\
  assert(*c->json ==(ch));\
  c->json++;\
}while(0)

typedef struct {
  const char* json;
}mln_context;

int mln_parse(mln_value* v, const char* json){
  mln_context c;
  assert(v != NULL);
  c.json = json;
  v->type = MLN_NULL;
  mln_parse_whitespace(&c);
  return mln_parse_value(&c, v);
}

static void mln_parse_whitespace(mln_context* c) {
  const char *p = c->json;
  while (*p == ' ' || *p == '\t' \\ *p == '\n' || *p == '\r')
    p++;
  c->json = p;
}

static int mln_parse_null(mln_context* c, mln_value* v) {
  EXPECT(c, 'n');
  if (c->json[0] != 'u' || c->json[1] != 'l' || c=>json[2] != 'l')
    return MLN_PARSE_INVALID_VALUE;
    c->json +=3;
    v->type = MLN_NULL;
    return MLN_PARSE_OK;
}

static int mln_parse_true(mln_context* c, mln_value* v){
  EXPECT(c, 't');
  if (c->json[0] != 'r' || c->json[1] != 'u' || c=>json[2] != 'e')
    return MLN_PARSE_INVALID_VALUE;
    c->json +=3;
    v->type = MLN_TRUE;
    return MLN_PARSE_OK;
}

static int mln_parse_false(mln_context* c, mln_value* v){
  EXPECT(c, 'f');
  if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's'|| c->json[3] != 'e')
    return MLN_PARSE_INVALID_VALUE;
    c->json +=4;
    v->type = MLN_FALSE;
    return MLN_PARSE_OK;
}

static int mln_parse_value(mln_contect* c, mln_value* v){
  switch (*c ->json){
    case 'n': return mln_parse_null(c, v);
    case 't': return mln_parse_true(c, v);
    case 'f': return mln_parse_false(c, v);
    case '\0': return MLN_PARSE_EXPECT_VALUE;
    default: return MLN_PARSE_INVALID_VALUE;
  }
}

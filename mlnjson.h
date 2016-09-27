#ifndef MLNJSON_H__
#define MLNJSON_H__

#define mln_set_null(v) mln_free(v)


typedef enum {
	MLN_NULL, MLN_FALSE, MLN_TRUE, MLN_NUMBER, MLN_ARRAY, MLN_STRING, MLN_OBJECT
}mln_type;
enum{
  MLN_PARSE_OK = 0,
  MLN_PARSE_EXPECT_VALUE,
  MLN_PARSE_INVALID_VALUE,
  MLN_PARSE_ROOT_NOT_SINGULAR,
  MLN_PARSE_NUMBER_TOO_BIG,
  MLN_PARSE_MISS_QUOTATION_MARK
};

typedef struct {
  union {
    struct { char* s; size_t len;}s;
    double n;
  } u;
  mln_type type;
}mln_value;

void mln_free(mln_value* v);

int mln_parse(mln_value* v, const char* json);

mln_type mln_get_type(const mln_value* v);
double mln_get_number(const mln_value* v);

size_t mln_get_string_length(const mln_value* v);
void mln_set_string(mln_value* v, const char* s, size_t len);
const char*  mln_get_string(mln_value* v);
#endif

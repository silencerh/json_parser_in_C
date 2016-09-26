#ifndef MLNJSON_H__
#define MLNJSON_H__

typedef enum {
	MLN_NULL, MLN_FALSE, MLN_TRUE, MLN_NUMBER, MLN_ARRAY, MLN_STRING, MLN_OBJECT
}mln_type;
enum{
  MLN_PARSE_OK = 0,
  MLN_PARSE_EXPECT_VALUE,
  MLN_PARSE_INVALID_VALUE,
  MLN_PARSE_ROOT_NOT_SINGULAR,
  MLN_PARSE_NUMBER_TOO_BIG
};

typedef struct {
  mln_type type;
  double n;
}mln_value;

int mln_parse(mln_value* v, const char* json);

mln_type mln_get_type(const mln_value* v);
double mln_get_number(const mln_value* v);
#endif

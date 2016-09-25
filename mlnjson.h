#ifndef MLNJSON_H__
#define MLNJSON_H__

typedef struct {
  mln_type type;
}mln_value;

int mln_parse(mln_value* v, const char* json);

enum{
  MLN_PARSE_OK = 0,
  MLN_PARSE_EXPECT_VALUE,
  MLN_PARSE_INVALID_VALUE,
  MLN_PARSE_ROOT_NOT_SINGULAR
};

#endif

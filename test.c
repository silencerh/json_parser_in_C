#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "mlnjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)\
  do{\
    test_count++;\
    if (equality)\
      test_pass++;\
    else {\
      fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual );\
      main_ret = 1;\
    }\
  }while(0)

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE(((expect)==(actual)), expect, actual, "%d")

#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect==actual), expect, actual, "%f")

#define EXPECT_EQ_STRING(expect, actual, alength) \
          EXPECT_EQ_BASE((sizeof(expect)-1)==alength && memcmp(expect, actual, alength)==0, expect, actual, "%s")
#define TEST_NUMBER(expect, json)\
  do{\
    mln_value v;\
    EXPECT_EQ_INT(MLN_PARSE_OK, mln_parse(&v, json));\
    EXPECT_EQ_INT(MLN_NUMBER, mln_get_type(&v));\
    EXPECT_EQ_DOUBLE(expect, mln_get_number(&v));\
  }while(0)

#define TEST_ERROR(error, json)\
  do{\
    mln_value v;\
    v.type = MLN_FALSE;\
    EXPECT_EQ_INT(error, mln_parse(&v, json));\
    EXPECT_EQ_INT(MLN_NULL, mln_get_type(&v));\
  }while(0)

static void test_parse_null() {
  mln_value v;
  v.type = MLN_NULL;
  EXPECT_EQ_INT(MLN_PARSE_OK, mln_parse(&v, "null"));
  EXPECT_EQ_INT(MLN_NULL, mln_get_type(&v));
}

static void test_parse_true(){
  mln_value v;
  v.type = MLN_TRUE;
  EXPECT_EQ_INT(MLN_PARSE_OK, mln_parse(&v, "true"));
  EXPECT_EQ_INT(MLN_TRUE, mln_get_type(&v));
}

static void test_parse_false(){
  mln_value v;
  v.type = MLN_NULL;
  EXPECT_EQ_INT(MLN_PARSE_OK, mln_parse(&v, "false"));
  EXPECT_EQ_INT(MLN_FALSE, mln_get_type(&v));
}


static void test_parse_number() {
    TEST_NUMBER(0.0, "0");
    TEST_NUMBER(0.0, "-0");
    TEST_NUMBER(0.0, "-0.0");
    TEST_NUMBER(1.0, "1");
    TEST_NUMBER(-1.0, "-1");
    TEST_NUMBER(1.5, "1.5");
    TEST_NUMBER(-1.5, "-1.5");
    TEST_NUMBER(3.1416, "3.1416");
    TEST_NUMBER(1E10, "1E10");
    TEST_NUMBER(1e10, "1e10");
    TEST_NUMBER(1E+10, "1E+10");
    TEST_NUMBER(1E-10, "1E-10");
    TEST_NUMBER(-1E10, "-1E10");
    TEST_NUMBER(-1e10, "-1e10");
    TEST_NUMBER(-1E+10, "-1E+10");
    TEST_NUMBER(-1E-10, "-1E-10");
    TEST_NUMBER(1.234E+10, "1.234E+10");
    TEST_NUMBER(1.234E-10, "1.234E-10");
    TEST_NUMBER(0.0, "1e-10000"); /* must underflow */
	TEST_NUMBER(1.0000000000000002, "1.0000000000000002"); /* the smallest number > 1 */
	TEST_NUMBER(4.9406564584124654e-324, "4.9406564584124654e-324"); /* minimum denormal */
	TEST_NUMBER(-4.9406564584124654e-324, "-4.9406564584124654e-324");
	TEST_NUMBER(2.2250738585072009e-308, "2.2250738585072009e-308");  /* Max subnormal double */
	TEST_NUMBER(-2.2250738585072009e-308, "-2.2250738585072009e-308");
	TEST_NUMBER(2.2250738585072014e-308, "2.2250738585072014e-308");  /* Min normal positive double */
	TEST_NUMBER(-2.2250738585072014e-308, "-2.2250738585072014e-308");
	TEST_NUMBER(1.7976931348623157e+308, "1.7976931348623157e+308");  /* Max double */
	TEST_NUMBER(-1.7976931348623157e+308, "-1.7976931348623157e+308");
}


static void test_parse_invalid_value() {

    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "+0");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "+1");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, ".123");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "1.");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "INF");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "inf");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "NAN");
    TEST_ERROR(MLN_PARSE_INVALID_VALUE, "nan");
}

static void test_parse_expect_value() {
    TEST_ERROR(MLN_PARSE_EXPECT_VALUE, "");
    TEST_ERROR(MLN_PARSE_EXPECT_VALUE, " ");
}

static void test_parse_number_too_big() {
    TEST_ERROR(MLN_PARSE_NUMBER_TOO_BIG, "1e309");
    TEST_ERROR(MLN_PARSE_NUMBER_TOO_BIG, "-1e309");
}

static void test_parse_root_not_singular() {
    TEST_ERROR(MLN_PARSE_ROOT_NOT_SINGULAR, "null x");
}

#define mln_init(v) do { (v)->type = MLN_NULL; } while(0)

static void test_access_string(){
  mln_value v;
  mln_init(&v);
  mln_set_string(&v,"",0);
  EXPECT_EQ_STRING("", mln_get_string(&v), mln_get_string_length(&v));
  mln_set_string(&v, "Hello", 5);
  EXPECT_EQ_STRING("Hello", mln_get_string(&v), mln_get_string_length(&v));
  mln_free(&v);
}
static void test_parse() {
  test_parse_null();
  test_parse_true();
  test_parse_false();
  test_parse_number();
  test_parse_invalid_value();
  test_parse_expect_value();
  test_parse_number_too_big();
  test_parse_root_not_singular();
  test_access_string();
}

void main() {
  test_parse();
  printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0/ test_count);
  getchar();
}

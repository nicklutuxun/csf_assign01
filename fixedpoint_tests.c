#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
  Fixedpoint zero;
  Fixedpoint one;
  Fixedpoint one_half;
  Fixedpoint one_fourth;
  Fixedpoint large1;
  Fixedpoint large2;
  Fixedpoint neg_1;
  Fixedpoint neg_one_eighth;
  Fixedpoint min_magnitude;
  Fixedpoint max;
  Fixedpoint min;

  // TODO: add more objects to the test fixture
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_fixedpoint_halve(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_overflow_neg(TestObjs *objs);
void test_is_underflow_pos(TestObjs *objs);
void test_is_underflow_neg(TestObjs *objs);
void test_is_err(TestObjs *objs);
void test_is_neg(TestObjs *objs);
void test_is_valid(TestObjs *objs);
void test_hex_is_valid(TestObjs *objs);
void test_remove_trailing_zeros(TestObjs *objs);
void test_halve(TestObjs *objs);
void test_double(TestObjs *objs);
// TODO: add more test functions

int main(int argc, char **argv) {
  // if a testname was specified on the command line, only that
  // test function will be executed
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_whole_part);
  TEST(test_frac_part);
  TEST(test_create_from_hex);
  TEST(test_fixedpoint_halve);
  TEST(test_format_as_hex);
  TEST(test_negate);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_is_overflow_pos);
  TEST(test_is_overflow_neg);
  TEST(test_is_underflow_pos);
  TEST(test_is_underflow_neg);
  TEST(test_is_err);
  TEST(test_is_neg);
  TEST(test_is_valid);
  TEST(test_hex_is_valid);
  TEST(test_remove_trailing_zeros);
  TEST(test_halve);
  TEST(test_double);

  // IMPORTANT: if you add additional test functions (which you should!),
  // make sure they are included here.  E.g., if you add a test function
  // "my_awesome_tests", you should add
  //
  //   TEST(my_awesome_tests);
  //
  // here. This ensures that your test function will actually be executed.

  TEST_FINI();
}

TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));

  objs->zero = fixedpoint_create(0UL);
  objs->one = fixedpoint_create(1UL);
  objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
  objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
  objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
  objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
  objs->max = fixedpoint_create2(0xffffffffffffffffUL, 0xffffffffffffffffUL);
  objs->neg_1 = fixedpoint_create_from_hex("-1");
  objs->neg_one_eighth = fixedpoint_create_from_hex("-0.2");
  objs->min = fixedpoint_create_from_hex("-ffffffffffffffff.ffffffffffffffff");

  return objs;
}

void cleanup(TestObjs *objs) {
  free(objs);
}

void test_whole_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0xffffffffffffffffUL == fixedpoint_whole_part(objs->max));
  ASSERT(1UL == fixedpoint_whole_part(objs->neg_1));
  ASSERT(0UL == fixedpoint_whole_part(objs->neg_one_eighth));
  ASSERT(0xffffffffffffffffUL == fixedpoint_whole_part(objs->min));
}

void test_frac_part(TestObjs *objs) {
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
  ASSERT(0xffffffffffffffffUL == fixedpoint_frac_part(objs->max));
  ASSERT(0UL == fixedpoint_frac_part(objs->neg_1));
  ASSERT(0x2000000000000000UL == fixedpoint_frac_part(objs->neg_one_eighth));
  ASSERT(0xffffffffffffffffUL == fixedpoint_frac_part(objs->min));
}

void test_create_from_hex(TestObjs *objs) {
  (void) objs;

  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));

  Fixedpoint val2 = fixedpoint_create_from_hex("f6a5865");
  ASSERT(fixedpoint_is_valid(val2));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val2));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(val2));

  Fixedpoint val3 = fixedpoint_create_from_hex("F6A5865.00F2");
  ASSERT(fixedpoint_is_valid(val3));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val3));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val3));

  Fixedpoint val4 = fixedpoint_create_from_hex("f!6a5865");
  ASSERT(!fixedpoint_is_valid(val4));
  ASSERT(fixedpoint_is_err(val4));

  Fixedpoint val5 = fixedpoint_create_from_hex("--f6a5865");
  ASSERT(!fixedpoint_is_valid(val5));
  ASSERT(fixedpoint_is_err(val5));

  Fixedpoint val6 = fixedpoint_create_from_hex("-.f6a5865");
  ASSERT(fixedpoint_is_valid(val6));
  ASSERT(!fixedpoint_is_err(val6));

  Fixedpoint val7 = fixedpoint_create_from_hex("f6a5865.");
  ASSERT(fixedpoint_is_valid(val7));
  ASSERT(!fixedpoint_is_err(val7));

  Fixedpoint val8 = fixedpoint_create_from_hex("-f-6a5865");
  ASSERT(!fixedpoint_is_valid(val8));
  ASSERT(fixedpoint_is_err(val8));

  Fixedpoint val9 = fixedpoint_create_from_hex("f.6.a5865");
  ASSERT(!fixedpoint_is_valid(val9));
  ASSERT(fixedpoint_is_err(val9));

  Fixedpoint val10 = fixedpoint_create_from_hex(".f6a5865");
  ASSERT(fixedpoint_is_valid(val10));
  ASSERT(!fixedpoint_is_err(val10));

  Fixedpoint val11 = fixedpoint_create_from_hex("");
  ASSERT(!fixedpoint_is_valid(val11));
  ASSERT(fixedpoint_is_err(val11));

  Fixedpoint val12 = fixedpoint_create_from_hex("1.3?f");
  ASSERT(!fixedpoint_is_valid(val12));  
  ASSERT(fixedpoint_is_err(val12));

  Fixedpoint val13 = fixedpoint_create_from_hex("csf_is_fun");
  ASSERT(!fixedpoint_is_valid(val13));
  ASSERT(fixedpoint_is_err(val13));

  Fixedpoint val14 = fixedpoint_create_from_hex("9.0.3");
  ASSERT(!fixedpoint_is_valid(val14));
  ASSERT(fixedpoint_is_err(val14));

  Fixedpoint val15 = fixedpoint_create_from_hex("-7--8");
  ASSERT(!fixedpoint_is_valid(val15));
  ASSERT(fixedpoint_is_err(val15));

  Fixedpoint val16 = fixedpoint_create_from_hex("ffffffffffffffff.ffffffffffffffff");
  ASSERT(fixedpoint_is_valid(val16));
  ASSERT(0xffffffffffffffffUL == fixedpoint_whole_part(val16));
  ASSERT(0xffffffffffffffffUL == fixedpoint_frac_part(val16));

  Fixedpoint val17 = fixedpoint_create_from_hex("0.0");
  ASSERT(fixedpoint_is_valid(val17));
  ASSERT(0x0000000000000000UL == fixedpoint_whole_part(val17));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(val17));

  Fixedpoint val18 = fixedpoint_create_from_hex("-f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val18));
  ASSERT(0xf6a5865UL == fixedpoint_whole_part(val18));
  ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val18));
}

void test_fixedpoint_halve(TestObjs *objs) {
  (void) objs;
  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  Fixedpoint val1half = fixedpoint_halve(val1);
  ASSERT(fixedpoint_is_valid(val1half));
  ASSERT(0x7b52c32UL == fixedpoint_whole_part(val1half));
  ASSERT(0x8079000000000000UL == fixedpoint_frac_part(val1half));

  Fixedpoint val2 = fixedpoint_create_from_hex("0.0000000000000001");
  Fixedpoint val2half = fixedpoint_halve(val2);
  ASSERT(!fixedpoint_is_valid(val2half));

  Fixedpoint val3 = fixedpoint_create_from_hex("-f6a5865.00f2");
  Fixedpoint val3half = fixedpoint_halve(val3);
  ASSERT(0x7b52c32UL == fixedpoint_whole_part(val3half));
  ASSERT(0x8079000000000000UL == fixedpoint_frac_part(val3half));  
}

void test_format_as_hex(TestObjs *objs) {
  char *s;

  s = fixedpoint_format_as_hex(objs->zero);
  ASSERT(0 == strcmp(s, "0"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one);
  ASSERT(0 == strcmp(s, "1"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_half);
  ASSERT(0 == strcmp(s, "0.8"));
  free(s);

  s = fixedpoint_format_as_hex(objs->one_fourth);
  ASSERT(0 == strcmp(s, "0.4"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large1);
  ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
  free(s);

  s = fixedpoint_format_as_hex(objs->large2);
  ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
  free(s);
}

void test_negate(TestObjs *objs) {
  // none of the test fixture objects are negative
  ASSERT(!fixedpoint_is_neg(objs->zero));
  ASSERT(!fixedpoint_is_neg(objs->one));
  ASSERT(!fixedpoint_is_neg(objs->one_half));
  ASSERT(!fixedpoint_is_neg(objs->one_fourth));
  ASSERT(!fixedpoint_is_neg(objs->large1));
  ASSERT(!fixedpoint_is_neg(objs->large2));
  ASSERT(fixedpoint_is_neg(objs->neg_1));
  ASSERT(fixedpoint_is_neg(objs->neg_one_eighth));
  ASSERT(fixedpoint_is_neg(objs->min));

  // negate the test fixture values
  Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
  Fixedpoint one_neg = fixedpoint_negate(objs->one);
  Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
  Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
  Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
  Fixedpoint large2_neg = fixedpoint_negate(objs->large2);
  Fixedpoint neg_1_neg = fixedpoint_negate(objs->neg_1);
  Fixedpoint neg_one_eighth_neg = fixedpoint_negate(objs->neg_one_eighth);
  Fixedpoint min_neg = fixedpoint_negate(objs->min);

  // zero does not become negative when negated
  ASSERT(!fixedpoint_is_neg(zero_neg));

  // non-negative values become negative
  ASSERT(fixedpoint_is_neg(one_neg));
  ASSERT(fixedpoint_is_neg(one_half_neg));
  ASSERT(fixedpoint_is_neg(one_fourth_neg));
  ASSERT(fixedpoint_is_neg(large1_neg));
  ASSERT(fixedpoint_is_neg(large2_neg));
  
  // negative values become non-negative
  ASSERT(!fixedpoint_is_neg(neg_1_neg));
  ASSERT(!fixedpoint_is_neg(neg_one_eighth_neg));
  ASSERT(!fixedpoint_is_neg(min_neg));

  // magnitudes should stay the same
  ASSERT(0UL == fixedpoint_whole_part(objs->zero));
  ASSERT(1UL == fixedpoint_whole_part(objs->one));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
  ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
  ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
  ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
  ASSERT(0UL == fixedpoint_frac_part(objs->zero));
  ASSERT(0UL == fixedpoint_frac_part(objs->one));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
  ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
  ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
  ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
  ASSERT(1UL == fixedpoint_whole_part(objs->neg_1));
  ASSERT(0UL == fixedpoint_whole_part(objs->neg_one_eighth));
  ASSERT(0xffffffffffffffffUL == fixedpoint_whole_part(objs->min));
  ASSERT(0UL == fixedpoint_frac_part(objs->neg_1));
  ASSERT(0x2000000000000000UL == fixedpoint_frac_part(objs->neg_one_eighth));
  ASSERT(0xffffffffffffffffUL == fixedpoint_frac_part(objs->min));
}

void test_add(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs1, rhs1, sum1, lhs2, rhs2, sum2, lhs3, rhs3, sum3;

  lhs1 = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  rhs1 = fixedpoint_create_from_hex("d09079.1e6d601");
  sum1 = fixedpoint_add(lhs1, rhs1);
  ASSERT(fixedpoint_is_neg(sum1));
  ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum1));
  ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum1));

  lhs2 = fixedpoint_create_from_hex("ffffffffffffffff");
  rhs2 = fixedpoint_create_from_hex("1");
  sum2 = fixedpoint_add(lhs2, rhs2);
  ASSERT(fixedpoint_is_overflow_pos(sum2));

  lhs3 = fixedpoint_create_from_hex("-ffffffffffffffff");
  rhs3 = fixedpoint_create_from_hex("-1");
  sum3 = fixedpoint_add(lhs3, rhs3);
  ASSERT(fixedpoint_is_overflow_neg(sum3));
}

void test_sub(TestObjs *objs) {
  (void) objs;

  Fixedpoint lhs1, rhs1, diff1, lhs2, rhs2, diff2, lhs3, rhs3, diff3;

  lhs1 = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs1 = fixedpoint_create_from_hex("f676e8.58");
  diff1 = fixedpoint_sub(lhs1, rhs1);
  ASSERT(fixedpoint_is_neg(diff1));
  ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff1));
  ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff1));

  lhs2 = fixedpoint_create_from_hex("-ffffffffffffffff");
  rhs2 = fixedpoint_create_from_hex("1");
  diff2 = fixedpoint_sub(lhs2, rhs2);
  ASSERT(fixedpoint_is_overflow_neg(diff2));

  lhs3 = fixedpoint_create_from_hex("ffffffffffffffff");
  rhs3 = fixedpoint_create_from_hex("-1");
  diff3 = fixedpoint_sub(lhs3, rhs3);
  ASSERT(fixedpoint_is_overflow_pos(diff3));
}

void test_is_overflow_pos(TestObjs *objs) {
  Fixedpoint sum;

  // boundary positive overflow
  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(objs->one, objs->max);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_sub(objs->max, objs->neg_1);
  ASSERT(fixedpoint_is_overflow_pos(sum));

  // no positive overflow
  sum = fixedpoint_add(objs->max, objs->neg_1);
  ASSERT(!fixedpoint_is_overflow_pos(sum));

  sum = fixedpoint_add(sum, objs->one);
  ASSERT(!fixedpoint_is_overflow_pos(sum));

  // negative overflow
  sum = fixedpoint_sub(objs->min, objs->one);
  ASSERT(!fixedpoint_is_overflow_pos(sum));
}

void test_is_overflow_neg(TestObjs *objs) {
  Fixedpoint sum;
  
  // boundary negative overflow
  sum = fixedpoint_sub(objs->min, objs->one);
  ASSERT(fixedpoint_is_overflow_neg(sum));

  // positive overflow
  sum = fixedpoint_add(objs->max, objs->one);
  ASSERT(!fixedpoint_is_overflow_neg(sum));

  // no negative overflow
  sum = fixedpoint_sub(objs->zero, objs->min);
  ASSERT(!fixedpoint_is_overflow_neg(sum));
}

void test_is_underflow_pos(TestObjs *objs) {
  Fixedpoint res;

  // no positive underflow
  res = fixedpoint_halve(objs->zero);
  ASSERT(!fixedpoint_is_underflow_pos(res));

  res = fixedpoint_halve(objs->one);
  ASSERT(!fixedpoint_is_underflow_pos(res));

  // positive underflow
  res = fixedpoint_halve(objs->max);
  ASSERT(fixedpoint_is_underflow_pos(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("0.0000000000000001"));
  ASSERT(fixedpoint_is_underflow_pos(res));

  // negative underflow
  res = fixedpoint_halve(objs->min);
  ASSERT(!fixedpoint_is_underflow_pos(res));
  Fixedpoint large2_half = fixedpoint_halve(objs->large2);
  ASSERT(fixedpoint_is_underflow_pos(large2_half));
  
}

void test_is_underflow_neg(TestObjs *objs) {
  Fixedpoint res;

  // no negative underflow
  res = fixedpoint_halve(objs->zero);
  ASSERT(!fixedpoint_is_underflow_neg(res));

  res = fixedpoint_halve(objs->one);
  ASSERT(!fixedpoint_is_underflow_neg(res));

  // positive underflow
  res = fixedpoint_halve(objs->max);
  ASSERT(!fixedpoint_is_underflow_neg(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("0.0000000000000001"));
  ASSERT(!fixedpoint_is_underflow_neg(res));

  // negative underflow
  res = fixedpoint_halve(objs->min);
  ASSERT(fixedpoint_is_underflow_neg(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("-0.0000000000000001"));
  ASSERT(fixedpoint_is_underflow_neg(res));
}

void test_is_err(TestObjs *objs) {
  (void) objs;

  // too many characters
  Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
  ASSERT(fixedpoint_is_err(err1));

  // too many characters
  Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
  ASSERT(fixedpoint_is_err(err2));

  // this one is actually fine
  Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
  ASSERT(fixedpoint_is_valid(err3));
  ASSERT(!fixedpoint_is_err(err3));

  // whole part is too large
  Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
  ASSERT(fixedpoint_is_err(err4));

  // fractional part is too large
  Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
  ASSERT(fixedpoint_is_err(err5));

  // invalid hex digits in whole part
  Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
  ASSERT(fixedpoint_is_err(err6));

  // invalid hex digits in fractional part
  Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
  ASSERT(fixedpoint_is_err(err7));
}

void test_is_neg(TestObjs *objs) {
  // not negative
  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(!fixedpoint_is_neg(val1));

  Fixedpoint val2 = objs->zero;
  ASSERT(!fixedpoint_is_neg(val2));

  Fixedpoint val3 = objs->max;
  ASSERT(!fixedpoint_is_neg(val3));

  // negative
  Fixedpoint val4 = fixedpoint_create_from_hex("-f6a5865.00f2");
  ASSERT(fixedpoint_is_neg(val4));

  Fixedpoint val5 = fixedpoint_create_from_hex("-111.f020");
  ASSERT(fixedpoint_is_neg(val5));

  Fixedpoint val6 = objs->min;
  ASSERT(fixedpoint_is_neg(val6));
}

void test_is_valid(TestObjs *objs) {
  Fixedpoint lhs, rhs, diff, sum;

  // valid
  Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
  ASSERT(fixedpoint_is_valid(val1));

  Fixedpoint val2 = fixedpoint_create_from_hex("f6a5865");
  ASSERT(fixedpoint_is_valid(val2));

  Fixedpoint val3 = fixedpoint_create_from_hex("F6A5865.00F2");
  ASSERT(fixedpoint_is_valid(val3));
  
  Fixedpoint val4 = fixedpoint_create_from_hex("-.f6a5865");
  ASSERT(fixedpoint_is_valid(val4));

  lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
  rhs = fixedpoint_create_from_hex("f676e8.58");
  diff = fixedpoint_sub(lhs, rhs);
  ASSERT(fixedpoint_is_valid(diff));

  lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
  rhs = fixedpoint_create_from_hex("d09079.1e6d601");
  sum = fixedpoint_add(lhs, rhs);
  ASSERT(fixedpoint_is_valid(sum));

  // not valid
  Fixedpoint val5 = fixedpoint_create_from_hex("f!6a5865");
  ASSERT(!fixedpoint_is_valid(val5));

  Fixedpoint val6 = fixedpoint_create_from_hex("--f6a5865");
  ASSERT(!fixedpoint_is_valid(val6));

  Fixedpoint val7 = fixedpoint_halve(objs->max);
  ASSERT(!fixedpoint_is_valid(val7));
}

void test_halve(TestObjs *objs) {
  Fixedpoint res;
  res = fixedpoint_halve(objs->zero);
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(0UL == fixedpoint_whole_part(res));
  ASSERT(0UL == fixedpoint_frac_part(res));

  res = fixedpoint_halve(objs->one);
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(0UL == fixedpoint_whole_part(res));
  ASSERT(0x8000000000000000UL == fixedpoint_frac_part(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("-0.0000000000000001"));
  ASSERT(fixedpoint_is_underflow_neg(res));
  ASSERT(0UL == fixedpoint_whole_part(res));
  ASSERT(0x0000000000000000UL == fixedpoint_frac_part(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("6666666666666666.8888888888888888"));
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(0x3333333333333333UL == fixedpoint_whole_part(res));
  ASSERT(0x4444444444444444UL == fixedpoint_frac_part(res));

  res = fixedpoint_halve(fixedpoint_create_from_hex("-36357F8.44AA44"));
  ASSERT(fixedpoint_is_neg(res));
  ASSERT(!fixedpoint_is_underflow_neg(res));
  ASSERT(0x1B1ABFCUL == fixedpoint_whole_part(res));
  ASSERT(0x2255220000000000UL == fixedpoint_frac_part(res));
}

void test_double(TestObjs *objs) {
  Fixedpoint res;
  res = fixedpoint_double(objs->zero);
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(0UL == fixedpoint_whole_part(res));
  ASSERT(0UL == fixedpoint_frac_part(res));

  res = fixedpoint_double(objs->one);
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(2UL == fixedpoint_whole_part(res));
  ASSERT(0UL == fixedpoint_frac_part(res));

  res = fixedpoint_double(fixedpoint_create_from_hex("-0.0000000000000001"));
  ASSERT(0UL == fixedpoint_whole_part(res));
  ASSERT(0x0000000000000002UL == fixedpoint_frac_part(res));

  res = fixedpoint_double(fixedpoint_create_from_hex("6666666666666666.6666666666666666"));
  ASSERT(!fixedpoint_is_neg(res));
  ASSERT(!fixedpoint_is_overflow_pos(res));
  ASSERT(0xCCCCCCCCCCCCCCCCUL == fixedpoint_whole_part(res));
  ASSERT(0xCCCCCCCCCCCCCCCCUL == fixedpoint_frac_part(res));

  res = fixedpoint_double(fixedpoint_create_from_hex("-36357F8.445544"));
  ASSERT(fixedpoint_is_neg(res));
  ASSERT(!fixedpoint_is_overflow_neg(res));
  ASSERT(0x6C6AFF0UL == fixedpoint_whole_part(res));
  ASSERT(0x88AA880000000000UL == fixedpoint_frac_part(res));

  res = fixedpoint_double(objs->max);
  ASSERT(fixedpoint_is_overflow_pos(res));
  ASSERT(0xFFFFFFFFFFFFFFFFUL == fixedpoint_whole_part(res));
  ASSERT(0xFFFFFFFFFFFFFFFEUL == fixedpoint_frac_part(res));
}

void test_hex_is_valid(TestObjs *objs) {
  (void) objs;

  // hex_is_valid has no contraint on length of whole and frac part
  // It only test whether the hex string is misformed
  ASSERT(hex_is_valid("88888888888888889.6666666666666666"));
  ASSERT(hex_is_valid("6666666666666666.88888888888888889"));
  ASSERT(hex_is_valid("-6666666666666666.8888888888888888"));
  ASSERT(hex_is_valid("88888888888888889"));
  ASSERT(hex_is_valid("7.88888888888888889"));
  ASSERT(!hex_is_valid("123xabc.4"));
  ASSERT(!hex_is_valid("7.0?4"));
  ASSERT(hex_is_valid("."));
  ASSERT(!hex_is_valid("-"));
  ASSERT(hex_is_valid("1.")); 
}

void test_remove_trailing_zeros(TestObjs *objs) {
}
// TODO: implement more test functions
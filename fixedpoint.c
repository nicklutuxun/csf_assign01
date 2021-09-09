#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "fixedpoint.h"

// You can remove this once all of the functions are fully implemented
static Fixedpoint DUMMY;

Fixedpoint fixedpoint_create(uint64_t whole) {
  // TODO: implement
  Fixedpoint val;
  val.whole = whole;
  val.frac = 0UL;
  val.tag = TAG_VALID_NONNEGATIVE;  // VNN for valid/non-negative
  return val;
}

Fixedpoint fixedpoint_create2(uint64_t whole, uint64_t frac) {
  // TODO: implement
  Fixedpoint val;
  val.whole = whole;
  val.frac = frac;
  val.tag = TAG_VALID_NONNEGATIVE;

  return val;
}

Fixedpoint fixedpoint_create_from_hex(const char *hex) {
  // TODO: implement
  Fixedpoint val;

  if (!hex_is_valid(hex))
  {
    val.tag = TAG_ERR;
    return val;
  }
  
  if (hex[0] == '-')
  {
    val.tag = TAG_VALID_NEGATIVE;  // VN for valid/negative
  } else {
    val.tag = TAG_VALID_NONNEGATIVE;
  }
  
  const char *pos = strchr(hex, '.');  // search for '.' in hex
  char *whole, *frac;
  int len = strlen(hex);
  int index;
  if (pos)  // "." found
  {
    index = pos - hex;
    if (val.tag == TAG_VALID_NEGATIVE)  // if valid/negative
    {
      whole = (char *)calloc(index, sizeof(char));
      frac = (char *)calloc(len - index, sizeof(char));
      memcpy(whole, &hex[1], index - 1);
      memcpy(frac, pos + 1, len - index);
    } else {
      whole = (char *)calloc(index + 1, sizeof(char));
      frac = (char *)calloc(len - index, sizeof(char));
      memcpy(whole, &hex[0], index);
      memcpy(frac, pos + 1, len - index);
    }
  } else {
    if (val.tag == TAG_VALID_NEGATIVE)
    {
      whole = (char *)calloc(len, sizeof(char));
      memcpy(whole, &hex[1], len - 1);
    } else {
      whole = (char *)calloc(len + 1, sizeof(char));
      memcpy(whole, &hex[0], len + 1);
    }
      frac = (char *)calloc(2, sizeof(char)); // 1 more to contain '\0'
      strcpy(frac, "0");
  }
  
  if (strlen(whole) > 16 || strlen(frac) > 16)
  {
    val.tag = TAG_ERR;
  }
  
  char *endptr;
  val.whole = strtoul(whole, &endptr, 16);
  uint64_t literal = strtoul(frac, &endptr, 16);
  int shifts = (16 - strlen(frac)) * 4;
  val.frac = literal << shifts;
  free(whole);
  free(frac);
  return val;
}

uint64_t fixedpoint_whole_part(Fixedpoint val) {
  // TODO: implement
  return val.whole;
}

uint64_t fixedpoint_frac_part(Fixedpoint val) {
  // TODO: implement
  return val.frac;
}

Fixedpoint fixedpoint_add(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  Fixedpoint res;
  uint64_t whole_res;
  uint64_t frac_res;
  if (left.tag == right.tag) {
    whole_res = left.whole + right.whole;
    frac_res = left.frac + right.frac;
    if (frac_res < left.frac || frac_res < right.frac) {
      whole_res += 1UL;
    }
    if (whole_res < left.whole || whole_res < right.whole) {
      if (left.tag == TAG_VALID_NONNEGATIVE) res.tag = TAG_POS_OVERFLOW;
      else res.tag = TAG_NEG_OVERFLOW;
    }
    else {
      if (left.tag == TAG_VALID_NONNEGATIVE) res.tag = TAG_VALID_NONNEGATIVE;
      else res.tag = TAG_VALID_NEGATIVE;
    }
  }
  else {
    if (left.tag == TAG_VALID_NEGATIVE) {
      whole_res = (right.whole >= left.whole) ? (right.whole - left.whole) : (left.whole - right.whole);
      frac_res = right.frac - left.frac;
      if (frac_res > right.frac) {
        whole_res = (right.whole > left.whole) ? (whole_res - 1) : (whole_res + 1);
      }
      if (right.whole > left.whole || (right.whole == left.whole && right.frac > left.frac)) res.tag = TAG_VALID_NONNEGATIVE;
      else res.tag = TAG_VALID_NEGATIVE;
    }
    else {
      whole_res = (left.whole >= right.whole) ? (left.whole - right.whole) : (right.whole - left.whole);
      frac_res = left.frac - right.frac;
      if (frac_res > left.frac) {
        whole_res = (left.whole > right.whole) ? (whole_res - 1) : (whole_res + 1);
      }
      if (left.whole > right.whole || (left.whole == right.whole && left.frac > right.frac)) res.tag = TAG_VALID_NONNEGATIVE;
      else res.tag = TAG_VALID_NEGATIVE;
    }
  }
  res.whole = whole_res;
  res.frac = frac_res;
  return res;
}

Fixedpoint fixedpoint_sub(Fixedpoint left, Fixedpoint right) {
  if (right.tag == TAG_VALID_NEGATIVE) right.tag = TAG_VALID_NONNEGATIVE;
  else if (right.tag == TAG_VALID_NONNEGATIVE) right.tag = TAG_VALID_NEGATIVE;
  return (fixedpoint_add(left, right));
}

Fixedpoint fixedpoint_negate(Fixedpoint val) {
  // TODO: implement
  if (val.tag == TAG_VALID_NONNEGATIVE) {
    if (!(fixedpoint_is_zero(val))) val.tag = TAG_VALID_NEGATIVE;
  }
  else if (val.tag == TAG_VALID_NEGATIVE) val.tag = TAG_VALID_NONNEGATIVE;
  return val;
}

Fixedpoint fixedpoint_halve(Fixedpoint val) {
  // TODO: implement
  assert(0);
  return DUMMY;
}

Fixedpoint fixedpoint_double(Fixedpoint val) {
  return fixedpoint_add(val, val);
}

int fixedpoint_compare(Fixedpoint left, Fixedpoint right) {
  // TODO: implement
  assert(0);
  return 0;
}

int fixedpoint_is_zero(Fixedpoint val) {
  // TODO: implement
  if ((val.whole == 0UL) && (val.frac == 0UL))
  {
    return 1;
  }
  
  return 0;
}

int fixedpoint_is_err(Fixedpoint val) {
  if (val.tag == TAG_ERR)
  {
    return 1;
  }
  return 0;
}

int fixedpoint_is_neg(Fixedpoint val) {
  if (val.tag == TAG_VALID_NEGATIVE) return 1;
  else return 0;
}

int fixedpoint_is_overflow_neg(Fixedpoint val) {
  if (val.tag == TAG_NEG_OVERFLOW) return 1;
  else return 0;
}

int fixedpoint_is_overflow_pos(Fixedpoint val) {
  if (val.tag == TAG_POS_OVERFLOW) return 1;
  else return 0;
}

int fixedpoint_is_underflow_neg(Fixedpoint val) {
  if (val.tag == TAG_NEG_UNDERFLOW) return 1;
  else return 0;
}

int fixedpoint_is_underflow_pos(Fixedpoint val) {
  if (val.tag == TAG_POS_UNDERFLOW) return 1;
  else return 0;
}

int fixedpoint_is_valid(Fixedpoint val) {
  if (val.tag == TAG_VALID_NONNEGATIVE || val.tag == TAG_VALID_NEGATIVE)
  {
    return 1;
  }
  return 0;
}

char *fixedpoint_format_as_hex(Fixedpoint val) {
  // TODO: implement
  assert(0);
  char *s = malloc(20);
  strcpy(s, "<invalid>");
  return s;
}

int hex_is_valid(const char *hex) {
  if (!(isxdigit(hex[0]) || (hex[0] == '-')))
  {
    return 0;
  }

  for (uint64_t i = 1; i < strlen(hex); i++)
  {
    if (!(isxdigit(hex[i]) || (hex[i] == '.')))
    {
      return 0;
    }
  }
  return 1;
}

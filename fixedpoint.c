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

  if (!hex_is_valid(hex)) // error if parameter hex is not valid
  {
    val.tag = TAG_ERR;
    return val;
  }
  
  if (hex[0] == '-') // determine sign
  {
    val.tag = TAG_VALID_NEGATIVE;
  } else {
    val.tag = TAG_VALID_NONNEGATIVE;
  }
  
  const char *pos = strchr(hex, '.');  // search for '.' in hex
  char *whole, *frac;
  int len = strlen(hex);
  int index;
  if (pos)  // '.' found
  {
    index = pos - hex; // index of '.'
    if (val.tag == TAG_VALID_NEGATIVE)
    {
      // copy whole and frac part into temporary char*
      whole = (char *)calloc(index, sizeof(char));
      frac = (char *)calloc(len - index, sizeof(char));
      memcpy(whole, &hex[1], index - 1);
      memcpy(frac, pos + 1, len - index);
    } else {
      // copy whole and frac part into temporary char*
      whole = (char *)calloc(index + 1, sizeof(char));
      frac = (char *)calloc(len - index, sizeof(char));
      memcpy(whole, &hex[0], index);
      memcpy(frac, pos + 1, len - index);
    }
  } else {
    // '.' not found
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
  // convert whole part into uint64_t
  val.whole = strtoul(whole, &endptr, 16); 
  // convert frac part into uint64_t. 
  //We use variable literal here to make following shifts more convenient
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
  int tag_conversion_flag = 0;
  if (left.tag == right.tag) {
    whole_res = left.whole + right.whole;
    if (whole_res < left.whole || whole_res < right.whole) {
      if (left.tag == TAG_VALID_NONNEGATIVE) res.tag = TAG_POS_OVERFLOW;
      else res.tag = TAG_NEG_OVERFLOW;
      tag_conversion_flag = 1;
    }
    frac_res = left.frac + right.frac;
    if (frac_res < left.frac || frac_res < right.frac) {
      whole_res += 1UL;
    }
    if (!tag_conversion_flag) {
      if (whole_res < left.whole || whole_res < right.whole) {
        if (left.tag == TAG_VALID_NONNEGATIVE) res.tag = TAG_POS_OVERFLOW;
        else res.tag = TAG_NEG_OVERFLOW;
      }
      else {
        if (left.tag == TAG_VALID_NONNEGATIVE) res.tag = TAG_VALID_NONNEGATIVE;
        else res.tag = TAG_VALID_NEGATIVE;
      }
    }
  }
  else {
    int flag = (right.whole > left.whole || (right.whole == left.whole && right.frac > left.frac));
    whole_res = flag ? (right.whole - left.whole) : (left.whole - right.whole);
    frac_res = flag ? (right.frac - left.frac) : (left.frac - right.frac);
    if (frac_res > (flag ? right.frac : left.frac)) {
      whole_res -= 1UL;
    }
    if (flag) res.tag = right.tag;
    else res.tag = left.tag;
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
  Fixedpoint res;
  uint64_t whole_res = val.whole/2;
  uint64_t frac_res = val.frac;
  if (frac_res % 2 != 0) {
    res.tag = (val.tag == TAG_VALID_NONNEGATIVE) ? TAG_POS_UNDERFLOW : TAG_NEG_UNDERFLOW;
  }
  else {
    res.tag = val.tag;
  }
  frac_res = frac_res/2;
  if (val.whole % 2 != 0) frac_res += 0x8000000000000000UL;
  res.whole = whole_res;
  res.frac = frac_res;
  return res;
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
  char *hexstr, *whole, *frac;

  whole = (char *)calloc(17, sizeof(char));
  sprintf(whole, "%llx", val.whole);
  frac = (char *)calloc(17, sizeof(char));
  sprintf(frac, "%llx", val.frac);
  hexstr = (char *)calloc(35, sizeof(char));

  if (val.tag == TAG_VALID_NEGATIVE)
  {
    strcat(hexstr, "-");
  }

  strcat(hexstr, whole);

  if (val.frac != 0x0UL)
  {
    strcat(hexstr, ".");
    for (int i = 0; i < (int)(16-strlen(frac)); i++)
    {
      strcat(hexstr, "0");
    }
    strcat(hexstr, frac);
  }

  remove_trailing_zeros(hexstr);

  return hexstr;
  free(hexstr);
  free(whole);
  free(frac);

}

int hex_is_valid(const char *hex) {
  if (!(isxdigit(hex[0]) || (hex[0] == '-') || (hex[0] == '.'))) // first digit can only have hex/-/.
  {
    return 0;
  }

  if (hex[0] == '-' && !(isxdigit(hex[1])) && hex[1] != '.') // leading '-' can only followed by hex besides '.'
  { 
    return 0;
  }
  
  int counter = 0; // count for decimal point
  if (hex[0] == '.' || ((hex[0] == '-') && (hex[1] == '.')))
  {
    counter++;
  }
  
  for (uint64_t i = counter + 1; i < strlen(hex); i++)
  {
    if (!(isxdigit(hex[i]) || (hex[i] == '.'))) // middle sequence can only have hex or '.'
    {
      return 0;
    }

    if (hex[i] == '.')
    {
      counter++;
    }
  }

  if (!isxdigit(hex[strlen(hex)-1]) && hex[strlen(hex)-1] != '.') // if last digit is not hex digit
  {
    return 0;
  }
  

  if (counter > 1) // if more than 1 '.' exist
  {
    return 0;
  }
  
  return 1;
}

void remove_trailing_zeros(char *str) {
  if (strcmp(str, "0") == 0)
  {
    return;
  }
  
  int index = strlen(str) - 1;
  while (str[index] == '0') {
    if (str[index-1] != '0')
    {
      str[index] = '\0';
      return;
    }
    index--;
  }
}

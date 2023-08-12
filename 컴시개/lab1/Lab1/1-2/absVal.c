#include "absVal.h"

/*
 * absVal(x): return the absolute value of x
 *   Example: absVal(-1) = 1
 *   You may assume -TMax <= x <= TMax
 *   Legal ops: ! ~ & ^ | + << >>
 */
int absVal(int x) {
  if (x < 0) {
    return -x;
  } else {
    return x;
  }
}

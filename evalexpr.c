/* Copyright 1997-2016 by Chris Osborn <fozztexx@fozztexx.com>
 * http://insentricity.com
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "evalexpr.h"
#include <fozzlib/math.h>
#include <fozzlib/strings.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define STACKSIZE 100

typedef enum {
  OP_LOR, OP_LAND, OP_NEQ, OP_EQ, OP_LE, OP_GE, OP_LT, OP_GT,
  OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_IDIV, OP_MOD, OP_POW,
  OP_LNOT, OP_BOR, OP_XOR, OP_BAND, OP_RSH, OP_LSH, OP_BNOT,
  OP_PAREN,
} operator;

typedef struct opRec {
  char *str;
  operator op;
  int precedence;
} opRec;

static opRec knownOps[] = {
  {"||", OP_LOR, 1},
  {"&&", OP_LAND, 2},
  {"|", OP_BOR, 3},
  {"^", OP_XOR, 4},
  {"&", OP_BAND, 5},
  {"!=", OP_NEQ, 6},
  {"==", OP_EQ, 6},
  {"<", OP_LT, 7},
  {"<=", OP_LE, 7},
  {">", OP_GT, 7},
  {">=", OP_GE, 7},
  {">>", OP_RSH, 8},
  {"<<", OP_LSH, 8},
  {"+", OP_ADD, 9},
  {"-", OP_SUB, 9},
  {"*", OP_MUL, 10},
  {"/", OP_DIV, 10},
  {"\\", OP_IDIV, 10},
  {"%", OP_MOD, 10},
  {"**", OP_POW, 12},
  {"!", OP_LNOT, 13},
  {"~", OP_BNOT, 13},
  {NULL, 0, 0},
};
static int sorted = 0;

static int stacksize = 0;
static operator stackOp[STACKSIZE];
static double stackVal[STACKSIZE];
static int parenLevel = 0;

int precedence(operator op)
{
  int i;


  for (i = 0; knownOps[i].str; i++)
    if (knownOps[i].op == op % OP_PAREN)
      return knownOps[i].precedence + (op / OP_PAREN) * OP_PAREN;

  return -1;
}

void dofunc(operator op, double val, double *curVal)
{
  switch (op % OP_PAREN) {
  case OP_LOR:
    *curVal = val || *curVal;
    break;

  case OP_LAND:
    *curVal = val && *curVal;
    break;

  case OP_NEQ:
    *curVal = val != *curVal;
    break;
    
  case OP_EQ:
    *curVal = val == *curVal;
    break;

  case OP_LE:
    *curVal = val <= *curVal;
    break;

  case OP_GE:
    *curVal = val >= *curVal;
    break;

  case OP_LT:
    *curVal = val < *curVal;
    break;

  case OP_GT:
    *curVal = val > *curVal;
    break;
    
  case OP_ADD:
    *curVal += val;
    break;

  case OP_SUB:
    *curVal = val - *curVal;
    break;

  case OP_MUL:
    *curVal *= val;
    break;

  case OP_DIV:
    *curVal = val / *curVal;
    break;

  case OP_IDIV:
    *curVal = (int) val / (int) *curVal;
    break;

  case OP_MOD:
    *curVal = (int) val % (int) *curVal;
    break;

  case OP_POW:
    *curVal = pow(val, *curVal);
    break;

  case OP_LNOT:
    *curVal = !*curVal;
    break;

  case OP_BOR:
    *curVal = (int) val | (int) *curVal;
    break;

  case OP_XOR:
    *curVal = (int) val ^ (int) *curVal;
    break;

  case OP_BAND:
    *curVal = (int) val & (int) *curVal;
    break;

  case OP_RSH:
    *curVal = (int) val >> (int) *curVal;
    break;
    
  case OP_LSH:
    *curVal = (int) val << (int) *curVal;
    break;

  case OP_BNOT:
    *curVal = ~(int) *curVal;
  }

  return;
}

void pushop(int op, double *value)
{
  op += OP_PAREN * parenLevel;
  
  if (stacksize && precedence(stackOp[stacksize - 1]) >= precedence(op)) {
    dofunc(stackOp[stacksize - 1], stackVal[stacksize - 1], value);
    stacksize--;
  }
  
  stackOp[stacksize] = op;
  stackVal[stacksize] = *value;
  stacksize++;
  *value = 0;
}

int xtoi(const char *str)
{
  int i, j;


  for (i = j = 0; isxdigit(str[i]); i++)
    j <<= 4, j |= toupper(str[i]) - '0' - (str[i] > '9' ? 7 : 0);

  return j;
}

double evalexpr(const char *str, double curval)
{
  int i, j, k;
  char buf[1024];
  int ss = stacksize;
  int len = strlen(str);
  double atof();
  opRec anOp;
  

  if (!sorted) {
    for (i = 0; knownOps[i].str; i++)
      for (j = i + 1; knownOps[j].str; j++)
	if (strlen(knownOps[j].str) > strlen(knownOps[i].str)) {
	  anOp = knownOps[j];
	  knownOps[j] = knownOps[i];
	  knownOps[i] = anOp;
	}
    sorted = 1;
  }
  
  i = skipwhite(str);
  while (i < len) {
    if (str[i] == '(') {
      for (k = j = 1; k; j++) {
	if (str[i + j] == ')')
	  k--;
	if (str[i + j] == '(')
	  k++;
      }
      strncpy(buf, &str[i + 1], j - 1);
      buf[j - 2] = 0;
      parenLevel++;
      curval = evalexpr(buf, 0);
      parenLevel--;
      i += j + 1;
      i += skipwhite(&str[i]);
      continue;
    }

    if (isdigit(str[i]) || str[i] == '.') { /* number */
      if (str[i+1] == 'x' || str[i+1] == 'X') /* Hex number */
	curval = xtoi(&str[i+2]), i += 2;
      else
	curval = atof(&str[i]);
      for (; str[i] && (isxdigit(str[i]) || str[i] == '.'); i++);
      i += skipwhite(&str[i]);
    }

    for (j = 0; knownOps[j].str; j++) {
      if (!strncmp(&str[i], knownOps[j].str, strlen(knownOps[j].str))) {
	pushop(knownOps[j].op, &curval);
	i += strlen(knownOps[j].str);
	i += skipwhite(&str[i]);
	break;
      }
    }

    if (!knownOps[j].str)
      i++;
  }

  
  for (j = stacksize; j > ss; j--) {
    dofunc(stackOp[j - 1], stackVal[j - 1], &curval);
    stacksize--;
  }

  return curval;
}

#ifdef DEBUG

void main(int argc, char *argv[])
{
  char buf[256];
  int i;
  
  void exit();
  

  if (argc > 1) {
    for (i = 1; i < argc; i++) {
      strcat(buf, argv[i]);
      strcat(buf, " ");
    }

    if (strpos(buf, "0x"))
      printf("0x%x\n", (unsigned int) evalexpr(buf, 2));
    else
      printf("%f\n", evalexpr(buf, 2));
  }
  else {
    printf("Enter expression: ");
    gets(buf);
    
    printf("Answer is: %f\n", evalexpr(buf, 2));
  }

  exit(0);
}

#endif /* DEBUG */

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

#include <stdio.h>
#include <fozzlib/strings.h>

char *doubletofraction(double val)
{
  long long PRECISION = 1000000000;
  int GIVEUP = 30;
  static char buf[256];
  double frac;
  double nfrac;
  int i, j;
  long long k, l;
  

  if (val - (int) val) {
    frac = val - (int) val;
    for (i = 2; i < GIVEUP; i++) {
      for (j = 1; j < i; j++) {
	nfrac = (double) j / (double) i;
	k = nfrac * PRECISION;
	l = frac * PRECISION;
	if (!(k - l))
	  break;
      }
      if (j != i)
	break;
    }
    
    if (i < GIVEUP) {
      if ((int) val) 
	sprintf (buf, "%i %i/%i", (int) val, j, i);
      else
	sprintf (buf, "%i/%i", j, i);
    }
    else
      sprintf (buf, "%f", val);
  }
  else
    sprintf (buf, "%i", (int) val);

  return buf;
}

double fractiontodouble(char *str, int *len)
{
  int i, j, k, l;
  double amount;
  int nf;
  
  
  i = skipwhite (str);
  for (amount = 0, j = 1; str[i] >= '0' && str[i] <= '9'; i++) {
    amount *= 10;
    amount += str[i] - '0';
  }
  if (str[i] == '.')
    for (i++; str[i] >= '0' && str[i] <= '9'; i++) {
      j *= 10;
      amount *= 10;
      amount += str[i] - '0';
    }
  amount /= j;
  if (str[i] == '/' && str[i+1] >= '0' && str[i+1] <= '9') {
    for (l = 0, i++; str[i] >= '0' && str[i] <= '9'; i++) {
      l *= 10;
      l += str[i] - '0';
    }
    amount /= l;
  }
  
  i += skipwhite (&str[i]);
  
  if (str[i] >= '0' && str[i] <= '9') { /* Looks like there could be a fraction */
    nf = i;
    for (k = 0; str[i] >= '0' && str[i] <= '9'; i++) {
      k *= 10;
      k += str[i] - '0';
    }
    if (str[i] == '/' && str[i+1] >= '0' && str[i+1] <= '9') { /* Yep, it's a fraction */
      for (l = 0, i++; str[i] >= '0' && str[i] <= '9'; i++) {
	l *= 10;
	l += str[i] - '0';
      }
      amount += (double) k / (double) l;
    }
    else
      i = nf;
  }

  *len = i;
  return amount;
}
  

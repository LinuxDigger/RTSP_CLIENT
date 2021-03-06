/*
 * Locale.cpp
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */


#include "Locale.h"
#include "strDup.h"

Locale::Locale(char const* newLocale, LocaleCategory category) {
#ifndef LOCALE_NOT_USED
#ifndef XLOCALE_NOT_USED
  int categoryMask;
  switch (category) {
    case All: { categoryMask = LC_ALL_MASK; break; }
    case Numeric: { categoryMask = LC_NUMERIC_MASK; break; }
  }
  fLocale = newlocale(categoryMask, newLocale, NULL);
  fPrevLocale = uselocale(fLocale);
#else
  switch (category) {
    case All: { fCategoryNum = LC_ALL; break; }
    case Numeric: { fCategoryNum = LC_NUMERIC; break; }
  }
  fPrevLocale = strDup(setlocale(fCategoryNum, NULL));
  setlocale(fCategoryNum, newLocale);
#endif
#endif
}

Locale::~Locale() {
#ifndef LOCALE_NOT_USED
#ifndef XLOCALE_NOT_USED
  if (fLocale != (locale_t)0) {
    uselocale(fPrevLocale);
    freelocale(fLocale);
  }
#else
  if (fPrevLocale != NULL) {
    setlocale(fCategoryNum, fPrevLocale);
    delete[] fPrevLocale;
  }
#endif
#endif
}



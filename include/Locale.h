/*
 * Locale.h
 *
 *  Created on: Jun 11, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_LOCALE_H_
#define INCLUDE_LOCALE_H_


#ifndef LOCALE_NOT_USED
#include <locale.h>
#ifndef XLOCALE_NOT_USED
#include <xlocale.h> // because, on some systems, <locale.h> doesn't include <xlocale.h>; this makes sure that we get both
#endif
#endif


enum LocaleCategory { All, Numeric }; // define and implement more categories later, as needed

class Locale {
public:
  Locale(char const* newLocale, LocaleCategory category = All);
  virtual ~Locale();

private:
#ifndef LOCALE_NOT_USED
#ifndef XLOCALE_NOT_USED
  locale_t fLocale, fPrevLocale;
#else
  int fCategoryNum;
  char* fPrevLocale;
#endif
#endif
};




#endif /* INCLUDE_LOCALE_H_ */

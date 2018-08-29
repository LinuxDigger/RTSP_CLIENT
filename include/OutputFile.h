/*
 * OutputFile.h
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#ifndef INCLUDE_OUTPUTFILE_H_
#define INCLUDE_OUTPUTFILE_H_

#include <stdio.h>

FILE* OpenOutputFile(UsageEnvironment& env, char const* fileName);

void CloseOutputFile(FILE* fid);

#endif /* INCLUDE_OUTPUTFILE_H_ */

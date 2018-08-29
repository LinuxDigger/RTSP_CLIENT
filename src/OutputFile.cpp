/*
 * OutputFile.cpp
 *
 *  Created on: Aug 9, 2018
 *      Author: jhb
 */

#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
#include <io.h>
#include <fcntl.h>
#endif
#ifndef _WIN32_WCE
#include <sys/stat.h>
#endif
#include <string.h>
#include <UsageEnvironment.h>
#include "OutputFile.h"

FILE* OpenOutputFile(UsageEnvironment& env, char const* fileName) {
  FILE* fid;

  // Check for special case 'file names': "stdout" and "stderr"
  if (strcmp(fileName, "stdout") == 0) {
    fid = stdout;
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
    _setmode(_fileno(stdout), _O_BINARY);       // convert to binary mode
#endif
  } else if (strcmp(fileName, "stderr") == 0) {
    fid = stderr;
#if (defined(__WIN32__) || defined(_WIN32)) && !defined(_WIN32_WCE)
    _setmode(_fileno(stderr), _O_BINARY);       // convert to binary mode
#endif
  } else {
    fid = fopen(fileName, "wb");
  }

  if (fid == NULL) {
    env.setResultMsg("unable to open file \"", fileName, "\"");
  }

  return fid;
}

void CloseOutputFile(FILE* fid) {
  // Don't close 'stdout' or 'stderr', in case we want to use it again later.
  if (fid != NULL && fid != stdout && fid != stderr) fclose(fid);
}


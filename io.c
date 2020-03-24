#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "io.h"

static void handleError(const char *message) {
   perror(message);
   exit(EXIT_FAILURE);
}

int openFile(const char *fileName, char mode) {
   int fd, actualMode = 0, flags = 0;

   if (mode == 'r') {
      flags = O_RDONLY;
      actualMode = 0600;
   } else if (mode == 'w') {
      flags = O_WRONLY | O_CREAT | O_TRUNC;
      actualMode = 0666;
   } else {
      handleError("dwmstatus: Unkown openFile mode");
   }

   if ((fd = open(fileName, flags, actualMode)) == -1) {
      handleError(fileName);
   }

   return fd;
}

void closeFile(int fd) {
   if (close(fd) == -1)
      handleError(NULL);
}

/**
 * @file
 * @brief This is a sample file for using LibReadConfig.
 *
 * Using LibReadConfig is as simple as possible. This example will show you
 * how to do that.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libreadconfig.h"

int main(int argc, char* argv[]){

  LRC_configDefaults ct[] = {
    {"default", "inidata", "test.dat", LRC_CHAR},
    {"default", "fs", "exts", LRC_CHAR},
    {"default", "nprocs", "4", LRC_INT},
    {"default", "bodies", "3", LRC_INT},
    {"logs", "dump", "100", LRC_INT},
    {"logs", "period", "23.47", LRC_DOUBLE},
    {"logs", "epoch", "2003.0", LRC_FLOAT},
    {"farm", "xres", "222", LRC_INT},
    {"farm", "yres", "444", LRC_INT},
    {NULL, NULL, NULL, 0}
  };
	
  LRC_assignDefaults(ct); 
  
  /* Print all options */
  LRC_printAll(); 
 
  /* Finally, we release resources.*/
  LRC_cleanup();
	
  return 0;
}


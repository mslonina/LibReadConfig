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
#include <unistd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/dir.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>

/**
 * Include LibReadConfig
 */

/*#define HAVE_HDF5_H 0*/

#include "libreadconfig.h"
#include "libreadconfig_hdf5.h"

/**
 * Include HDF5 library
 */
#include "hdf5.h"

int main(int argc, char* argv[]){

/**
 * @var char* inif
 * @brief Config file name.
 *
 * @var char* sep
 * @brief Separator mark.
 *
 * @var char* comm
 * @brief Comment mark.
 *
 * @var LRC_configNamespace cs[50]
 * @brief Define maximum number of namespaces.
 *
 * @var LRC_configNamespace cc[50]
 * @brief Define maximum number of namespaces (for HDF purpose).
 *
 * @var LRC_configTypes cs[9]
 * @brief Define allowable datatypes.
 *
 * @var int numCT
 * @brief Number of allowed datatypes (the same as number of elements in cs, @see cs).
 */
  
	char* inif; 
  hid_t file;
  char* sep = "=";
  char* comm = "#";

	int nprocs = 0;
  int nbody = 0;
  int dump = 0;
	float period = 0.0;
  float epoch = 0.0;
  int xres = 0;
  int yres = 0;
	
  int k = 0;
	int i = 0;
  int opts = 0;
  int h5opts = 0;

  LRC_configNamespace* cs;
  LRC_configNamespace* cc;

  LRC_configTypes ct[9] = {
    {"default", "inidata", LRC_CHAR},
    {"default", "fs", LRC_CHAR},
    {"default", "nprocs", LRC_INT},
    {"default", "bodies", LRC_INT},
    {"logs", "dump", LRC_INT},
    {"logs", "period", LRC_DOUBLE},
    {"logs", "epoch", LRC_FLOAT},
    {"farm", "xres", LRC_INT},
    {"farm", "yres", LRC_INT}
  };
  int numCT = 9;
	
  /* We create HDF5 file */ 
   file = H5Fopen("h5config.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
  
  /* Read hdf5 config file */
   h5opts = LRC_HDF5Parser(file, ct, numCT); 

   printf("H5OPTS = %d\n", h5opts); 
  
  /* Print all options */
   LRC_printAll(); 
 
  /* Finally, we release resources.*/
   H5Fclose(file);
   LRC_cleanup();
	
  return 0;
}


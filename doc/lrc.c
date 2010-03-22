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

  char* ascii = "ascii.config";

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
	
  inif = "lrc-config"; //input file
	
  printf("\n");
  
  /* Read config file. LRC will return number of namespaces. */
  opts = LRC_parseASCIIConfig(inif, sep, comm, ct, numCT);
  
  /* You can handle any errors in the way You like. */
  if(opts < 0) exit(1);

  /* Here we just print all options using LRC_printAll() */
	printf("\nALL OPTIONS [%d]: \n", opts);
  LRC_printAll();

  /* We create HDF5 file */ 
  file = H5Fcreate("h5config.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  
  /* Copy config file to hdf5 file */
  LRC_HDF5writer(file);

  /* Finally, we release resources.*/
  H5Fclose(file);

  /* Write ASCII config */
  LRC_writeASCIIConfig(ascii, sep, comm, opts);

  LRC_cleanup();
	
	/* The tricky part is to do some conversions.
   * You need to handle this by hand, because we don't know
   * what variables You use. */
	/*printf("\nCONVERTED OPTIONS:\n");
	for(i = 0; i < opts; i++){
    if(strcmp(cs[i].space,"default") == 0 || strcmp(cs[i].space, "logs") == 0){
		  for(k = 0; k < cs[i].num; k++){
			  if(strcmp(cs[i].options[k].name,"period") == 0) period = atof(cs[i].options[k].value);  
			  if(strcmp(cs[i].options[k].name,"nprocs") == 0) nprocs = atoi(cs[i].options[k].value); 
			  if(strcmp(cs[i].options[k].name,"bodies") == 0) nbody = atoi(cs[i].options[k].value); 
			  if(strcmp(cs[i].options[k].name,"dump") == 0) dump = atoi(cs[i].options[k].value); 
			  if(strcmp(cs[i].options[k].name,"epoch") == 0) epoch = atof(cs[i].options[k].value); 
		  }
	  }
    if(strcmp(cs[i].space,"map") == 0){
		  for(k = 0; k < cs[i].num; k++){
			  if(strcmp(cs[i].options[k].name,"xres") == 0) xres = atoi(cs[i].options[k].value);  
			  if(strcmp(cs[i].options[k].name,"yres") == 0) yres = atoi(cs[i].options[k].value); 
      }
    }
*/

	 /* Here we just print options. */
  /*  if(strcmp(cs[i].space,"default") == 0 || strcmp(cs[i].space, "logs") == 0){
		printf("Namespace [%s]:\n",cs[i].space);
		for(k = 0; k < cs[i].num; k++){
			printf("%s\t = \t",cs[i].options[k].name);
			if (strcmp(cs[i].options[k].name,"period") == 0) printf("%.6f\n",period);
			if (strcmp(cs[i].options[k].name,"inidata") == 0) printf("%s\n",cs[i].options[k].value);
			if (strcmp(cs[i].options[k].name,"nprocs") == 0) printf("%d\n",nprocs);
			if (strcmp(cs[i].options[k].name,"bodies") == 0) printf("%d\n",nbody);
			if (strcmp(cs[i].options[k].name,"dump") == 0) printf("%d\n",dump);
			if (strcmp(cs[i].options[k].name,"epoch") == 0) printf("%.6f\n",epoch);
		}
    
		printf("\n");
    }
    if(strcmp(cs[i].space,"map") == 0){
		printf("Namespace [%s]:\n",cs[i].space);
		for(k = 0; k < cs[i].num; k++){
			printf("%s\t = \t",cs[i].options[k].name);
			if (strcmp(cs[i].options[k].name,"xres") == 0) printf("%d\n",xres);
			if (strcmp(cs[i].options[k].name,"yres") == 0) printf("%d\n",yres);
    }
    }

  }
	printf("\n");
*/

  return 0;
}


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

#if HAVE_HDF5_H
	#include "libreadconfig_hdf5.h"
	#include "hdf5.h"
#endif

int main(int argc, char* argv[]){

	FILE* inif;
	int opts = 0, nms = 0;
  char* sep = "=";
  char* comm = "#";
  char* optvalue;

  LRC_configNamespace* head;
  LRC_configNamespace* current;

  char convstr[1024];

  /* Our data */
  int nbodies;
  float epoch;
  double period;


#if HAVE_HDF5_H
	hid_t fileid;
#endif

	/* Assign defaults */
  LRC_configDefaults ct[] = {
    {"default", "inidata", "test.dat", LRC_STRING},
    {"default", "nprocs", "4", LRC_INT},
    {"default", "bodies", "7", LRC_INT},
    {"logs", "dump", "100", LRC_INT},
    {"logs", "period", "23.47", LRC_DOUBLE},
    {"logs", "epoch", "2003.0", LRC_FLOAT},
    {"farm", "xres", "222", LRC_INT},
    {"farm", "yres", "444", LRC_INT},
    LRC_OPTIONS_END
  };
	
	/* LRC_assignDefaults is required 
   * This will create a linked list with config options
   * */
  head = LRC_assignDefaults(ct); 

	printf("\nDefault configuration:\n\n");

  /* LRC_printAll
   * This function can be used at any time
   */
	LRC_printAll(head);

	/* Parse ASCII config file. 
   * Will override defaults and ignore any option not included in defaults
   * */
  printf("B-A\n");
	inif = fopen("lrc-config","ro");
  if(inif != NULL){
	  nms = LRC_ASCIIParser(inif, sep, comm, head);
  printf("B-B\n");
  }else{
    perror("Error opening file: ");
    exit(-1);
  }
	fclose(inif);
  printf("B-C\n");

  /* LRC_modifyOption
   * This will modify the value and type of given option
   */
	LRC_modifyOption("logs", "dump","234", LRC_INT, head);
  printf("B-D\n");

	/* Write new config file */
	inif = fopen("lrc-ascii", "w");
  if(inif != NULL){
	  LRC_ASCIIWriter(inif, sep, comm, head);
  printf("B-E\n");
  }else{
    perror("Error opening file for write: ");
    exit(-1);
  }
	fclose(inif);

#if HAVE_HDF5_H
	/* Write HDF5 file */
  fileid = H5Fcreate("lrc-hdf-test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  LRC_HDF5Writer(fileid, head);
  H5Fclose(fileid);
  
  /* Reopen and read HDF5 config */
  fileid = H5Fopen("lrc-hdf-test.h5", H5F_ACC_RDONLY, H5P_DEFAULT);
  nms = LRC_HDF5Parser(fileid, head);

  printf("\nHDF5 config:\n\n");
  LRC_printAll(head);
  H5Fclose(fileid);
  
#endif

  /* Print all options */
	printf("\nFinal configuration:\n\n");
  LRC_printAll(head); 

  /* We need to do some conversions */
  nbodies = LRC_option2int("default","bodies",head);
  period = LRC_option2double("logs","period",head);
  epoch = LRC_option2float("logs","epoch",head);

  printf("Options convertions: \n");
  printf("NBODIES: %d\n", nbodies);
  printf("PERIOD: %lf\n", period);
  printf("EPOCH: %f\n", epoch);

  /* Itoa implementation
   * For ints convert to (int*) */
  LRC_itoa(convstr, nbodies, LRC_INT);
  printf("ITOA? Int: %d String %s\n", nbodies, convstr);
  
  /* For floats, pass the address of the variable */
  //LRC_itoa(convstr, &epoch, LRC_FLOAT);
  //printf("FloatTOA? Float: %f String %s\n", epoch, convstr);

  /* For doubles, pass the address of the variable */
  //LRC_itoa(convstr, &period, LRC_DOUBLE);
  //printf("DoubleTOA? Double: %f String %s\n", period, convstr);

  /* We can get also single string value */
  optvalue = LRC_getOptionValue("default", "bodies",head);
  printf("Get Option Value: %s", optvalue);
  
  /* We can count options
   * In fact, LRC_printAll uses this function too
   * */
  opts = LRC_countOptions("default",head) + 
    LRC_countOptions("logs",head) +
    LRC_countOptions("farm",head);

  printf("\nStats: %d namespaces and %d options\n", nms, opts);
  
  /* LRC_cleanup is required */
 	LRC_cleanup(head);
	
  return 0;
}


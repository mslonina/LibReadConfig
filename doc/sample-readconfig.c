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
#include "libreadconfig.h"


int main(int argc, char* argv[]){

	char* inif; char* sep = "="; char* comm = "#"; 
	int nprocs = 0; int nbody = 0; int dump = 0;
	int k = 0;
	float period = 0.0, epoch = 0.0;
	
	int i = 0, opts = 0;
  int xres = 0; int yres = 0;

  LRC_configNamespace cs[50];

  LRC_configTypes ct[] = {
    {"inidata", LRC_CHAR},
    {"fs", LRC_CHAR},
    {"nprocs", LRC_INT},
    {"bodies", LRC_INT},
    {"dump", LRC_INT},
    {"period", LRC_DOUBLE},
    {"epoch", LRC_FLOAT},
    {"xres", LRC_INT},
    {"yres", LRC_INT}
  };
  int numCT = 9;
	
  inif = "sample-config"; //input file
	printf("\n");
  opts = LRC_parseConfigFile(inif, sep, comm, cs, ct, numCT);
  if(opts < 0) exit(1);

	printf("\nALL OPTIONS [%d]: \n", opts);
  LRC_printAll(opts, cs);

	/**
	 * now tricky part -- do some conversions
	 * you need to handle this by hand
	 */
	printf("\nCONVERTED OPTIONS:\n");
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


	/**
	 * here we just print options 
	 */
    if(strcmp(cs[i].space,"default") == 0 || strcmp(cs[i].space, "logs") == 0){
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
	return 0;
}

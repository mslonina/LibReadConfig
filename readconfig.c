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

configOptions options[MAX_OPTIONS_NUM];
configNamespace configSpace[MAX_CONFIG_SIZE];

int main(int argc, char* argv[]){

	char* inif; char* sep = "="; char* comm = "#"; 
	int nprocs = 0; int nbody = 0; int dump = 0;
	int k = 0;
	float period = 0.0, epoch = 0.0;
	
	int i = 0, opts = 0;
  int xres = 0; int yres = 0;

	inif = "config"; //input file
	printf("\n");
  opts = parseConfigFile(inif, sep, comm);

	/**
	 * now tricky part -- do some conversions
	 * you need to handle this by hand
	 */
	for(i = 0; i < opts; i++){
    if(strcmp(configSpace[i].space,"default") == 0 || strcmp(configSpace[i].space, "logs") == 0){
		  for(k = 0; k < configSpace[i].num; k++){
			  if(strcmp(configSpace[i].options[k].name,"period") == 0) period = atof(configSpace[i].options[k].value);  
			  if(strcmp(configSpace[i].options[k].name,"nprocs") == 0) nprocs = atoi(configSpace[i].options[k].value); 
			  if(strcmp(configSpace[i].options[k].name,"bodies") == 0) nbody = atoi(configSpace[i].options[k].value); 
			  if(strcmp(configSpace[i].options[k].name,"dump") == 0) dump = atoi(configSpace[i].options[k].value); 
			  if(strcmp(configSpace[i].options[k].name,"epoch") == 0) epoch = atof(configSpace[i].options[k].value); 
		  }
	  }
    if(strcmp(configSpace[i].space,"map") == 0){
		  for(k = 0; k < configSpace[i].num; k++){
			  if(strcmp(configSpace[i].options[k].name,"xres") == 0) xres = atoi(configSpace[i].options[k].value);  
			  if(strcmp(configSpace[i].options[k].name,"yres") == 0) yres = atoi(configSpace[i].options[k].value); 
      }
    }

  }

	printf("\nCONVERTED OPTIONS:\n");
	/**
	 * here we just print options 
	 */
	for(i = 0; i < opts; i++){
    if(strcmp(configSpace[i].space,"default") == 0 || strcmp(configSpace[i].space, "logs") == 0){
		printf("Namespace [%s]:\n",configSpace[i].space);
		for(k = 0; k < configSpace[i].num; k++){
			printf("%s\t = \t",configSpace[i].options[k].name);
			if (strcmp(configSpace[i].options[k].name,"period") == 0) printf("%.6f\n",period);
			if (strcmp(configSpace[i].options[k].name,"inidata") == 0) printf("%s\n",configSpace[i].options[k].value);
			if (strcmp(configSpace[i].options[k].name,"nprocs") == 0) printf("%d\n",nprocs);
			if (strcmp(configSpace[i].options[k].name,"bodies") == 0) printf("%d\n",nbody);
			if (strcmp(configSpace[i].options[k].name,"dump") == 0) printf("%d\n",dump);
			if (strcmp(configSpace[i].options[k].name,"epoch") == 0) printf("%.6f\n",epoch);
		}
    
		printf("\n");
    }
    if(strcmp(configSpace[i].space,"map") == 0){
		printf("Namespace [%s]:\n",configSpace[i].space);
		for(k = 0; k < configSpace[i].num; k++){
			printf("%s\t = \t",configSpace[i].options[k].name);
			if (strcmp(configSpace[i].options[k].name,"xres") == 0) printf("%d\n",xres);
			if (strcmp(configSpace[i].options[k].name,"yres") == 0) printf("%d\n",yres);
    }
    }
	}

		printf("\nALL OPTIONS: \n");
  printAll(opts);
	printf("\n");
	return 0;
}

/**
 * ReadConfig -- config file parser
 * mariusz slonina <mariusz.slonina@gmail.com>
 * last modified: 05/01/2010
 * (c) 2009 - 2010
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
#include "libreadconfig.h"

/**
 * configError(
 *  int j
 *  char *m
 *  )
 *
 * prints errors (m) in config file with number of line j
 */
void configError(int j,char *m){
  
  printf("%s\nLine %d: %s\n", E_CONFIG_SYNTAX, j, m);

#ifdef OMPI_MPI_H
  MPI_Finalize();
#else
  exit(1);
#endif
}

/**
 *  TRIM.C - Remove leading, trailing, & excess embedded spaces
 *
 *  public domain by Bob Stout
 *  http://www.cs.umu.se/~isak/snippets/trim.c
 */

#define NUL '\0'

char *trim(char *str){

  char *ibuf = str, *obuf = str;
  int i = 0, cnt = 0;

  /**
   *  Trap NULL
   */
  if (str){
 
    /**
     *  Remove leading spaces (from RMLEAD.C)
     */
    for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
      ;
    if (str != ibuf)
      memmove(str, ibuf, ibuf - str);

    /**
     *  Collapse embedded spaces (from LV1WS.C)
     */
    while (*ibuf){
      if (isspace(*ibuf) && cnt) ibuf++;
        else{
          if (!isspace(*ibuf)) cnt = 0;
            else{
              *ibuf = ' ';
              cnt = 1;
              }
            obuf[i++] = *ibuf++;
            }
          }
          obuf[i] = NUL;

    /**
     *  Remove trailing spaces (from RMTRAIL.C)
     */
     while (--i >= 0) { if (!isspace(obuf[i])) break;}
     obuf[++i] = NUL;
    }

  return str;
}

/**
 * check namespace and trim it
 */
char* nametrim(char* l, int j){

  int len = 0;

  len = strlen(l);

  if(l[len-1] != ']') configError(j,E_MISSING_BRACKET);

  /* quick and dirty solution using trim function above */  
  l[0] = ' ';
  l[len-1] = ' ';
  l = trim(l);  

  return l;
}

/**
 * count number of separators in line
 */
int charcount(char *l, char* s){
  
  int i = 0; int sep = 0; int len = 0; 

  len = strlen(l);

  for (i = 0; i < len; i++){
    if((int) *s == l[i] ) sep++;
  }
  
  return sep;
}

/**
 * CONFIG FILE PARSER
 */

/**
 * parsefile (
 *  FILE* read, 
 *  char* SEP, 
 *  char* COMM
 *  )
 *
 * reads config namespaces, vars names and values into global options structure
 * and returns number of config vars
 */
int libreadconfig_parsefile(FILE* read, char* SEP, char* COMM, configNamespace* configSpace, configTypes* ct, int numCT){
  
  int i = 0; int j = 0; int n = 0; int sepc = 0;
  char* line; char l[MAX_LINE_LENGTH]; char* b; char* c;
  int ret = 0;

  while(n < MAX_CONFIG_SIZE){
        
    j++; //count lines
  
    if((feof(read) != 0)) break;
    line = fgets(l, MAX_LINE_LENGTH, read);
    
    /**
     * skip blank lines and any NULL
     */
    if (line == NULL) break;
    if (line[0] == '\n') continue;
    
    /**
     * now we have to trim leading and trailing spaces etc.
     */
    line = trim(line);

    /**
     * check for full line comments and skip them
     */
    if (strspn(line, COMM) > 0) continue;
    
    /**
     * check for separator at the begining
     */
    if (strspn(line, SEP) > 0) configError(j,E_MISSING_VAR);

    /**
     * first split var/value and inline comments
     * trim leading and trailing spaces
     */
    b = trim(strtok(line,COMM));

    /**
     * check for namespaces
     */
    if (b[0] == '['){
      b = nametrim(b,j);
      strcpy(configSpace[n].space,b); 
      i = 0;
      n++;
      continue;
    }
  
    /**
     * check if in the var/value string the separator exist
     */
    if(strstr(b,SEP) == NULL) configError(j,E_MISSING_SEP); 
    
    /**
     * check some special case:
     * we have separator, but no value
     */
    if((strlen(b) - 1) == strcspn(b,SEP)) configError(j,E_MISSING_VAL);

    /**
     * we allow to have only one separator in line
     */
    sepc = charcount(b, SEP);
    if(sepc > 1) configError(j,E_TOOMANY_SEP);
    
    /**
     * ok, now we are prepared
     */     
    c = trim(strtok(b,SEP));
    
    strcpy(configSpace[n-1].options[i].name,c);
  
    while (c!=NULL){
      if (c[0] == '\n') break;
      strcpy(configSpace[n-1].options[i].value,c);
      c = trim(strtok(NULL,"\n"));
    }  
    ret = matchType(configSpace[n-1].options[i].name, configSpace[n-1].options[i].value, ct, numCT);
    if(ret == 99) configError(j, E_WRONG_INPUT);

    configSpace[n-1].options[i].type = ret;

    i++;
    configSpace[n-1].num = i;
  }

  return n;
}

/**
 * prints all options
 */
void libreadconfig_printAll(int n, configNamespace* configSpace){
  int i = 0; int k = 0;
  for (i = 0; i < n; i++){
		printf("Namespace [%s]:\n",configSpace[i].space);
    for (k = 0; k < configSpace[i].num; k++){
			printf("\t%s = %s [type %d]\n",
          configSpace[i].options[k].name,
          configSpace[i].options[k].value,
          configSpace[i].options[k].type
          );
    }
    printf("\n");
  }
}

/**
 * reads config file -- wrapper function
 */
int parseConfigFile(char* inif, char* sep, char* comm, configNamespace* configSpace, configTypes* ct, int numCT){
  
  FILE* read;
  int opts;

	read = fopen(inif,"r");
	if(read != NULL){
		opts = libreadconfig_parsefile(read, sep, comm, configSpace, ct, numCT); //read and parse config file
	}else{
		perror("Error opening config file:");
		exit(1);
	}
	fclose(read);
	if(opts == 0){
		printf("Config file seems to be empty.\n");
		exit(0);
	}
  return opts;
}

int matchType(char* varname, char* value, configTypes* ct, int numCT){
  
  int i = 0;

  while(i < numCT){
    if(strcmp(ct[i].name,varname) == 0){
    //  printf("Varname: %s -> %s ", varname, value);
      if(checkType(value, ct[i].type) != 0) 
        return 99;
      else
        return ct[i].type;
    }
      i++;
  }
  return 0;

}

/**
 * Check type of the value 
 * Return 0 if the type is correct, 1 otherwise
 */
int checkType(char* value, int type){
  
  int i = 0, ret = 0, k = 0;
  char *p;

  switch(type){
    
    case RC_INT:
      for(i = 0; i < strlen(value); i++){
        if(isdigit(value[i]) || value[0] == '-'){ 
          ret = 0;
        }else{
          ret = 1;
          break;
        }
      }
    //  if(ret == 0) printf("is INT\n");
    //  if(ret == 1) printf("is NOT INT (followed by %d)\n", value[i]);
      break;

    case RC_FLOAT:
      k = strtol(value, &p, 10);
      if(value[0] != '\n' && (*p == '\n' || *p != '\0')){
      //  printf("is FLOAT\n");
        ret = 0;
      }else{
      //  printf("is NOT FLOAT (followed by %s)\n", p);
        ret = 1;
      }
      break;

    case RC_DOUBLE:
      k = strtol(value, &p, 10);
      if(value[0] != '\n' && (*p == '\n' || *p != '\0')){
      //  printf("is DOUBLE\n");
        ret = 0;
      }else{
      //  printf("is NOT DOUBLE (followed by %s)\n", p);
        ret = 1;
      }
      break;

    case RC_CHAR:
      for(i = 0; i < strlen(value); i++){
        if(isalpha(value[i]) || isallowed(value[i]) == 0){
          ret = 0;
        }else{
          ret = 1;
          break;
        }
      }
     // if(ret == 0) printf("is CHAR\n");
     // if(ret == 1) printf("is NOT CHAR (followed by %d)\n",value[i]);
      break;
  
    default:
      break;

  }

  return ret;

}

/* Check if c is one of the allowed chars */
int isallowed(int c){

  char* allowed = "_-. ";
  int i = 0;

  for(i = 0; i < strlen(allowed); i++){
    if(c == allowed[i]) return 0;
  }

  return 1;
}

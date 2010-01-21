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
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include "libreadconfig.h"

/**
 * Internal functions
 */
void LRC_configError(int, char*);
char* LRC_trim(char*);
char* LRC_nameTrim(char*);
int LRC_charCount(char*, char*);
int LRC_matchType(char*, char*, LRC_configTypes*, int numCT);
int LRC_checkType(char*, int);
int LRC_isAllowed(int);
int LRC_checkName(char*, LRC_configTypes*, int numCT);

/**
 * configError(
 *  int j
 *  char *m
 *  )
 *
 * prints errors (m) in config file with number of line j
 */
void LRC_configError(int j,char *m){
  
  printf("%s\nLine %d: %s\n", E_CONFIG_SYNTAX, j, m);

}

/**
 *  TRIM.C - Remove leading, trailing, & excess embedded spaces
 *
 *  public domain by Bob Stout
 *  http://www.cs.umu.se/~isak/snippets/trim.c
 */

#define NUL '\0'

char* LRC_trim(char *str){

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
 * Check namespace and trim it
 */
char* LRC_nameTrim(char* l){

  int len = 0;

  len = strlen(l);

  /* Quick and dirty solution using trim function above */  
  l[0] = ' ';
  l[len-1] = ' ';
  l = LRC_trim(l);  

  return l;
}

/**
 * Count the number of separators in line
 */
int LRC_charCount(char *l, char* s){
  
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
 * and returns number of config vars, or -1 on failure
 */
int LRC_parseFile(FILE* read, char* SEP, char* COMM, LRC_configNamespace* configSpace, LRC_configTypes* ct, int numCT){
  
  int i = 0; int j = 0; int sepc = 0; int n = 0;
  char* line; char l[MAX_LINE_LENGTH]; char* b; char* c;
  int ret = 0; int err = 0;

  while(n < MAX_CONFIG_SIZE){
        
    j++; //count lines
  
    if((feof(read) != 0)) break;
    line = fgets(l, MAX_LINE_LENGTH, read);
    
    /**
     * Skip blank lines and any NULL
     */
    if (line == NULL) break;
    if (line[0] == '\n') continue;
    
    /**
     * Now we have to trim leading and trailing spaces etc.
     */
    line = LRC_trim(line);

    /**
     * Check for full line comments and skip them
     */
    if (strspn(line, COMM) > 0) continue;
    
    /**
     * Check for separator at the begining
     */
    if (strspn(line, SEP) > 0){
      LRC_configError(j,E_MISSING_VAR); 
      goto failure;
    }

    /**
     * First split var/value and inline comments
     * Trim leading and trailing spaces
     */
    b = LRC_trim(strtok(line,COMM));

    /**
     * Check for namespaces
     * TODO: check if namespace is allowed
     */
    if (b[0] == '['){
      if(b[strlen(b)-1] != ']'){
        LRC_configError(j,E_MISSING_BRACKET); 
        goto failure;
      }

      b = LRC_nameTrim(b);
      strcpy(configSpace[n].space,b); 
      i = 0;
      n++;
      continue;
    }
  
    /**
     * Check if in the var/value string the separator exist
     */
    if(strstr(b,SEP) == NULL){
      LRC_configError(j,E_MISSING_SEP); 
      goto failure;
    }
    
    /**
     * Check some special case:
     * we have separator, but no value
     */
    if((strlen(b) - 1) == strcspn(b,SEP)){
      LRC_configError(j,E_MISSING_VAL); 
      goto failure;
    }

    /**
     * We allow to have only one separator in line
     */
    sepc = LRC_charCount(b, SEP);
    if(sepc > 1){
      LRC_configError(j,E_TOOMANY_SEP); 
      goto failure;
    }
    
    /**
     * Ok, now we are prepared
     */     
    c = LRC_trim(strtok(b,SEP));

    /**
     * Check if variable is allowed
     */
    if(LRC_checkName(c, ct, numCT) < 0){
      LRC_configError(j,E_UNKNOWN_VAR); 
      goto failure;
    }

    strcpy(configSpace[n-1].options[i].name,c);
  
    while (c!=NULL){
      if (c[0] == '\n') break;
      strcpy(configSpace[n-1].options[i].value,c);
      c = LRC_trim(strtok(NULL,"\n"));
    }  

    ret = LRC_matchType(configSpace[n-1].options[i].name, configSpace[n-1].options[i].value, ct, numCT);
    if(ret < 0){
      LRC_configError(j, E_WRONG_INPUT); 
      goto failure;
    }

    configSpace[n-1].options[i].type = ret;

    i++;
    configSpace[n-1].num = i;
  }
  return n;

failure:
  return -1;
}

/**
 * Prints all options
 */
void LRC_printAll(int n, LRC_configNamespace* configSpace){
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
 * Reads config file -- wrapper function
 * Returns number of options, or -1 on failure
 */
int LRC_parseConfigFile(char* inif, char* sep, char* comm, LRC_configNamespace* configSpace, LRC_configTypes* ct, int numCT){
  
  FILE* read;
  int opts;

	read = fopen(inif,"r");
	if(read != NULL){
		opts = LRC_parseFile(read, sep, comm, configSpace, ct, numCT); //read and parse config file
	}else{
		perror("Error opening config file:");
		return -1;
	}
	fclose(read);
  
  return opts;
}

/**
 * Checks if variable is allowed
 */
int LRC_checkName(char* space, char* varname, LRC_configTypes* ct, int numCT){
   
  int i = 0, count = 0;

  for(i = 0; i < numCT; i++){
    if(strcmp(space, ct[i].space) == 0){
      if(strcmp(varname, ct[i].name) == 0) count++;
    {
  }
  
  if(count > 0) 
    return 0;
  else
    return -1;

}

/**
 * Match input type. Returns type (integer value) or -1 on failure 
 */
int LRC_matchType(char* varname, char* value, LRC_configTypes* ct, int numCT){
  
  int i = 0;

  while(i < numCT){
    if(strcmp(ct[i].name,varname) == 0){
      if(LRC_checkType(value, ct[i].type) != 0){ 
//        printf("Error: %s -> %s ", varname, value);
        return -1;
      }else{
        return ct[i].type;
      }
    }
      i++;
  }
  return 0;

}

/**
 * Check type of the value 
 * Return 0 if the type is correct, -1 otherwise
 */
int LRC_checkType(char* value, int type){
  
  int i = 0, ret = 0, k = 0;
  char *p;

  switch(type){
    
    case LRC_INT:
      for(i = 0; i < strlen(value); i++){
        if(isdigit(value[i]) || value[0] == '-'){ 
          ret = 0;
        }else{
//          printf("is not INT (followed by %d)\n", value[i]);
          ret = -1;
          break;
        }
      }
      break;

    case LRC_FLOAT:
      k = strtol(value, &p, 10);
      if(value[0] != '\n' && (*p == '\n' || *p != '\0')){
        ret = 0;
      }else{
//        printf("is not FLOAT (followed by %s)\n", p);
        ret = -1;
      }
      break;

    case LRC_DOUBLE:
      k = strtol(value, &p, 10);
      if(value[0] != '\n' && (*p == '\n' || *p != '\0')){
        ret = 0;
      }else{
//        printf("is not DOUBLE (followed by %s)\n", p);
        ret = -1;
      }
      break;

    case LRC_CHAR:
      for(i = 0; i < strlen(value); i++){
        if(isalpha(value[i]) || LRC_isAllowed(value[i]) == 0){
          ret = 0;
        }else{
//          printf("is not STRING (followed by %d)\n",value[i]);
          ret = -1;
          break;
        }
      }
      break;
  
    default:
      break;

  }

  return ret;

}

/* Check if c is one of the allowed chars */
int LRC_isAllowed(int c){

  char* allowed = "_-. ";
  int i = 0;

  for(i = 0; i < strlen(allowed); i++){
    if(c == allowed[i]) return 0;
  }

  return 1;
}

/* Convert types. Returns 0 on success, -1 on failure */
int LRC_convertTypes(){



}

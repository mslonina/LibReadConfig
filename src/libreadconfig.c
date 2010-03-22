/*
 * LIBREADCONFIG Copyright (c) 2010, Mariusz Slonina (Nicolaus Copernicus University)
 * All rights reserved.
 * 
 * This file is part of LIBREADCONFIG code. 
 *
 * LIBREADCONFIG was created to help in handling config files by providing common
 * tools and including HDF5 support. The code was released in in belief it will be 
 * useful. If you are going to use this code, or its parts, please consider referring 
 * to the authors either by the website or the user guide reference.
 *
 * http://mechanics.astri.umk.pl/projects/libreadconfig
 *
 * User guide should be provided with the package or 
 * http://mechanics.astri.umk.pl/projects/mechanic/libreadconfig_userguide.pdf
 *
 * Redistribution and use in source and binary forms, 
 * with or without modification, are permitted provided 
 * that the following conditions are met:
 * 
 *  - Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 *  - Neither the name of the Nicolaus Copernicus University nor the names of 
 *    its contributors may be used to endorse or promote products derived from 
 *    this software without specific prior written permission.
 *  
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 */

/**
 * @file libreadconfig.c
 * @brief Functions for reading/writing config files.
 *
 * @mainpage
 * LibReadConfig is a standalone solution for handling config files.
 *
 * Features:
 * - inline/full-line comments
 * - simple error checking, input value checking
 * - ASCII and HDF5 config file read/write support
 * - customizable separator and comment marks
 * - namespaces
 * 
 * @example sample-config
 * @example sample-makefile
 * @example sample-readconfig.c
 */

#include "libreadconfig.h"
#if HAVE_HDF5_H
  #include "libreadconfig_hdf5.h"
#endif

/**
 * @defgroup LRC_internals Helper functions
 * @{
 */
void LRC_message(int line, int type, char* message);
char* LRC_trim(char*);
char* LRC_nameTrim(char*);
int LRC_charCount(char*, char*);
int LRC_matchType(char*, char*, LRC_configTypes*, int);
int LRC_checkType(char*, int);
int LRC_isAllowed(int);
int LRC_checkName(char*, LRC_configTypes*, int);
void LRC_newNamespace(char* cfg);

#if HAVE_HDF5_H
/**
 * @var typedef struct ccd_t
 * @brief Helper datatype used for HDF5 storage
 *
 * @param name
 *  Name of the variable
 * 
 * @param value
 *  Value of the variable
 *
 * @param type
 *  Type of the variable
 */
typedef struct{
  char* name;
  char* value;
  int type;
} ccd_t;
#endif

/**
 * @var LRC_configNamespace* head
 *  @brief Linked list head element
 *
 * @var LRC_configNamespace* current
 *  @brief Linked list current element
 */
LRC_configNamespace* head;
LRC_configNamespace* current;

/**
 * @fn void LRC_message(int j, int type, char* message)
 * @brief Prints error messages.
 * 
 * @param j
 *  The line in the config file where the error exist.
 *
 * @param type
 *  Type of the error.
 *
 * @param message
 *  Error message to print.
 */
void LRC_message(int line, int type, char* message){

  switch(type){
    case LRC_ERR_CONFIG_SYNTAX:
      printf("%s at line %d: %s\n", LRC_MSG_CONFIG_SYNTAX, line, message);
      break;
    case LRC_ERR_FILE_OPEN:
      printf("%s at line %d: %s\n", LRC_MSG_FILE_OPEN, line, message);
      break;
    case LRC_ERR_HDF:
      printf("%s at line %d: %s\n", LRC_MSG_HDF, line, message);
      break;
    default:
      break;
  }

}

/**
 * @fn char* LRC_trim(char* str)
 *
 * @brief Remove leading, trailing, & excess embedded spaces.
 *
 * Public domain by Bob Stout.
 * @see http://www.cs.umu.se/~isak/snippets/trim.c
 *
 * @param str
 *   String to trim.
 */
char* LRC_trim(char* str){

  char *ibuf = str, *obuf = str;
  int i = 0, cnt = 0;

  /* Trap NULL.*/
  if (str){
 
    /* Remove leading spaces (from RMLEAD.C).*/
    for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
      ;
    if (str != ibuf)
      memmove(str, ibuf, ibuf - str);

    /* Collapse embedded spaces (from LV1WS.C).*/
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
          obuf[i] = LRC_NULL;

     /* Remove trailing spaces (from RMTRAIL.C).*/
     while (--i >= 0) { if (!isspace(obuf[i])) break;}
     obuf[++i] = LRC_NULL;
    }

  return str;
}

/**
 * @fn char* LRC_nameTrim(char* l)
 * @brief Check namespace name and trim it.
 *
 * @param l
 *   Name of the namespace.
 *
 * @return
 *   Trimmed name of the namespace.
 */
char* LRC_nameTrim(char* l){

  int len = 0;

  len = strlen(l);

  /* Quick and dirty solution using trim function. */
  l[0] = ' ';
  l[len-1] = ' ';
  l = LRC_trim(l);  

  return l;
}

/**
 * @fn int LRC_charCount(char* l, char* s)
 * @brief Count the number of separators in the current line.
 *
 * @param l
 *   Current line.
 *
 * @param s
 *   The separator to count.
 *
 * @return
 *   Number of separators in the current line.
 */
int LRC_charCount(char* l, char* s){
  
  int i = 0; int sep = 0; int len = 0; 

  len = strlen(l);

  for (i = 0; i < len; i++){
    if((int) *s == l[i] ) sep++;
  }
  
  return sep;
}

/**
 * @fn int LRC_checkName(char* varname, LRC_configTypes* ct, int numCT)
 * @brief Checks if variable is allowed.
 *
 * @param varname
 *   The name of the variable to check.
 *
 * @param ct
 *   Pointer to the structure with allowed config datatypes.
 *
 * @param numCT
 *   Number of allowed config datatypes.
 *
 * @return
 *   0 on success, -1 otherwise.  
 */
int LRC_checkName(char* varname, LRC_configTypes* ct, int numCT){
   
  int i = 0, count = 0;

  for(i = 0; i < numCT; i++){
      if(strcmp(varname, ct[i].name) == 0) count++;
  }
  
  if(count > 0) 
    return 0;
  else
    return -1;

}

/**
 * @fn int LRC_matchType(char* varname, char* value, LRC_configTypes* ct, int numCT)
 * @brief Match input type.
 *
 * @param varname
 *   Variable name.
 *
 * @param value
 *   The value of the variable.
 *
 * @param ct
 *   Pointer to the structure with allowed config datatypes.
 *
 * @param numCT
 *   Number of config datatypes allowed.
 *   
 * @return
 *   Returns type (integer value) on success, -1 otherwise.
 *   @see LRC_configTypes
 */
int LRC_matchType(char* varname, char* value, LRC_configTypes* ct, int numCT){
  
  int i = 0;

  while(i < numCT){
    if(strcmp(ct[i].name,varname) == 0){
      if(LRC_checkType(value, ct[i].type) != 0){ 
/*        printf("Error: %s -> %s ", varname, value); */
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
 * @fn int LRC_checkType(char* value, int type)
 * @brief Check type of the value.
 *
 * @param value
 *   The value to check.
 *
 * @param type
 *   The datatype for given value.
 *   @see LRC_configTypes
 *
 * @return
 *   0 on success, -1 otherwise.
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
/*          printf("is not INT (followed by %d)\n", value[i]);*/
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
/*        printf("is not FLOAT (followed by %s)\n", p);*/
        ret = -1;
      }
      break;

    case LRC_DOUBLE:
      k = strtol(value, &p, 10);
      if(value[0] != '\n' && (*p == '\n' || *p != '\0')){
        ret = 0;
      }else{
/*        printf("is not DOUBLE (followed by %s)\n", p);*/
        ret = -1;
      }
      break;

    case LRC_CHAR:
      for(i = 0; i < strlen(value); i++){
        if(isalpha(value[i]) || LRC_isAllowed(value[i]) == 0){
          ret = 0;
        }else{
/*          printf("is not STRING (followed by %d)\n",value[i]);*/
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

/** 
 * @fn int LRC_isAllowed(int c)
 * @brief Check if given char is one of the allowed chars.
 *
 * @param c
 *   The char to check.
 *
 * @return
 *   0 on succes, -1 otherwise.
 *
 * @todo
 *   Allow user to override allowed char string.
 */
int LRC_isAllowed(int c){

  char* allowed = "_-. ";
  int i = 0;

  for(i = 0; i < strlen(allowed); i++){
    if(c == allowed[i]) return 0;
 }

  return 1;
}

/**
 * @fn void newNamespace(char* cfg)
 * @brief Helper function for creating new namespaces
 */
void LRC_newNamespace(char* cfg){

  LRC_configNamespace* newNM;
  
  newNM = malloc(sizeof(LRC_configNamespace));

  if(head == NULL) head = newNM;
  else  current->next = newNM;

  current = newNM;
  newNM->space = malloc(strlen(cfg+1));
  strncpy(newNM->space, cfg, strlen(cfg));
  newNM->space[strlen(cfg)]=LRC_NULL;
  newNM->options = NULL;
  newNM->next = NULL;
}

/**
 * @}
 */

/**
 * @defgroup LRC_userAPI User API
 * @{
 * Public User API.
 */

/**
 *  @defgroup LRC_parser Parsers
 *  @{
 *  Currently there are two parsers available:
 *  - Text file parser @see LRC_ASCIIParser()
 *  - HDF5 file parser @see LRC_HDF5Parser()
 *
 *  @todo
 *    Add secondary separator support.
 */

/**
 *  Text parser
 *
 *  @fn int LRC_ASCIIParser(FILE* read, char* SEP, char* COMM, LRC_configTypes* ct, int numCT)
 *  @brief Reads config file, namespaces, variable names and values,
 *  into the options structure @see LRC_configNamespace.
 *
 *  @param read
 *    Handler of the config file to read.
 *    
 *  @param SEP
 *    The separator name/value.
 *
 *  @param COMM
 *    The comment mark.
 *
 *  @param ct
 *    Pointer to the structure with datatypes allowed in the config file.
 *
 *  @param numCT
 *    Number of datatypes allowed in the config file.
 *
 *  @return
 *    Number of namespaces found in the config file on success, -1 otherwise.
 *
 *  @todo
 *    Check if the namespace is allowed.
 */

int LRC_ASCIIParser(FILE* read, char* SEP, char* COMM, LRC_configTypes* ct, int numCT){
  
  int i = 0; int j = 0; int sepc = 0; int n = 0;
  char* line; char l[LRC_MAX_LINE_LENGTH]; char* b; char* c;
  char* name; char* value; 
  int ret = 0; 

  LRC_configOptions* newOP = NULL;
  LRC_configOptions* lastOP;

  size_t p = 0;
  size_t valuelen = 0;

  while(!feof(read)){
    
    /* Count lines */
    j++; 
  
    line = fgets(l, LRC_MAX_LINE_LENGTH, read);
    
    /* Skip blank lines and any NULL */
    if (line == NULL) break;
    if (line[0] == '\n') continue;
    
    /* Now we have to trim leading and trailing spaces etc */
    line = LRC_trim(line);

    /* Check for full line comments and skip them */
    if (strspn(line, COMM) > 0) continue;
    
    /* Check for the separator at the beginning */
    if (strspn(line, SEP) > 0){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_MISSING_VAR); 
      goto failure;
    }

    /* First split var/value and inline comments.
     * Trim leading and trailing spaces */
    b = LRC_trim(strtok(line,COMM));

    /* Check for namespaces */
    if (b[0] == '['){
      if(b[strlen(b)-1] != ']'){
        LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_MISSING_BRACKET); 
        goto failure;
      }

      b = LRC_nameTrim(b);
      LRC_newNamespace(b);
      i = 0;
      n++;

      continue;
    }
  
    /* If no namespace was specified return failure */
    if (current == NULL){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_NONAMESPACE);
      goto failure;
    }

    /* Check if in the var/value string the separator exist.*/
    if(strstr(b,SEP) == NULL){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_MISSING_SEP); 
      goto failure;
    }
    
    /* Check some special case:
     * we have separator, but no value */
    if((strlen(b) - 1) == strcspn(b,SEP)){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_MISSING_VAL); 
      goto failure;
    }

    /* We allow to have only one separator in line */
    sepc = LRC_charCount(b, SEP);
    if(sepc > 1){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_TOOMANY_SEP); 
      goto failure;
    }
    
    /* Ok, now we are prepared */
    c = LRC_trim(strtok(b,SEP));

    /* Check if variable is allowed.
     * We skip unknown variables silently */
    if(LRC_checkName(c, ct, numCT) < 0){
      /* LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_UNKNOWN_VAR); */
      continue;
    }

    p = strlen(c);
    newOP = malloc(sizeof(LRC_configOptions));
    name = malloc(p+1);
    strncpy(name, c, p);
    name[p] = LRC_NULL;
    newOP->name = name;

    while (c!=NULL){
      if (c[0] == '\n') break;

      valuelen = strlen(c);
      value = malloc(valuelen+1);
      strncpy(value, c, valuelen);
      value[valuelen] = LRC_NULL;

      newOP->value = value;
      c = LRC_trim(strtok(NULL,"\n"));
    }  

    newOP->next = NULL;
    ret = LRC_matchType(newOP->name, newOP->value, ct, numCT);
    if(ret < 0){
      LRC_message(j, LRC_ERR_CONFIG_SYNTAX, LRC_MSG_WRONG_INPUT); 
      goto failure;
    }

    newOP->type = ret;

    i++;
    current->num = i;

    if(current->options == NULL) 
      current->options = newOP;
    else{
     lastOP = current->options;
     while((lastOP->next) != NULL)
       lastOP = lastOP->next;
     lastOP->next = newOP;
    }

  }
  return n;

failure:
  return -1;
}

/**
 * @fn void LRC_cleanup()
 * @brief Cleanup assign pointers. This is required for proper memory managment.
 */
void LRC_cleanup(){

  LRC_configOptions* currentOP;
  LRC_configOptions* nextOP;
  LRC_configNamespace* nextNM;

  current = head;

  do{
    currentOP = current->options;
    do {
      if(currentOP){
        nextOP = currentOP->next;
        free(currentOP->value);
        free(currentOP->name);
        free(currentOP);
        currentOP = nextOP;
      }
    }while(currentOP);

  if(current){
    nextNM = current->next;
    free(current->space);
    free(current);
    current=nextNM;
  }
  }while(current);

  head = NULL;

}

#if HAVE_HDF5_H
/**
 * HDF5 parser 
 * 
 * @fn int LRC_HDF5Parser(hid_t file, LRC_configTypes* ct, int numCT)
 * @brief Parse config data stored in HDF5 files.
 *
 * @param file
 *   The handler of the file.
 *
 * @param cs
 *   Pointer to the structure of config data.
 *
 * @param ct
 *   Pointer to the structure with allowed datatypes.
 *
 * @param numCT
 *   Number of allowed datatypes.
 *
 * @return
 *   Number of read namespaces.
 *
 * @todo
 *   - Type checking.
 *   - Open, rather than recreate compound datatype.
 *
 */
int LRC_HDF5Parser(hid_t file, LRC_configTypes* ct, int numCT){
  
  hid_t group, dataset, dataspace;
  hid_t ccm_tid, name_dt, value_dt;
  herr_t status, info;
  H5G_info_t group_info;
  
  int numOfNM = 0, i = 0, k = 0;
  char link_name[LRC_MAX_LINE_LENGTH];
  ssize_t link;
  ssize_t nlen, vlen;
  hsize_t edims[1], emaxdims[1];

  LRC_configOptions* newOP = NULL;
  LRC_configOptions* lastOP;

  ccd_t* rdata;

  /* NULL global pointers */
  current = NULL;
  head = NULL;

  /* For future me: how to open compound data type and read it,
   * without rebuilding memtype? Is this possible? */

  /* Create variable length string datatype */
  name_dt = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(name_dt, H5T_VARIABLE);

  value_dt = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(value_dt, H5T_VARIABLE);

  /* Create compound datatype for the memory */
  ccm_tid = H5Tcreate(H5T_COMPOUND, sizeof(ccd_t));

  H5Tinsert(ccm_tid, "Name", HOFFSET(ccd_t, name), name_dt);
  H5Tinsert(ccm_tid, "Value", HOFFSET(ccd_t, value), value_dt);
  H5Tinsert(ccm_tid, "Type", HOFFSET(ccd_t, type), H5T_NATIVE_INT);

  /* Open config group */
  group = H5Gopen(file, LRC_CONFIG_GROUP, H5P_DEFAULT);

  /* Get group info */
  info = H5Gget_info(group, &group_info);

  /* Get number of opts (dataspaces) */
  numOfNM = group_info.nlinks;

  /* Iterate each dataspace and assign config values */
  for(i = 0; i < numOfNM; i++){

    /* Get name of dataspace -> the namespace */
    link = H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, link_name, LRC_MAX_LINE_LENGTH, H5P_DEFAULT);

    /* Get size of the table with config data */
    dataset = H5Dopen(group, link_name, H5P_DEFAULT);
    dataspace = H5Dget_space(dataset);
    H5Sget_simple_extent_dims(dataspace, edims, emaxdims);
    
    /* We will get all data first */
    rdata = malloc((int)edims[0]*sizeof(ccd_t));
    status = H5Dread(dataset, ccm_tid, H5S_ALL, H5S_ALL, H5P_DEFAULT, rdata);
    
    H5Sclose(dataspace);
    H5Dclose(dataset);

    /* Create new namespace */
    LRC_newNamespace(link_name);
    current->num = (int)edims[0];

    /* Assign values */
    for(k = 0; k < current->num; k++){
      newOP = malloc(sizeof(LRC_configOptions));

      nlen = strlen(rdata[k].name);
      newOP->name = malloc(nlen);
      strcpy(newOP->name, rdata[k].name);

      vlen = strlen(rdata->value);
      newOP->value = malloc(vlen);
      strcpy(newOP->value, rdata[k].value);

      newOP->type = rdata[k].type;
      
      newOP->next = NULL;

      if(current->options == NULL){
        current->options = newOP;
      }else{
        lastOP = current->options;
        while((lastOP->next) != NULL){
          lastOP = lastOP->next;
        }
        lastOP->next = newOP;
      }

    }

    free(rdata);
  }

  H5Tclose(ccm_tid);
  H5Gclose(group);
 
  return numOfNM;
}
#endif

/**
 * @}
 */

/**
 * @defgroup LRC_wrappers Wrappers
 * @{
 * Wrapper functions for reading/writing config files.
 */

/**
 * @fn void LRC_writeASCIIConfig(char* file, char* s, char* l, LRC_configNamespace* ct, int opts)
 * @brief Write config file. Wrapper for @see LRC_writeTextConfig() and @see LRC_writeHdfConfig()
 */
int LRC_writeASCIIConfig(char* file, char* sep, char* comm, int opts){

  FILE* write;
  write = fopen(file, "w");
  
  if(write != NULL){
    LRC_ASCIIwriter(write, sep, comm, opts);
  }else{
    perror("Error writing config file:");
    return -1;
  }

  fclose(write);
  return 0;
}

/**
 * @fn void LRC_ASCIIwriter(FILE*, char* sep, char* comm, int opts)
 * @brief Write ASCII config file.
 * @return
 *  Should return 0 on success, errcode otherwise
 */
int LRC_ASCIIwriter(FILE* write, char* sep, char* comm, int opts){

  LRC_configOptions* currentOP;
  LRC_configOptions* nextOP;
  LRC_configNamespace* nextNM;

  current = head;

  fprintf(write,"%s Written by LibReadConfig \n",comm);

  do{
    if(current){
      fprintf(write, "[%s]\n",current->space);
      currentOP = current->options; 
      do{
        if(currentOP){
          fprintf(write, "%s %s %s\n", currentOP->name, sep, currentOP->value);
          nextOP = currentOP->next;
          currentOP = nextOP;
        }
      }while(currentOP);
      
      nextNM = current->next;
      current = nextNM;
    }
    fprintf(write,"\n");
  }while(current);

  fprintf(write,"\n");
  return 0;
}

#if HAVE_HDF5_H
/**
 * @fn void LRC_HDF5writer(hid_t file)
 * @brief Write config values to hdf file.
 * 
 * @param file
 *   The handler of the file.
 *
 * @return
 *  Should return 0 on success, errcode otherwise
 *
 */
int LRC_HDF5writer(hid_t file){

  hid_t group, dataset, dataspace, memspace;
  hid_t ccm_tid, ccf_tid, name_dt, value_dt;
  hsize_t dims[2], dimsm[1], offset[2], count[2], stride[2];
  herr_t status;
  int k = 0;
  size_t nlen, vlen;

  LRC_configOptions* currentOP;
  LRC_configOptions* nextOP;
  LRC_configNamespace* nextNM;

  ccd_t* ccd;
  ccd = malloc(sizeof(ccd_t));

  group = H5Gcreate(file, LRC_CONFIG_GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

  current = head;

  /* Create variable length string datatype */
  name_dt = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(name_dt, H5T_VARIABLE);

  value_dt = H5Tcopy(H5T_C_S1);
  status = H5Tset_size(value_dt, H5T_VARIABLE);

  /* Create compound datatype for the memory */
  ccm_tid = H5Tcreate(H5T_COMPOUND, sizeof(ccd_t));

  H5Tinsert(ccm_tid, "Name", HOFFSET(ccd_t, name), name_dt);
  H5Tinsert(ccm_tid, "Value", HOFFSET(ccd_t, value), value_dt);
  H5Tinsert(ccm_tid, "Type", HOFFSET(ccd_t, type), H5T_NATIVE_INT);

  /* Create compound datatype for the file */
  ccf_tid = H5Tcreate(H5T_COMPOUND, 8 + sizeof(hvl_t) + sizeof(hvl_t));

  status = H5Tinsert(ccf_tid, "Name", 0, name_dt);
  status = H5Tinsert(ccf_tid, "Value", sizeof(hvl_t), value_dt);
  status = H5Tinsert(ccf_tid, "Type", sizeof(hvl_t) + sizeof(hvl_t),H5T_NATIVE_INT);

  /* Commit datatype */
  status = H5Tcommit(file, LRC_HDF5_DATATYPE, ccf_tid, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
      
  do{
   if(current){ 

      currentOP = current->options;
      dims[0] = (hsize_t)current->num;
      dims[1] = 1;
      dataspace = H5Screate_simple(1, dims, NULL);
    
      dataset = H5Dcreate(group, current->space, ccf_tid, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /* Write config data one by one in given namespace */
    k = 0;
    do{
      if(currentOP){ 

      /* Assign values */
      nlen = strlen(currentOP->name);
      ccd->name = malloc(nlen);
      strcpy(ccd->name, currentOP->name);
      
      vlen = strlen(currentOP->value);
      ccd->value = malloc(vlen);
      strcpy(ccd->value, currentOP->value);
      
      ccd->type = currentOP->type;

      /* Prepare HDF write */
      offset[0] = k; 
      offset[1] = 0;
      
      dimsm[0] = 1; 
      dimsm[1] = 1;
      
      count[0] = 1; 
      count[1] = 1;
      
      stride[0] = 1;
      stride[1] = 1;

      memspace = H5Screate_simple(1, dimsm, NULL);
      status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, NULL);

      status = H5Dwrite(dataset, ccm_tid, memspace, dataspace, H5P_DEFAULT, ccd);
      status = H5Sclose(memspace);

      free(ccd->name);
      free(ccd->value);
      nextOP = currentOP->next;
      currentOP = nextOP;
      k++;

      }
    }while(currentOP);

    status = H5Dclose(dataset);
    status = H5Sclose(dataspace);

    nextNM = current->next;
    current = nextNM;
   }
  }while(current);

  status = H5Gclose(group);
  status = H5Tclose(name_dt);
  status = H5Tclose(value_dt);
  status = H5Tclose(ccf_tid);
  status = H5Tclose(ccm_tid);

  free(ccd);

  return 0;
}
#endif

/**
 * @fn void LRC_printAll()
 * @brief Prints all options.
 */
void LRC_printAll(){

  LRC_configOptions* currentOP;
  LRC_configOptions* nextOP;
  LRC_configNamespace* nextNM;

  current = head;

  do{
    if(current){
      nextNM = current->next;
      printf("\n[%s][%d]\n",current->space, current->num);
      currentOP = current->options;
      do {
        if(currentOP){
          nextOP = currentOP->next;
          printf("%s = %s [type %d]\n", currentOP->name, currentOP->value, currentOP->type);
          currentOP = nextOP;
        }
      }while(currentOP);
      
      current=nextNM;
    }
    
  }while(current);

}

/**
 * @fn int LRC_parseASCIIConfig(char* inif, char* sep, char* comm, LRC_configTypes* ct, int numCT)
 * @brief Wrapper function.
 *
 * @param inif
 *   Name of the config file to read.
 *
 * @param sep
 *   The separator.
 *
 * @param comm
 *   The comment mark.
 * 
 * @param ct
 *   Pointer to allowed datatypes structure.
 *
 * @param numCT
 *   Number of allowed datatypes.
 *   
 * @return
 *    Number of namespaces found in the config file on success, -1 otherwise.
 *
 */
int LRC_parseASCIIConfig(char* inif, char* sep, char* comm, LRC_configTypes* ct, int numCT){
  
  FILE* read;
  int opts;

	read = fopen(inif,"r");
	if(read != NULL){
    
    /* Read and parse config file.*/
		opts = LRC_ASCIIParser(read, sep, comm, ct, numCT); 
	}else{
		perror("Error opening config file:");
		return -1;
	}
	fclose(read);
  
  return opts;
}

/**
 * @}
 */

/**
 * @}
 */

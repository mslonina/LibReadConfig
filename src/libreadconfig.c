/**
 * @mainpage
 * LibReadConfig -- config file parser
 *
 * @version 0.12
 *
 * @author mariusz slonina <mariusz.slonina@gmail.com>
 * @date 2010
 */
#include "libreadconfig.h"

/**
 * Internals
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
 * Prints error messages.
 * 
 * @param int j
 *   The line in the config file where the error exist.
 *
 * @param char* m
 *   Error message to print.
 * 
 */
void LRC_configError(int j,char* m){
  
  printf("%s\nLine %d: %s\n", E_CONFIG_SYNTAX, j, m);

}

/**
 *  Remove leading, trailing, & excess embedded spaces
 *
 *  Public domain by Bob Stout
 *  http://www.cs.umu.se/~isak/snippets/trim.c
 *
 *  @param char* str
 *    String to trim.
 */

#define NUL '\0'

char* LRC_trim(char* str){

  char *ibuf = str, *obuf = str;
  int i = 0, cnt = 0;

  // Trap NULL
  if (str){
 
    // Remove leading spaces (from RMLEAD.C)
    for (ibuf = str; *ibuf && isspace(*ibuf); ++ibuf)
      ;
    if (str != ibuf)
      memmove(str, ibuf, ibuf - str);

    // Collapse embedded spaces (from LV1WS.C)
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

     // Remove trailing spaces (from RMTRAIL.C)
     while (--i >= 0) { if (!isspace(obuf[i])) break;}
     obuf[++i] = NUL;
    }

  return str;
}

/**
 * Check namespace name and trim it.
 *
 * @param char* l
 *   Name of the namespace.
 *
 * @return
 *   Trimmed name of the namespace.
 */
char* LRC_nameTrim(char* l){

  int len = 0;

  len = strlen(l);

  // Quick and dirty solution using trim function above
  l[0] = ' ';
  l[len-1] = ' ';
  l = LRC_trim(l);  

  return l;
}

/**
 * Count the number of separators in current line.
 *
 * @param char* l
 *   Current line.
 *
 * @param char* s
 *   The separator to count.
 *
 * @return
 *   Number of separators in current line.
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
 *  Text parser
 *
 *  Reads config file -- namespaces, variable names and values -- into options
 *  structure.
 *
 *  @param FILE* read
 *    Handler of the config file to read.
 *    
 *  @param char* SEP 
 *    The separator name/value.
 *
 *  @param char* COMM
 *    The comment mark.
 *
 *  @param LRC_configNamespace*
 *    The structure with config file data.
 *
 *  @param LRC_configTypes*
 *    The structure with datatypes allowed in the config file.
 *
 *  @param int
 *    The number of datatypes allowed in the config file.
 *
 *  @return
 *    Number of config variables, or -1 on failure.
 *
 *  @todo
 *    Check if namespace is allowed.
 */
int LRC_textParser(FILE* read, char* SEP, char* COMM, LRC_configNamespace* configSpace, LRC_configTypes* ct, int numCT){
  
  int i = 0; int j = 0; int sepc = 0; int n = 0;
  char* line; char l[MAX_LINE_LENGTH]; char* b; char* c;
  int ret = 0; int err = 0;

  while(n < MAX_CONFIG_SIZE){
    
    // Count lines
    j++; 
  
    if((feof(read) != 0)) break;
    line = fgets(l, MAX_LINE_LENGTH, read);
    
    // Skip blank lines and any NULL
    if (line == NULL) break;
    if (line[0] == '\n') continue;
    
    // Now we have to trim leading and trailing spaces etc.
    line = LRC_trim(line);

    // Check for full line comments and skip them
    if (strspn(line, COMM) > 0) continue;
    
    // Check for the separator at the begining
    if (strspn(line, SEP) > 0){
      LRC_configError(j,E_MISSING_VAR); 
      goto failure;
    }

    // First split var/value and inline comments
    // Trim leading and trailing spaces
    b = LRC_trim(strtok(line,COMM));

    // Check for namespaces
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
  
    // Check if in the var/value string the separator exist
    if(strstr(b,SEP) == NULL){
      LRC_configError(j,E_MISSING_SEP); 
      goto failure;
    }
    
    // Check some special case:
    // we have separator, but no value
    if((strlen(b) - 1) == strcspn(b,SEP)){
      LRC_configError(j,E_MISSING_VAL); 
      goto failure;
    }

    // We allow to have only one separator in line
    sepc = LRC_charCount(b, SEP);
    if(sepc > 1){
      LRC_configError(j,E_TOOMANY_SEP); 
      goto failure;
    }
    
    // Ok, now we are prepared
    c = LRC_trim(strtok(b,SEP));

    // Check if variable is allowed
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
 * Prints all options.
 *
 * @param int
 *   Number of options.
 *   @see LRC_textParser()
 *   @see LRC_hdfParser()
 *
 * @param LRC_configNamespace*
 *   The structure with config data.
 *   @see LRC_textParser()
 *   @see LRC_hdfParser()
 *
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
 * Wrapper function.
 *
 * @return
 *    The number of options, or -1 on failure
 */
int LRC_parseConfigFile(char* inif, char* sep, char* comm, LRC_configNamespace* configSpace, LRC_configTypes* ct, int numCT){
  
  FILE* read;
  int opts;

	read = fopen(inif,"r");
	if(read != NULL){
		opts = LRC_textParser(read, sep, comm, configSpace, ct, numCT); //read and parse config file
	}else{
		perror("Error opening config file:");
		return -1;
	}
	fclose(read);
  
  return opts;
}

/**
 * Checks if variable is allowed.
 *
 * @param char*
 *   The name of the variable to check.
 *
 * @param LRC_configTypes*
 *   The structure with allowed config datatypes.
 *
 * @param int
 *   The number of allowed config datatypes.
 *
 * @return
 *   Returns 0 on success, -1 on failure.  
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
 * Match input type.
 *
 * @param char*
 *   Variable name.
 *
 * @param char*
 *   The value of the variable.
 *
 * @param LRC_configTypes*
 *   The structure with allowed config datatypes.
 *
 * @param int
 *   The number of config datatypes allowed.
 *   
 * @return
 *   Returns type (integer value) or -1 on failure.
 *   @see LRC_configTypes
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
 * Check type of the value.
 *
 * @param char*
 *   The value to check.
 *
 * @param int
 *   The datatype for given value
 *   @see LRC_confgiTypes
 *
 * @return
 *   Return 0 if the type is correct, -1 otherwise.
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

/** 
 * Check if c is one of the allowed chars.
 *
 * @param int
 *   The char to check.
 *
 * @return
 *   Return 0 on succes, -1 otherwise.
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
 * HDF5 parser 
 *
 * Parse config data stored in HDF5 files.
 *
 * @param hid_t
 *   The handler of the file.
 *
 * @param LRC_configNamespace*
 *   The structure to handle config data.
 *
 * @param LRC_configTypes*
 *   The structure with allowed datatypes.
 *
 * @param int
 *   The number of allowed datatypes.
 *
 * @return
 *   Number of read namespaces.
 *
 * @todo
 *   - Type checking.
 *   - Open rather recreate compound datatype.
 *
 */
int LRC_hdfParser(hid_t file, LRC_configNamespace* cs, LRC_configTypes* ct, int numCT){
  
  hid_t group, dataset, dataspace, memspace;
  hid_t cc_tid;
  herr_t status, info, obj;
  hsize_t dims[2], dimsm[1], offset[2], count[2], stride[2];
  H5G_info_t group_info;
  H5O_info_t object_info;
  int opts = 0, i = 0, k = 0;
  char link_name[MAX_NAME_LENGTH];
  ssize_t link;
  hsize_t edims[1], emaxdims[1];

  LRC_configOptions rdata[1];

  // FIXME: [OPEN, NOT CREATE] Create compound datatype
  cc_tid = H5Tcreate(H5T_COMPOUND, sizeof(LRC_configOptions));
  hid_t name_dt = H5Tcopy(H5T_C_S1);
  H5Tset_size(name_dt, MAX_NAME_LENGTH);

  hid_t value_dt = H5Tcopy(H5T_C_S1);
  H5Tset_size(value_dt, MAX_VALUE_LENGTH);

  H5Tinsert(cc_tid, "name", HOFFSET(LRC_configOptions, name), name_dt);
  H5Tinsert(cc_tid, "value", HOFFSET(LRC_configOptions, value), value_dt);
  H5Tinsert(cc_tid, "type", HOFFSET(LRC_configOptions, type), H5T_NATIVE_INT);

  // Open config group
  group = H5Gopen(file, CONFIG_GROUP, H5P_DEFAULT);

  // Get group info
  info = H5Gget_info(group, &group_info);

  // Get number of opts (dataspaces)
  opts = group_info.nlinks;

  // Iterate each dataspace and assign config values
  for(i = 0; i < opts; i++){

    // Get name od dataspace -> the namespace
    link = H5Lget_name_by_idx(group, ".", H5_INDEX_NAME, H5_ITER_INC, i, link_name, MAX_SPACE_LENGTH, H5P_DEFAULT);
    strcpy(cs[i].space,link_name);

    // Get size of the table with config data
    dataset = H5Dopen(group, cs[i].space, H5P_DEFAULT);
    dataspace = H5Dget_space(dataset);
    H5Sget_simple_extent_dims(dataspace, edims, emaxdims);

    cs[i].num = (int)edims[0];

    // Now we know how many rows are in data tables. 
    // Thus, we can read them one by one and assign to cs struct.
    for(k = 0; k < cs[i].num; k++){
      
      offset[0] = k;
      offset[1] = 1;
      
      dimsm[0] = 1; 
      dimsm[1] = 1;

      count[0] = 1;
      count[1] = 1;

      stride[0] = 1;
      stride[1] = 1;

      memspace = H5Screate_simple(1, dimsm, NULL);
      status = H5Sselect_hyperslab(dataspace, H5S_SELECT_SET, offset, stride, count, NULL);
      status = H5Dread(dataset, cc_tid, memspace, dataspace, H5P_DEFAULT, rdata);

      // Assign values
      strcpy(cs[i].options[k].name, rdata->name);
      strcpy(cs[i].options[k].value, rdata->value);
      cs[i].options[k].type = rdata->type;
   
      H5Sclose(memspace);
    }

    H5Dclose(dataset);
    H5Sclose(dataspace);

  }

  H5Tclose(cc_tid);
  H5Gclose(group);
  
  return opts;
}

/**
 * Write config values to hdf file.
 * 
 * @param hid_t
 *   The handler of the file.
 *
 * @param LRC_configNamespace*
 *   The structure with config values.
 *
 * @param int
 *   Number of namespaces in the config structure.
 *
 */
void LRC_writeHdfConfig(hid_t file, LRC_configNamespace* cs, int allopts){

  hid_t group, dataset, dataspace, memspace;
  hid_t cc_tid;
  hsize_t dims[2], dimsm[1], offset[2], count[2], stride[2];
  herr_t status;
  int i = 0, k = 0;

  LRC_configOptions ccd;

  group = H5Gcreate(file, CONFIG_GROUP, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
 
  // Create compound datatype
  cc_tid = H5Tcreate(H5T_COMPOUND, sizeof(LRC_configOptions));
  hid_t name_dt = H5Tcopy(H5T_C_S1);
  H5Tset_size(name_dt, MAX_NAME_LENGTH);

  hid_t value_dt = H5Tcopy(H5T_C_S1);
  H5Tset_size(value_dt, MAX_VALUE_LENGTH);

  H5Tinsert(cc_tid, "name", HOFFSET(LRC_configOptions, name), name_dt);
  H5Tinsert(cc_tid, "value", HOFFSET(LRC_configOptions, value), value_dt);
  H5Tinsert(cc_tid, "type", HOFFSET(LRC_configOptions, type), H5T_NATIVE_INT);

  for(i = 0; i < allopts; i++){
    
    dims[0] = cs[i].num;
    dims[1] = 1;
    dataspace = H5Screate_simple(1, dims, NULL);
    
    dataset = H5Dcreate(group, cs[i].space,cc_tid, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    // Write config data one by one in given namespace
    for(k = 0; k < dims[0]; k++){
      
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

      strcpy(ccd.name,cs[i].options[k].name);
      strcpy(ccd.value,cs[i].options[k].value);
      ccd.type = cs[i].options[k].type;

      status = H5Dwrite(dataset, cc_tid, memspace, dataspace, H5P_DEFAULT, &ccd);

      status = H5Sclose(memspace);
    }

    status = H5Dclose(dataset);
    status = H5Sclose(dataspace);
  }


  status = H5Gclose(group);
  status = H5Tclose(cc_tid);

  return;
}

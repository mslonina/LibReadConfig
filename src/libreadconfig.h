#ifndef LIBREADCONFIG_H
#define LIBREADCONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include "hdf5.h"

#define MAX_LINE_LENGTH 1024
#define MAX_CONFIG_SIZE 1024
#define MAX_SPACE_LENGTH 256
#define MAX_NAME_LENGTH 256
#define MAX_VALUE_LENGTH 256
#define MAX_OPTIONS_NUM 50
#define CONFIG_GROUP "config"

#define E_CONFIG_SYNTAX "Config file syntax error."
#define E_MISSING_VAR "Missing variable name."
#define E_MISSING_VAL "Missing value."
#define E_MISSING_SEP "Missing separator."
#define E_MISSING_BRACKET "Missing bracket in namespace."
#define E_TOOMANY_SEP "To many separators."
#define E_WRONG_INPUT "Wrong input value type."
#define E_UNKNOWN_VAR "Unknown variable."

enum{ LRC_INT, LRC_FLOAT, LRC_DOUBLE, LRC_CHAR }; 

/**
 * define options structs
 */
typedef struct {
  char name[MAX_NAME_LENGTH];
  char value[MAX_VALUE_LENGTH];
  int type;
} LRC_configOptions;

typedef struct {
  char space[MAX_SPACE_LENGTH]; //name of the namespace
  LRC_configOptions options[MAX_OPTIONS_NUM]; //options for given namespace
  int num; //number of options read
} LRC_configNamespace;

/* Assgin types */
typedef struct {
  char space[MAX_SPACE_LENGTH];
  char name[MAX_NAME_LENGTH];
  int type;
} LRC_configTypes;

/* Parsers */
int LRC_textParser(FILE*, char*, char*, LRC_configNamespace*, LRC_configTypes*, int);
int LRC_hdfParser(hid_t, LRC_configNamespace*, LRC_configTypes*, int);
int LRC_opFunc(hid_t, const char*, const H5O_info_t*, void*);

/* Parser wrapper */
int LRC_parseConfigFile(char*, char*, char*, LRC_configNamespace*, LRC_configTypes* ct, int);

void LRC_printAll(int, LRC_configNamespace*);
void LRC_writeHdfConfig(hid_t, LRC_configNamespace*, int);
void LRC_writeTextConfig(FILE*, char*, char*, LRC_configNamespace*, int);
void LRC_writeConfig(char*, char*, char*, LRC_configNamespace*, int);

#endif

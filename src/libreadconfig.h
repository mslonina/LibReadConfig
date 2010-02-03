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

/**
 * Predefined constants.
 */
#define MAX_LINE_LENGTH 1024
#define MAX_CONFIG_SIZE 1024
#define MAX_SPACE_LENGTH 256
#define MAX_NAME_LENGTH 256
#define MAX_VALUE_LENGTH 256
#define MAX_OPTIONS_NUM 50
#define CONFIG_GROUP "config"

/**
 * Error messages.
 */
#define E_CONFIG_SYNTAX "Config file syntax error."
#define E_MISSING_VAR "Missing variable name."
#define E_MISSING_VAL "Missing value."
#define E_MISSING_SEP "Missing separator."
#define E_MISSING_BRACKET "Missing bracket in namespace."
#define E_TOOMANY_SEP "To many separators."
#define E_WRONG_INPUT "Wrong input value type."
#define E_UNKNOWN_VAR "Unknown variable."

/**
 * Datatypes.
 */
enum{ LRC_INT, LRC_FLOAT, LRC_DOUBLE, LRC_CHAR }; 

/**
 * Options struct.
 *
 * @param char
 *   The name of the variable.
 *
 * @param char
 *   The value of the variable.
 *
 * @param int
 *   The type of the variable.
 */
typedef struct {
  char name[MAX_NAME_LENGTH];
  char value[MAX_VALUE_LENGTH];
  int type;
} LRC_configOptions;

/**
 * Namespace struct.
 *
 * @param char 
 *   The name of the namespace.
 *
 * @param LRC_configOptions
 *   The array of structs of config options.
 *
 * @param int
 *   The number of options read for given config options struct.
 */
typedef struct {
  char space[MAX_SPACE_LENGTH];
  LRC_configOptions options[MAX_OPTIONS_NUM];
  int num;
} LRC_configNamespace;

/**
 * Assign types.
 * 
 * @param char
 *   The namespace name.
 *
 * @param char
 *   The name of the option.
 *
 * @param int
 *   The type of the value.
 */
typedef struct {
  char space[MAX_SPACE_LENGTH];
  char name[MAX_NAME_LENGTH];
  int type;
} LRC_configTypes;

/**
 * Public API
 */
int LRC_textParser(FILE*, char*, char*, LRC_configNamespace*, LRC_configTypes*, int);
int LRC_hdfParser(hid_t, LRC_configNamespace*, LRC_configTypes*, int);

int LRC_parseConfigFile(char*, char*, char*, LRC_configNamespace*, LRC_configTypes* ct, int);

void LRC_printAll(int, LRC_configNamespace*);
void LRC_writeHdfConfig(hid_t, LRC_configNamespace*, int);
void LRC_writeTextConfig(FILE*, char*, char*, LRC_configNamespace*, int);
void LRC_writeConfig(char*, char*, char*, LRC_configNamespace*, int);

#endif

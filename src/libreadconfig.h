#ifndef LIBREADCONFIG_H
#define LIBREADCONFIG_H

#define MAX_LINE_LENGTH 1024
#define MAX_CONFIG_SIZE 1024
#define MAX_NAME_LENGTH 256
#define MAX_VALUE_LENGTH 256
#define MAX_OPTIONS_NUM 50

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
  char space[MAX_NAME_LENGTH]; //name of the namespace
  LRC_configOptions options[MAX_OPTIONS_NUM]; //options for given namespace
  int num; //number of options read
} LRC_configNamespace;

/* Assgin types */
typedef struct {
  char name[MAX_NAME_LENGTH];
  int type;
} LRC_configTypes;

void LRC_configError(int, char*);
char* LRC_trim(char*);
char* LRC_nameTrim(char*);
int LRC_charCount(char*, char*);
int LRC_parseFile(FILE*, char*, char*, LRC_configNamespace*, LRC_configTypes*, int);
void LRC_printAll(int, LRC_configNamespace*);
int LRC_parseConfigFile(char*, char*, char*, LRC_configNamespace*, LRC_configTypes* ct, int);
int LRC_matchType(char*, char*, LRC_configTypes*, int numCT);
int LRC_checkType(char*, int);
int LRC_isAllowed(int);
int LRC_checkName(char*, LRC_configTypes*, int numCT);

#endif

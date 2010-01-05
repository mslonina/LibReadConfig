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

enum{ RC_INT, RC_FLOAT, RC_DOUBLE, RC_CHAR }; 

/**
 * define options structs
 */
typedef struct {
  char name[MAX_NAME_LENGTH];
  char value[MAX_VALUE_LENGTH];
  int type;
} configOptions;

typedef struct {
  char space[MAX_NAME_LENGTH]; //name of the namespace
  configOptions options[MAX_OPTIONS_NUM]; //options for given namespace
  int num; //number of options read
} configNamespace;

/* Assgin types */
typedef struct {
  char name[MAX_NAME_LENGTH];
  int type;
} configTypes;

void configError(int, char*);
char* trim(char*);
char* nametrim(char*, int);
int charcount(char*, char*);
int libreadconfig_parsefile(FILE*, char*, char*, configNamespace*, configTypes*, int);
void libreadconfig_printAll(int, configNamespace*);
int parseConfigFile(char*, char*, char*, configNamespace*, configTypes* ct, int);
int matchType(char*, char*, configTypes*, int numCT);
int checkType(char*, int);
int isallowed(int);

#endif

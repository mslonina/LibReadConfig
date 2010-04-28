#ifndef LIBREADCONFIG_INTERNALS_H
#define LIBREADCONFIG_INTERNALS_H

void LRC_message(int line, int type, char* message);
char* LRC_trim(char*);
char* LRC_nameTrim(char*);
int LRC_charCount(char*, char*);
int LRC_matchType(char*, char*, LRC_configDefaults*, int);
int LRC_checkType(char*, int);
int LRC_isAllowed(int);
int LRC_checkName(char*, LRC_configDefaults*, int);
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



#endif


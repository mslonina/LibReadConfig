/*
 * LIBREADCONFIG
 *
 * Copyright (c) 2010-2011, Mariusz Slonina (Nicolaus Copernicus University)
 * All rights reserved.
 *
 * LIBREADCONFIG was created to help in handling config files by providing common
 * tools and including HDF5 support. The code was released in in belief it will be 
 * useful. If you are going to use this code, or its parts, please consider referring 
 * to the authors either by the website or the user guide reference.
 *
 * See http://git.astri.umk.pl/projects/lrc
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

#ifndef LIBREADCONFIG_H
#define LIBREADCONFIG_H

#if HAVE_CONFIG_H
 #include <config.h>
#endif
/**
 * @file
 * @brief LibreadConfig public library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>

/**
 * @def LRC_MAX_LINE_LENGTH
 * @brief Maximum line length in the config file.
 *
 * @def LRC_NULL
 * @brief Null character for trimming.
 */
#define LRC_MAX_LINE_LENGTH 1024
#define LRC_OPTIONS_END {NULL, NULL, NULL, 0}
#define LRC_NULL '\0'

/**
 * @def LRC_E_CONFIG_SYNTAX
 * @brief Main message for config syntax error. 
 * 
 * @def LRC_E_MISSING_VAR
 * @brief Message for missing variable error.
 * 
 * @def LRC_E_MISSING_VAL
 * @brief Message for missing value error.
 * 
 * @def LRC_E_MISSING_SEP
 * @brief Message for missing separator error.
 * 
 * @def LRC_E_MISSING_BRACKET
 * @brief Message for namespace error.
 * 
 * @def LRC_E_TOOMANY_SEP
 * @brief Message for toomany separators error.
 * 
 * @def LRC_E_WRONG_INPUT
 * @brief Message for wrong user input.
 *
 * @def LRC_E_UNKNOWN_VAR
 * @brief Message for unknown variable error.
 */

enum LRC_messages_type{
  LRC_ERR_CONFIG_SYNTAX,
  LRC_ERR_WRONG_INPUT,
  LRC_ERR_UNKNOWN_VAR,
  LRC_ERR_FILE_OPEN,
  LRC_ERR_FILE_CLOSE,
  LRC_ERR_HDF
} LRC_messages;

#define LRC_MSG_CONFIG_SYNTAX "Config file syntax error"
#define LRC_MSG_MISSING_VAR "Missing variable name"
#define LRC_MSG_MISSING_VAL "Missing value"
#define LRC_MSG_MISSING_SEP "Missing separator"
#define LRC_MSG_MISSING_BRACKET "Missing bracket in namespace"
#define LRC_MSG_TOOMANY_SEP "Too many separators"
#define LRC_MSG_WRONG_INPUT "Wrong input value type"
#define LRC_MSG_UNKNOWN_VAR "Unknown variable"
#define LRC_MSG_FILE_OPEN "File open error"
#define LRC_MSG_HDF "HDF5 error"
#define LRC_MSG_NONAMESPACE "No namespace has been specified"
#define LRC_MSG_UNKNOWN_NAMESPACE "Unknown namespace"

/**
 * @enum datatypes
 * @brief List of datatypes. More may be added in the future.
 */
enum datatypes{ 
  LRC_INT, 
  LRC_LONGINT,
  LRC_FLOAT, 
  LRC_DOUBLE, 
  LRC_LONGDOUBLE, 
  LRC_STRING 
} types; 

/**
 * @struct LRC_configOptions
 * @brief Options struct.
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
typedef struct LRC_configOptions{
  char* name;
  char* value;
  int type;
  struct LRC_configOptions* next;
} LRC_configOptions;

/**
 * @struct LRC_configNamespace
 * @brief Namespace struct.
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
typedef struct LRC_configNamespace{
  char* space;
  LRC_configOptions* options;
  struct LRC_configNamespace* next;
} LRC_configNamespace;

/**
 * @struct LRC_configDefaults
 * @brief Allowed types.
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
  char* space;
  char* name;
  char* value;
  int type;
} LRC_configDefaults;

/**
 * Public API
 */

/* Required */
int LRC_assignDefaults(LRC_configDefaults*);
void LRC_cleanup(void);

/* Output */
void LRC_printAll(void);

/* Parsers and writers */
int LRC_ASCIIParser(FILE* file, char* sep, char* comm);
int LRC_ASCIIWriter(FILE* file, char* sep, char* comm);

/* Search and modify */
LRC_configNamespace* LRC_findNamespace(char* space);
LRC_configOptions* LRC_findOption(char* var);
LRC_configOptions* LRC_modifyOption(char* space, char* var, char* value, int type);
int LRC_countOptions(char* space);
char* LRC_getOptionValue(char* space, char* var);

/* Converters */
int LRC_option2int(char* space, char* var);
float LRC_option2float(char* space, char* var);
double LRC_option2double(char* space, char* var);
long double LRC_option2Ldouble(char* space, char* var);
int LRC_itoa(char* deststr, int value, int type);

#endif

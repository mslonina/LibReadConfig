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

#if HAVE_HDF5_SUPPORT
  #include "hdf5.h"
#endif

/**
 * @def LRC_MAX_LINE_LENGTH
 * @brief Maximum line length in the config file.
 *
 * @def LRC_MAX_CONFIG_SIZE
 * @brief Maximum number of lines in the config file.
 *
 * @def LRC_MAX_SPACE_LENGTH
 * @brief Maximum length of the namespace.
 * 
 * @def LRC_MAX_NAME_LENGTH
 * @brief Maximum length of the variable name.
 * 
 * @def LRC_MAX_VALUE_LENGTH
 * @brief Maximum length of the value.
 * 
 * @def LRC_MAX_OPTIONS_NUM
 * @brief Maximum variables in given namespace.
 * 
 * @def LRC_CONFIG_GROUP
 * @brief Name of the config file group in HDF
 * 
 * @def LRC_NULL
 * @brief Null character for trimming.
 */
#define LRC_MAX_LINE_LENGTH 1024
#define LRC_MAX_CONFIG_SIZE 1024
#define LRC_MAX_SPACE_LENGTH 256
#define LRC_MAX_NAME_LENGTH 256
#define LRC_MAX_VALUE_LENGTH 256
#define LRC_MAX_OPTIONS_NUM 50
#define LRC_NULL '\0'

#if HAVE_HDF5_SUPPORT
  #define LRC_CONFIG_GROUP "config"
#endif

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
#define LRC_E_CONFIG_SYNTAX "Config file syntax error."
#define LRC_E_MISSING_VAR "Missing variable name."
#define LRC_E_MISSING_VAL "Missing value."
#define LRC_E_MISSING_SEP "Missing separator."
#define LRC_E_MISSING_BRACKET "Missing bracket in namespace."
#define LRC_E_TOOMANY_SEP "To many separators."
#define LRC_E_WRONG_INPUT "Wrong input value type."
#define LRC_E_UNKNOWN_VAR "Unknown variable."

/**
 * @enum datatypes
 * @brief List of datatypes. More may be added in the future.
 */
enum datatypes{ LRC_INT, LRC_FLOAT, LRC_DOUBLE, LRC_CHAR } types; 

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
typedef struct {
  char name[LRC_MAX_NAME_LENGTH];
  char value[LRC_MAX_VALUE_LENGTH];
  int type;
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
typedef struct {
  char space[LRC_MAX_SPACE_LENGTH];
  LRC_configOptions options[LRC_MAX_OPTIONS_NUM];
  int num;
} LRC_configNamespace;

/**
 * @struct LRC_configTypes
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
  char space[LRC_MAX_SPACE_LENGTH];
  char name[LRC_MAX_NAME_LENGTH];
  int type;
} LRC_configTypes;

/**
 * Public API
 */
int LRC_textParser(FILE*, char*, char*, LRC_configNamespace*, LRC_configTypes*, int);

#if HAVE_HDF5_SUPPORT
int LRC_hdfParser(hid_t, LRC_configNamespace*, LRC_configTypes*, int);
void LRC_writeHdfConfig(hid_t, LRC_configNamespace*, int);
#endif

int LRC_parseConfigFile(char*, char*, char*, LRC_configNamespace*, LRC_configTypes* ct, int);
void LRC_printAll(int, LRC_configNamespace*);
void LRC_writeTextConfig(FILE*, char*, char*, LRC_configNamespace*, int);
void LRC_writeConfig(char*, char*, char*, LRC_configNamespace*, int);

#endif

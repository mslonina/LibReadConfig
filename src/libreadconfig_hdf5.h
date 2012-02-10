/*
 * LIBREADCONFIG
 *
 * Copyright (c) 2010-2012, Mariusz Slonina (Nicolaus Copernicus University)
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

#ifndef LIBREADCONFIG_HDF5_H
#define LIBREADCONFIG_HDF5_H

#include "libreadconfig.h"
#include <hdf5.h>

#define LRC_CONFIG_GROUP "config"
#define LRC_HDF5_DATATYPE "LRC_Config"

int LRC_HDF5Parser(hid_t file_id, char* group_name, LRC_configNamespace* head);
int LRC_HDF5Writer(hid_t file_id, char* group_name, LRC_configNamespace* head);

#endif

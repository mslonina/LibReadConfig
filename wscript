#!/usr/bin/env python
#encoding: utf-8

import sys
import os
import Utils
import Build
import Scripting
import Options

import string

VERSION='0.12.0'
APPNAME='libreadconfig'
URL='http://mechanics.astri.umk.pl/projects/libreadconfig'
BUGS='mariusz.slonina@gmail.com'

LD_LIBRARY_PATH = string.split(os.environ.get("LD_LIBRARY_PATH"),':')
INCLUDE_PATH = string.split(os.environ.get("C_INCLUDE_PATH"),':')

srcdir = '.'
blddir = 'build'

stdlibs = ['stdio.h', 'dlfcn.h', 'locale.h', 'memory.h', 'stdlib.h', 'stdint.h', 'inttypes.h', 
           'strings.h', 'string.h', 'sys/stat.h', 'sys/types.h', 'unistd.h']

hdf5_test_code = '''
#include <stdio.h>
#include "hdf5.h"

int main()
{
  hid_t file_id, dataset_id, dataspace_id;
  hsize_t dims[2];
  herr_t status;

  H5open();

  file_id = H5Fcreate("waf.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  dims[0] = 2;
  dims[1] = 2;

  dataspace_id = H5Screate_simple(2, dims, NULL);
  dataset_id = H5Dcreate(file_id, "/dset", H5T_NATIVE_INT, dataspace_id, 
      H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  
  status = H5Dclose(dataset_id);
  status = H5Sclose(dataspace_id);
  status = H5Fclose(file_id);

  H5close();
  return 0;
}
'''


# [Helper] Check for standard headers
def _check_std_headers(conf, stdlibs, lpath, ipath):

  for i in stdlibs:
    j = string.split(i,'.h')
    l = string.split(j[0], '/')
    if len(l) > 1:
      j[0] = l[0] + l[1] 

    k = 'HAVE_' + j[0].upper() + '_H'
    conf.check_cc(header_name=i, define_name=k, mandatory=0)

  conf.check_cc(type_name='ssize_t')
  conf.check_cc(type_name='struct')
  conf.check_cc(type_name='int')
  conf.check_cc(type_name='char')
  conf.check_cc(type_name='double')
  conf.check_cc(function_name='printf', header_name='stdio.h')
  conf.check_cc(function_name='memmove', header_name='memory.h')
  conf.check_cc(function_name='strcspn', header_name='string.h')
  conf.check_cc(function_name='strspn', header_name='string.h')
  conf.check_cc(function_name='strstr', header_name='string.h')
  conf.check_cc(function_name='strtol', header_name='stdlib.h')
  conf.check_cc(function_name='strtok', header_name='string.h')

#
# SET OPTIONS
#
def set_options(opt):
  opt.tool_options('compiler_cc')

  opt.add_option('--enable-hdf',
                  action = 'store_true', 
                  default = False,
                  help = 'Enable HDF5 support',
                  dest = 'enablehdf'
                  )
  opt.add_option('--enable-doc',
                  action = 'store_true', 
                  default = False,
                  help = 'Build documentation',
                  dest = 'enabledoc'
                  )

#
# CONFIGURE
#
def configure(conf):
  global stdlibs
  global LD_LIBRARY_PATH
  global INCLUDE_PATH
  global hdf5_test_code

  conf.check_tool('compiler_cc')

  _check_std_headers(conf, stdlibs, LD_LIBRARY_PATH, INCLUDE_PATH)

  # Check for HDF5
  if Options.options.enablehdf:
    try:
      conf.check_cc(lib='hdf5', 
                    uselib='HDF5', 
                    mandatory=True, 
                    msg="Looking for HDF5 library");
    except:
      print "HDF5 library was not found on your system :("

    try:
      conf.check_cc(header_name='hdf5.h', 
                    mandatory=True)
    except:
      print "HDF5 header was not found on your system :("

    try:
      conf.check_cc(fragment=hdf5_test_code, 
                    execute=True, 
                    uselib="HDF5", 
                    define_ret=True, 
                    mandatory=True, 
                    msg="Checking if HDF5 is usable")
    except:
      print "Cannot run HDF5 testprogram :("
    
  # Define standard declarations
  conf.define('PACKAGE_NAME', APPNAME)
  conf.define('PACKAGE_VERSION', VERSION)
  conf.define('PACKAGE_BUGS', BUGS)
  conf.define('PACKAGE_URL', URL)

  conf.env.append_value('CCFLAGS', '-Wall')
  
  # Write config.h
  conf.write_config_header('config.h')

#
# BUILD
#
def build(bld):
  obj = bld.new_task_gen('cc', 'shlib')
  obj.source = ['src/libreadconfig.c']
  obj.target = 'readconfig'
  bld.install_files('${PREFIX}/include', 'src/libreadconfig.h')

#!/usr/bin/env python
# wscript for LibReadConfig
# Mariusz Slonina, 2010
# encoding: utf-8

import sys
import os
import Utils
import Build
import Scripting
import Options

import string

VERSION='0.12.0-alpha2'
APPNAME='libreadconfig'
URL='http://mechanics.astri.umk.pl/projects/libreadconfig'
BUGS='mariusz.slonina@gmail.com'

srcdir = '.'
blddir = 'build'

Scripting.g_gz = 'gz'

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

	remove("waf.h5");

  return 0;
}
'''


# [Helper] Check for standard headers/functions
def _check_std_headers(conf, stdlibs):

  for i in stdlibs:
    j = string.split(i,'.h')
    l = string.split(j[0], '/')
    if len(l) > 1:
      j[0] = l[0] + l[1] 

    k = 'HAVE_' + j[0].upper() + '_H'
    conf.check_cc(header_name=i, define_name=k, mandatory=True)

  conf.check_cc(type_name='ssize_t', header_name='sys/types.h')
  conf.check_cc(type_name='int', header_name='sys/types.h')
  conf.check_cc(type_name='char', header_name='sys/types.h')
  conf.check_cc(type_name='double', header_name='sys/types.h')
  conf.check_cc(function_name='printf', header_name='stdio.h', mandatory=True)
  conf.check_cc(function_name='memmove', header_name='memory.h', mandatory=True)
  conf.check_cc(function_name='strcspn', header_name='string.h', mandatory=True)
  conf.check_cc(function_name='strspn', header_name='string.h', mandatory=True)
  conf.check_cc(function_name='strstr', header_name='string.h', mandatory=True)
  conf.check_cc(function_name='strtol', header_name='stdlib.h', mandatory=True)
#  conf.check_cc(function_name='strtold', header_name='stdlib.h', mandatory=True)
#  conf.check_cc(function_name='strtof', header_name='stdlib.h', mandatory=True)
  conf.check_cc(function_name='strtod', header_name='stdlib.h', mandatory=True)
  conf.check_cc(function_name='strtok', header_name='string.h', mandatory=True)
  conf.check_cc(function_name='strncpy', header_name='string.h', mandatory=True)

# [Helper] Yes/No at the summary
def _check_defined(conf, define):
	if conf.is_defined(define):
		return "Yes"
	else:
		return "No"

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
  opt.add_option('--build-doc',
                  action = 'store_true', 
                  default = False,
                  help = 'Build documentation',
                  dest = 'builddoc'
                  )
  opt.add_option('--examples',
                  action = 'store_true', 
                  default = False,
                  help = 'Install examples',
                  dest = 'examples'
                  )

#
# CONFIGURE
#
def configure(conf):
  global stdlibs
  global hdf5_test_code
  global install_hdf_h
  
  conf.check_tool('compiler_cc')
  
  _check_std_headers(conf, stdlibs)
  
  conf.env.HDF5 = 0
  # Check for HDF5
  if Options.options.enablehdf:
    try:
      conf.check_cc(lib='hdf5', mandatory=True, uselib_store='HDF5', msg="Looking for HDF5 library")
      conf.env.HDF5 = 1
    except:
      print "HDF5 library was not found on your system :("
      conf.env.HDF5 = 0

    try:
      conf.check_cc(header_name='hdf5.h', mandatory=True)
      conf.env.HDF5 = 1
    except:
      print "HDF5 header was not found on your system :("
      conf.env.HDF5 = 0

    try:
      conf.check_cc(fragment=hdf5_test_code, 
                  execute=True, 
                  uselib='HDF5', 
                  define_ret=True, 
                  mandatory=True, 
                  msg="Checking if HDF5 is usable")
      conf.env.HDF5 = 1
    except:
      print "Cannot run HDF5 testprogram :("
      conf.env.HDF5 = 0

  # Check for doxygen
  BUILDDOC="No"
  if Options.options.builddoc:
    try:
      conf.find_program('doxygen', var='DOXYGEN', mandatory=True)
      BUILDDOC="Yes"
    except:
      print "You need Doxygen installed to build documentation"
	
  # Examples
  EXAMPLES="No"
  conf.env.EXAMPLES = 0
  if Options.options.examples:
    EXAMPLES="Yes"
    conf.env.EXAMPLES = 1
    pass
	
	
  # Define standard declarations
  conf.define('PACKAGE_NAME', APPNAME)
  conf.define('PACKAGE_VERSION', VERSION)
  conf.define('PACKAGE_BUGS', BUGS)
  conf.define('PACKAGE_URL', URL)
  conf.env['CCFLAGS'] += ['-Wall']
  conf.env['CCFLAGS'] += ['-std=c99']
  conf.env['CPPFLAGS'] += ['-DHAVE_CONFIG_H']
  conf.env['CPPFLAGS'] += ['-I../build/default']
	
  # Write config.h
  conf.write_config_header('config.h')

  print
  print "Configuration summary:"
  print
  print "  Install prefix: " + conf.env.PREFIX
  print "  HDF5 support:  " + _check_defined(conf, "HAVE_HDF5_H")
  print "  Documentation: " + BUILDDOC
  print "  Examples:\t " + EXAMPLES
  print

#
# BUILD
#
def build(bld):
  global uselib
  obj = bld.new_task_gen('cc', 'shlib')
  obj.source = ['src/libreadconfig.c']
  obj.includes = 'src'
  obj.target = 'readconfig'
  obj.uselib = 'HDF5'
  
  obj = bld.new_task_gen('cc', 'staticlib')
  obj.source = ['src/libreadconfig.c']
  obj.includes = 'src'
  obj.target = 'readconfig'
  obj.uselib = 'HDF5'
  
  if bld.env.HDF5 == 1:
    bld.install_files('${PREFIX}/include', 'src/libreadconfig.h src/libreadconfig_hdf5.h')
  else:
    bld.install_files('${PREFIX}/include', 'src/libreadconfig.h')

  if bld.env.EXAMPLES == 1:
    bld.install_files('${PREFIX}/share/doc/libreadconfig/examples',
    'doc/lrc-example.c doc/lrc-config doc/Sample-Makefile')

#  pobj = bld(
#    features = 'subst',
#    source = 'lrc.pc.in',
#    target = 'libreadconfig.pc',
#    dict = {
#      'NAME': APPNAME,
#      'LIB': 'APPNAME' + '.so',
#      'PREFIX': bld.env['PREFIX'],
#      'BINDIR': os.path.join("${prefix}", "bin"),
#      'LIBDIR': os.path.join("${prefix}", "lib"),
#      'INCLUDEDIR': os.path.join("${prefix}", "include"),
#      'VERSION': bld.env["VERSION"],
#    },
#    install_path = '${PKGCONFIGDIR}'
#  )



#include <stdio.h>
#include <stdlib.h>
#include "libreadconfig.h"
#include "hdf5.h"

int main(int argc, char* argv[]){

  hid_t file;
  int opts = 1;
  int numCT = 4;

  LRC_configTypes ct[4] = {
    {"default", "char", LRC_CHAR},
    {"default", "int", LRC_INT},
    {"default", "double", LRC_DOUBLE},
    {"default", "float", LRC_FLOAT},
  };

  LRC_configNamespace cs[] = {
    {"default",{
                 {"char","aaa",LRC_CHAR},
                 {"int","44",LRC_INT},
                 {"double","12.3456",LRC_DOUBLE},
                 {"float","34.5678",LRC_FLOAT},
               },
    4}
  };
  
  file = H5Fcreate("lrc-hdf-test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  
  LRC_writeHdfConfig(file, cs, opts);

  H5Fclose(file);

  remove("lrc-hdf-test.h5");
	
  return 0;
}


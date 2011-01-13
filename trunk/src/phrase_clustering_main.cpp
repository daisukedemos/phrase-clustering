#include <iostream>
#include "phrase_clustering.hpp"
#include "cmdline.h"

using namespace std;

int main(int argc, char* argv[]){
  if (argc != 2){
    cerr << argv[0] << " filelist" << endl;
    return -1;
  }

  phrase_clustering::PhraseClustering pc;
  try {
    pc.InitFromFileList(argv[1]);
  } catch (const string& str){
    cerr << "Error:" << str << endl;
    return -1;
  }

  return 0;
}

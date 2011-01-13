#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "phrase_clustering.hpp"
#include "sais.hxx"

using namespace std;

namespace {
void ReadFileList(const char* fn,
		    vector<string>& file_list){
  ifstream ifs(fn);
  if (!ifs){
    throw string("cannot open ") + fn;
  }
  for (string line; getline(ifs, line);){
    file_list.push_back(line);
  }
}

}

namespace phrase_clustering{

PhraseClustering::PhraseClustering() : n(0){
}

PhraseClustering::~PhraseClustering(){

}

void PhraseClustering::InitFromFileList(const char* fn){
  vector<string> file_list;
  ReadFileList(fn, file_list);
  ReadFiles(file_list);
  Build();
}

void PhraseClustering::Print(uint64_t offset, uint64_t len) const{
  for (uint64_t i = 0; i < len; ++i){
    cout << code2rawstr[T[offset+i]];
  }
  cout << endl;

}

void PhraseClustering::Build(){
  n = T.size();
  cout << n << " " << str2code.size() << endl;
  SA.resize(n);
  saisxx(T.begin(), SA.begin(), (int64_t)n, (int64_t)str2code.size());
  
  B.resize(T.size());
  for (uint64_t i = 0; i < n; ++i){
    if (SA[i] != 0) B[i] = T[SA[i]-1];
    else            B[i] = T[n-1];
  }
  
  vector<uint64_t> P(n);
  for (size_t i = 0; i < n; ++i){
    P[SA[i]] = SA[i-1];
  }
  
  vector<uint64_t> PLCP(P.size());
  uint64_t height = 0;
  for (uint64_t i = 0; i < P.size(); ++i){
    uint64_t j = P[i];
    uint64_t max_ij = max(i, j);
    while (max_ij+height < n &&
	   T[i+height] == T[j+height]){
      ++height;
    }
    PLCP[i] = height;
    if (height > 0) --height;
  }

  uint64_t node_num = 0;
  vector<pair<int, int> > stack;
  stack.push_back(make_pair(-1, -1));
  for (uint64_t i = 0; ; ++i){
    pair<int, int> cur(i, (i==n) ? -1 : PLCP[SA[i]]);
    pair<int, int> cand(stack.back());
    while (cand.second > cur.second){
      if (i - cand.first > 1){
	bool all_eq = true;
	for (uint64_t j = cand.first+1; j < i; ++j){
	  if (B[j-1] != B[j]){
	    all_eq = false;
	    break;
	  } 
	}
	if (!all_eq){
	  //cout << i - cand.first << " " << cand.second << " ";
	  //Print(SA[cand.first], cand.second);
	  ++node_num;
	}
      } 
      cur.first = cand.first;
      stack.pop_back();
      cand = stack.back();
    }
    if (cand.second < cur.second){
      stack.push_back(cur);
    }
    if (i == n) break;
    stack.push_back(make_pair(i, n - SA[i] + 1));
  } 
  cout << node_num << endl;
}

uint64_t PhraseClustering::GetCode(const uint64_t str){
  map<uint64_t, uint64_t>::const_iterator it = str2code.find(str);
  if (it != str2code.end()){
    return it->second;
  }
  uint64_t code = (uint64_t)str2code.size();
  str2code[str] = code;
  code2str.push_back(str);
  string rawstr;
  uint64_t strval = str;
  for (size_t i = 0; i < 4 && strval != 0; ++i){
    rawstr.push_back((char)(strval & 0xFF));
    strval >>= 8;
  }
  reverse(rawstr.begin(), rawstr.end());
  code2rawstr.push_back(rawstr);
  return code;
}

void PhraseClustering::ReadFile(const std::string& file){
  ifstream ifs(file.c_str());
  if (!ifs){
    throw string("cannot open ") + file;
  }

  ifs.seekg(0, fstream::end);
  size_t end_size = ifs.tellg();
  ifs.clear();
  ifs.seekg(0, fstream::beg);
  size_t file_size = end_size - ifs.tellg();
  if (file_size == 0) return;
  vector<char> buf(file_size);
  ifs.read(&buf[0], file_size);
  uint64_t cur = (uint8_t)(buf[0]);
  for (size_t i = 1; i <= file_size; ++i){
    if (i == file_size || ((buf[i] & 0xC0) != 0x80)){
      T.push_back(GetCode(cur));
      cur = 0;
      if (i == file_size) break;
    }
    cur <<= 8;
    cur |= (uint8_t)(buf[i]);
  }
}

void PhraseClustering::ReadFiles(const vector<string>& file_list){
  uint64_t file_delim = GetCode(0);
  for (size_t i = 0; i < file_list.size(); ++i){
    T.push_back(file_delim);
    ReadFile(file_list[i]);
  }
  T.push_back(file_delim);
}




}

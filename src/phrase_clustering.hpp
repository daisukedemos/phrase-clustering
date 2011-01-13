#ifndef PHRASE_CLUSTERING_HPP_
#define PHRASE_CLUSTERING_HPP_

#include <vector>
#include <string>
#include <map>
#include <stdint.h>

namespace phrase_clustering{

class PhraseClustering{
public:
  PhraseClustering();
  ~PhraseClustering();

  void InitFromFileList(const char* fn);
  void ReadFiles(const std::vector<std::string>& file_list);
  void ReadFile(const std::string& file);
  void Build();
  

private:
  uint64_t GetCode(const uint64_t str);
  void Print(uint64_t offset, uint64_t len) const;

  std::vector<uint64_t> T;
  std::vector<uint64_t> B;
  std::vector<int64_t> SA;
  std::vector<int64_t> ISA;

  std::map<uint64_t, uint64_t> str2code;
  std::vector<uint64_t>        code2str;
  std::vector<std::string>     code2rawstr;

  uint64_t n;
};


}

#endif // PHRASE_CLUSTERING_HPP_

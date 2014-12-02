#include "../../src/common.h"
#include "../../src/memmr.h"
#include "../../src/worker.h"

class MyWorker: public Worker {

  bool map(Data line, std::vector<KV>& kvs) {
    for (int i = 0; i < line.size(); ++i) {
      kvs.push_back(std::make_pair<Key, Value>(line[i], "1"));
    }
    return true;
  }

  bool reduce(std::vector<KV> kvs, Data& data) {
    int sz = kvs.size();
    if (sz != 0) {
      std::ostringstream oss;
      oss << sz;
      data.push_back(kvs[0].first);
      data.push_back(oss.str());
    }
    return true;
  }
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    puts("FILE!!!");
    exit(-1);
  }

  MyWorker myW;
  MapReduce mr(32, 32, argv[1], &myW);

  if (mr.runMaster()) {
    puts("DONE");
    std::vector<Data> out = mr.getResults();
    
    for (int i = 0; i < out.size(); ++i) {
      for (int j = 0; j < out[i].size(); ++j) {
        std::cout << out[i][j] << " ";
      }
      std::cout << std::endl;
    }
    
  } else {
    puts("FAIL");
  }
  return 0;
}

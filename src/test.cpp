#include "common.h"
#include "memmr.h"
#include "worker.h"

class MyWorker: public Worker {

  bool map(Data lines, std::vector<KV>& kvs) {
    puts("My Worker!");
    return true;
  }

  bool reduce(std::vector<KV> kvs, Data& data) {
    puts("My Reducer!");
    return true;
  }
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    puts("FILE!!!");
    exit(-1);
  }
  MyWorker myW;
  MapReduce mr(2, 2, argv[1], myW);
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

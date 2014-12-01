#ifndef __MEMMR_CORE__
#define __MEMMR_CORE__

#include "common.h"

class MapReduce {
protected:
  int nMap, nReduce, n;
  std::ifstream file;
  std::vector<Args> args;
  std::vector<Data> out;

  pthread_t workers[MAXTHREAD];

public:
  MRManager() {
    n = nMap = nReduce = 0;
  }

  MRManager(int m, int r, std::string f) {
    nMap = m;
    nReduce = r;
    n = m + r;
    out.clear();
    args.clear();
    file.open(f);
    for (int i = 0; i < n; ++i) {
      args.push_back(Args(i));
    }

  }

  ~MRManager() {
    if (file.is_open()) {
      file.close();
    }
  }

  std::vector<Data> getResults() {
    return out;
  }

  bool runMaster() {
    if (!this->split()) {
      puts("MRManager - calling split() failed.");
      return false;
    }

    puts("MRManager - start mapping.");
    for (int i = 0; i < nMap; ++i) {
      if (pthread_create(&workers[i], NULL, map, (void*)args[i])) {
        puts("MRManager - cannot initialize thread.");
        return false;
      }
    }

    for (int i = 0; i < nMap; ++i) {
      if (pthread_join(workers[i], NULL)) {
        puts("MRManager - cannot join thread.");
        return false;
      }
    }

    puts("MRManager - start reducing.");
    for (int i = nMap; i < n; ++i) {
      if (pthread_create(&workers[i], NULL, reduce, (void*)args[i])) {
        puts("MRManager - cannot initialize thread.");
        return false;
      }
    }

    for (int i = nMap; i < n; ++i) {
      if (pthread_join(workers[i], NULL)) {
        puts("MRManager - cannot join thread.");
        return false;
      }
    }

    for (int i = nMap; i < n; ++i) {
      out.push_back(args[i].data);
    }

    return true;
  }

  // simply assign an equal number of lines of input to all mappers but 
  // the last one.
  virtual bool split() {
    if (!file.is_open()) {
      // TODO - I know, I know, don't tell me how to handle this...
      puts("MRManager - Input file does not exist");
      return false;
    }

    // get all lines and get the number of lines implicitly
    std::vector<std::string> input;
    while (file) {
      std::string buf;
      getline(file, buf);
      input.push_back(buf);
    }

    int sz = input.size(), idx = 0, id = 0;
    for (int i = 0; i < sz; ++i) {
      std::istringstream iss(buf[i]);
      std::string token;
      while (iss >> token) {
        args[id].data[idx].push_back(token);
      }
      if (++idx == nMap) {
        ++id;
        idx = 0;
      }
    }
    
    return true;
  }

  virtual bool map(Arg args) {
    return true;
  }

  virtual bool reduce(Arg args) { 
    return true;
  }
}

#endif

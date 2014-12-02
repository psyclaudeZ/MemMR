#ifndef __MEMMR_CORE__
#define __MEMMR_CORE__

#include "common.h"
#include "worker.h"

class Worker;
class MapReduce {
protected:
  int nMap, nReduce, n;
  std::ifstream file;
  std::vector<Data> out;

  Worker workers[MAXTHREAD];
  pthread_t threads[MAXTHREAD];

public:
  MapReduce() {
    n = nMap = nReduce = 0;
  }

  MapReduce(int m, int r, std::string f, Worker w) {
    nMap = m;
    nReduce = r;
    n = m + r;
    out.clear();
    args.clear();
    file.open(f.c_str());
    for (int i = 0; i < n; ++i) {
      args.push_back(Args(i));
      workers[i] = w;
    }
    for (int i = 0; i < n; ++i) {
      pthread_mutex_init(&locks[i], NULL);
    }
  }

  ~MapReduce() {
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
      wraps[i].id = i;
      wraps[i].nMap = nMap;
      wraps[i].nReduce = nReduce;
      wraps[i].worker = &workers[i];

      if (pthread_create(&threads[i], NULL, &Worker::MapEntryFunc, (void*)&wraps[i])) {
        puts("MRManager - cannot initialize thread.");
        return false;
      }
    }

    for (int i = 0; i < nMap; ++i) {
      if (pthread_join(threads[i], NULL)) {
        puts("MRManager - cannot join thread.");
        return false;
      }
    }

    puts("MRManager - start reducing.");
    for (int i = nMap; i < n; ++i) {
      wraps[i].id = i;
      wraps[i].nMap = nMap;
      wraps[i].nReduce = nReduce;
      wraps[i].worker = &workers[i];

      if (pthread_create(&threads[i], NULL, &Worker::ReduceEntryFunc, (void*)&wraps[i])) {
        puts("MRManager - cannot initialize thread.");
        return false;
      }
    }

    for (int i = nMap; i < n; ++i) {
      if (pthread_join(threads[i], NULL)) {
        puts("MRManager - cannot join thread.");
        return false;
      }
    }

    for (int i = nMap; i < n; ++i) {
      std::vector<Data> d = args[i].data;
      for (int j = d.size() - 1; j >= 0; --j) {
        out.push_back(d[j]);
      }
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
    std::string buf;
    while (getline(file, buf)) {
      input.push_back(buf);
    }

    int sz = input.size(), load = sz % nMap == 0 ? sz / nMap : sz / nMap + 1, idx = 0, id = 0;
    //std::cout << "SIZE: " << load << std::endl;
    for (int i = 0; i < sz; ++i) {
      std::istringstream iss(input[i]);
      std::string token;
      Data dataBuf;
      while (iss >> token) {
        //std::cout << "TOKEN: " << token << 
         // " ID: " << id << std::endl;
        dataBuf.push_back(token);
      }
      args[id].data.push_back(dataBuf);
      if (++idx == load) {
        ++id;
        idx = 0;
      }
    }

    return true;
  }
};
#endif

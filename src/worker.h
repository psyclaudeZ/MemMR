#ifndef __MEMMR_WORKER__
#define __MEMMR_WORKER__

#include "common.h"

class Worker {


protected:
  // TODO - currently using boost library
  inline static int getHash(std::string s) {
    boost::hash<std::string> shash;
    return shash(s);
  }

  virtual bool map(Data line, std::vector<KV>& kvs) {
    //puts("Original mapper");
    for (int i = 0; i < line.size(); ++i) {
      kvs.push_back(std::make_pair<Key, Value>(line[i], "1"));
    }
    return true;
  }

  virtual bool reduce(std::vector<KV> kvs, Data& data) { 
    int sz = kvs.size();
    if (sz != 0) {
      std::ostringstream oss;
      oss << sz;
      data.push_back(kvs[0].first);
      data.push_back(oss.str());
    }
    return true;
  }

public:
  static void* MapEntryFunc(void* t) {
    Worker* worker = (Worker*)(((Wrap*)t)->worker);
    int id = ((Wrap*)t)->id, nReduce = ((Wrap*)t)->nReduce, nMap = ((Wrap*)t)->nMap;
    
    std::vector<KV>& kvs = args[id].kvs[0];

    for (int i = args[id].data.size() - 1; i >= 0; --i) {
      //printf("Loop 1 %d\n", i);
      if (args[id].data[i].size() == 0) continue;
      Data line = args[id].data[i];
      std::vector<KV> buf;
      worker->map(line, buf);
      for (int j = buf.size() - 1; j >= 0; --j) {
        kvs.push_back(buf[j]);
      }
    }

    for (int i = kvs.size() - 1; i >= 0; --i) {
      //printf("Loop 2: %d\n", i);
      Key k = kvs[i].first;
      Value v = kvs[i].second;

      int reducerID = (getHash(k) % nReduce + nReduce) % nReduce + nMap;

//      std::cout << "Hash: " << getHash(k) % nReduce<< std::endl;
      //std::cout << "ReducerID: " << reducerID << std::endl;

      pthread_mutex_lock(&locks[reducerID]);
      if (getKeyIndex[reducerID].find(k) == getKeyIndex[reducerID].end()) {
        getKeyIndex[reducerID][k] = std::vector<KV>();
      }

      //printf("Loop 222222222222222: %d\n", reducerID);

      //std::cout << kvs[i].first << std::endl;
      getKeyIndex[reducerID][k].push_back(kvs[i]);
      pthread_mutex_unlock(&locks[reducerID]);

      //printf("Loop end: %d\n", i);
    }
    return NULL;
  }

  static void* ReduceEntryFunc(void* t) {
    Worker* worker = (Worker*)(((Wrap*)t)->worker);
    int id = ((Wrap*)t)->id;

//    printf("Reducer ID: %d\n", id);

    std::map<std::string, std::vector<KV> > tmp = getKeyIndex[id];

    for (std::map<std::string, std::vector<KV> >::iterator it = tmp.begin(); it != tmp.end(); ++it) {
      Data dataBuf;
      worker->reduce(it->second, dataBuf);
      args[id].data.push_back(dataBuf);
      //std::cout << "REDUCED DATA: " << args[id].data.size() << std::endl;
    }

    /*
    for (int i = args[id].kvs.size() - 1; i >= 0; --i) {
      Data dataBuf;
      if (args[id].kvs[i].size() == 0) continue;
      worker->reduce(args[id].kvs[i], dataBuf);
      args[id].data.push_back(dataBuf);
      std::cout << "REDUCED DATA: " << args[id].data.size() << std::endl;
    }
    */
    return NULL;
  }
};

#endif

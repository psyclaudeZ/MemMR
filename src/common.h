#ifndef __MEMMR_COMMON__
#define __MEMMR_COMMON__

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <pthread.h>
#include <stdint.h>

#include <boost/functional/hash.hpp>

const int MAXTHREAD = 1 << 8;
const int MAXSPACE = 1 << 8;

typedef std::string Key;
typedef std::string Value;
typedef std::vector<std::string> Data;

typedef std::pair<Key, Value> KV;

typedef struct ARGSWRAPPER {
  void* worker;
  int id, nMap, nReduce;
} Wrap;

typedef struct ARGS {
  int id;
  std::vector<std::vector<KV> > kvs;
  std::vector<Data> data;

  ARGS() {
    id = -1;
    kvs = std::vector<std::vector<KV> >(MAXSPACE, std::vector<KV>());
  }

  ARGS(int i) {
    id = i;
    kvs = std::vector<std::vector<KV> >(MAXSPACE, std::vector<KV>());
  }

} Args;

std::vector<Args> args;
pthread_mutex_t locks[MAXTHREAD];
std::map<std::string, std::vector<KV> > getKeyIndex[MAXTHREAD];
Wrap wraps[MAXTHREAD];



#endif

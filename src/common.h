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

const int MAXTHREAD = 1 << 8;

typedef Key std::string;
typedef Value std::string;
typedef Data std::vector<std::string>;

typedef pair<Key, Value> KV;

typedef struct ARGS {
  int id;
  std::vector<KV> kvs;
  std::vector<Data> data;

  ARGS() {
    id = -1;
    kvs = std::vector<KV>();
    data = std::vector<Data>();
  }

  ARGS(int i) {
    id = i;
    kvs = std::vector<KV>();
    data = std::vector<Data>();
  }

} Args;

#endif

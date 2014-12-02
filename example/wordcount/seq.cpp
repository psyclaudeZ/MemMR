#include <iostream>
#include <cstdio>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc != 2) {
    puts("INPUT!!!");
    exit(-1);
  }
  ifstream file(argv[1]);
  map<string, int> hash;
  string buf;

    string ss;
    while (getline(file, ss)) {
      string token;
      istringstream iss(ss);
      while (iss >> token) {
        hash[token]++;
      }
    }

  for (map<string, int>::iterator it = hash.begin(); it != hash.end(); ++it) {
    cout << it->first << " " << it->second << endl;
  }
  return 0;
}

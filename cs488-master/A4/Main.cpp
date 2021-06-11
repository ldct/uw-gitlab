// Winter 2020

#include <iostream>
#include <string>
#include <vector>

#include "scene_lua.hpp"

using namespace std;

vector<string> args;

int main(int argc, char** argv) {
  string filename = "Assets/simple.lua";
  if (argc >= 2) {
    filename = argv[1];
  }
  args = vector<string>();
  for (int i=2; i<argc; i++) {
    string s;
    s = argv[i];
    args.push_back(s);
  }

  if (!run_lua(filename)) {
    cerr << "Could not open " << filename << std::endl;
    return 1;
  }
}

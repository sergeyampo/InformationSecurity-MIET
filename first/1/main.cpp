#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int main() {
  fs::path p;
  try {
	p = "../text.txt";
	ifstream fin(p);
  }
  catch(...){
    cerr << "Cannot read filename, please change the path to the file!" << endl;
  }
  cout << "Size in bytes: " << fs::file_size(p) << endl;
  return 0;
}


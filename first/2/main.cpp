#include <iostream>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <map>
#include <ios>

using namespace std;
namespace fs = std::filesystem;


const string filename = "second.txt";

map<char, uint64_t> get_map_of_bytes_entries(ifstream& fin) {
  map<char, uint64_t> dict;
  while (fin) {
	char temp_byte = fin.get();
	if (!fin) {
	  break;
	}

	const auto found_elem = dict.find(temp_byte);
	if (found_elem != end(dict))
	  found_elem->second += 1;
	else
	  dict[temp_byte] = 1;
  }

  return dict;
}

int main() {
  fs::path p = "../" + filename;
  ifstream fin(p, ios::binary);
  const auto file_size = fs::file_size(p);

  const auto dict = get_map_of_bytes_entries(fin);
  for (auto&[key, value] : dict) {
	cout << "For byte: " << key << " frequency is: " << value /(double) file_size << "\n";
  }

  return 0;
}


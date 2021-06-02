#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <climits>


auto filesize(const char* filename) {
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  return in.tellg();
}

std::map<int, int> byteFrequence(const char* filename) {
  std::map<int, int> myMap;
  std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
  int fileSize = in.tellg();
  in.seekg(0, std::ifstream::beg);
  char buffer;
  while (in) {
	in.read(&buffer, 1);

	auto search = myMap.find(buffer);
	if (search == myMap.end())// there is no key
	{
	  myMap.insert(std::pair<char, int>(buffer, 1));
	} else {
	  search->second++;
	}
  }
  return myMap;
}

bool is_mas_contains_elem(const int* mas, const int size, const int elem) {
  for (int i = 0; i < size; i++) {
	if (mas[i] == elem)
	  return true;
  }
  return false;
}

auto keyDecode(const char* key, const int size) {
  int* intKey = new int[size];
  int max_elem = 0;
  for (int i = 0; i < size; i++) {
	int min_elem = INT_MAX;
	int mim_elem_index = 0;
	for (int j = 0; j < size; j++) {
	  if (key[j] < min_elem && key[j] >= max_elem && !is_mas_contains_elem(intKey, i, j)) {
		min_elem = key[j];
		mim_elem_index = j;
	  }
	}
	intKey[i] = mim_elem_index;
	max_elem = min_elem;
  }
  return intKey;
}

std::stringstream fstream_to_stringstream(std::fstream f_stream) {
  std::stringstream str_stream;
  if (f_stream) {
	str_stream << f_stream.rdbuf();
	f_stream.close();
  }
  return str_stream;
}

std::stringstream encode(std::stringstream& str_stream, const char* key, int key_size) {
  str_stream.seekg(0, std::ios::beg);
  std::string* columns = new std::string[key_size];
  char sym;
  int column_index = 0;
  int str_length = str_stream.str().length() ? 1 : 0;
  while (str_stream.get(sym)) {
	if (column_index >= key_size) {
	  column_index %= key_size;
	  str_length++;
	}
	columns[column_index] += sym;

	column_index++;
  }
  for (int i = 0; i < key_size; i++) {
	if (columns[i].length() != str_length) {
	  columns[i] += '\0';
	}
  }
  int* int_key = keyDecode(key, key_size);
  std::stringstream return_str_stream;
  for (int i = 0; i < key_size; i++) {
	return_str_stream << columns[int_key[i]];
  }
  delete[] int_key;
  delete[] columns;
  return return_str_stream;
}

std::stringstream encode(const char* filename, bool isBinary, const char* key, int key_size) {
  std::stringstream ss;
  std::stringstream result;
  if (isBinary) {
	std::ifstream in(filename, std::ifstream::binary);
	if (in) {
	  ss << in.rdbuf();
	  result = encode(ss, key, key_size);
	  in.close();
	}
  } else {
	std::ifstream in(filename);
	if (in) {
	  ss << in.rdbuf();
	  result = encode(ss, key, key_size);
	  in.close();
	}
  }
  return result;
}

std::stringstream decode(std::stringstream& str_stream, const char* key, int key_size) {
  str_stream.seekg(0, std::ios::beg);
  std::string str_buf = str_stream.str();
  std::string* columns = new std::string[key_size];
  int str_size = str_buf.length();
  int str_index = 0;
  int column_index = 0;
  int char_counter = 0;
  for (int i = 0; i < key_size; i++) {
	for (int j = 0; j < str_size / key_size; j++) {
	  columns[i] += str_buf[str_size / key_size * i + j];
	}
  }

  std::string str_ordered_columns;
  std::stringstream return_str_stream;
  int* int_key = keyDecode(key, key_size);

  for (int i = 0; i < key_size; i++) {
	for (int j = 0; j < key_size; j++) {
	  if (int_key[j] == i) {
		str_ordered_columns += columns[j];
	  }
	}
  }
  while (char_counter != str_size) {
	return_str_stream << str_ordered_columns[str_index];
	str_index += str_size / key_size;
	column_index++;
	char_counter++;
	if (column_index >= key_size) {
	  column_index %= key_size;
	  str_index %= str_size / key_size;
	  str_index++;
	}
  }

  return return_str_stream;
}

void write_to_file_binary(const char* fileName, std::stringstream& data) {
  std::ofstream fs(fileName, std::ios::out | std::ios::binary);
  fs << data.rdbuf();
  fs.close();
}


int main() {
  const char* key = "at;z";

  const char* inputData = "1.bmp";
  const char* encodedData = "encoded.bmp";
  const char* decodedData = "res.bmp";

  std::stringstream encoded_text_stream_doc;
  encoded_text_stream_doc = encode(inputData, true, key, 5);
  std::stringstream decoded_text_stream_doc;
  decoded_text_stream_doc = decode(encoded_text_stream_doc, key, 5);

  write_to_file_binary(encodedData, encoded_text_stream_doc);
  write_to_file_binary(decodedData, decoded_text_stream_doc);

  return 0;
}

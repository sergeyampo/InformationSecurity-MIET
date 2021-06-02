//
// Created by sergeyampo on 30.04.2021.
//

#ifndef INFORMATIONSECURITYMIET_SECOND_RSA_H_
#define INFORMATIONSECURITYMIET_SECOND_RSA_H_

#include <string>
#include <iostream>
#include <vector>


class RSA {
 private:

  std::string word;

  std::vector<int> encode_word;

  std::string decode_word;

  int e;

  int d;

  int n;

 public:
  RSA();

  void set_keys(int p, int q);

  static bool is_num_easy(int num);

  static bool is_easy2easy(int a, int b);

  void set_word(std::string word);

  bool encode();

  bool decode();

  void show_en_word() {
	std::cout << "Encode word: ";
	for (int i = 0; i < encode_word.size(); i++) std::cout << encode_word[i] << " ";
  }

  void show_de_word() { std::cout << "Decode word: " << decode_word << std::endl; }
};

#endif //INFORMATIONSECURITYMIET_SECOND_RSA_H_

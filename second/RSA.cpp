#include "RSA.h"
#include <iostream>
#include <math.h>


RSA::RSA() {
  encode_word.clear();
  decode_word = "";
  return;
}

bool RSA::is_num_easy(int num) {
  for (int i = 2; i < sqrt(num); i++)
	if (num % i == 0) return false;

  return true;
}

bool RSA::is_easy2easy(int a, int b) {
  if (a < b) {
	int i = a;
	a = b;
	b = a;
  }
  for (int i = 2; i <= b; i++)
	if (a % i == 0 && b % i == 0) return false;
  return true;
}

void RSA::set_keys(int p, int q) {
  e = 0;
  n = p * q;
  int fi = (p - 1) * (q - 1);
  do {
	for (int i = 2; i < n; i++)
	  if (RSA::is_easy2easy(fi, i) && (rand() % 10 > (8 - 2 / (2 * (n - i))))) {
		e = i;
		break;
	  }
  } while (e == 0);
  int k = 0;
  int check;
  do {
	k++;
	check = (k * fi + 1) % e;
  } while (check != 0);
  d = (k * fi + 1) / e;
  std::cout << "\nRes:\npublic_key: e = " << e << " n = " << n << "\nprivate_key: d = " << d << " n = " << n
			<< std::endl;
}

void RSA::set_word(std::string word) {
  encode_word.clear();
  decode_word = "";
  this->word = word;
}

bool RSA::encode() {
  for (int i = 0; i < word.size(); i++) {
	int word_int = (int)word[i];
	int word_c = word_int;
	for (int j = 1; j < e; j++)
	  word_int = (word_int * word_c) % n;
	encode_word.push_back(word_int);
  }
  return true;
}

bool RSA::decode() {
  for (int i = 0; i < word.size(); i++) {
	int word_int = encode_word[i];
	int word_c = word_int;
	for (int j = 1; j < d; j++)
	  word_int = (word_int * word_c) % n;
	decode_word += (char)word_int;
  }

  return true;
}
#include <iostream>
#include <string>
#include "RSA.h"
#include <time.h>
int main()
{
  srand(time(NULL));
  int p, q;
  do {
	std::cout << "Enter to easy num keys (p;q): ";
	std::cin >> p >> q; //23 37 47 67 83 97ex
  } while (!RSA::is_num_easy(p) || !RSA::is_num_easy(q));
  RSA safe;
  safe.set_keys(p, q);
  std::string word = "Source data to be encoded!";
  std::cout << "\nword: " << word << std::endl;
  safe.set_word(word);
  safe.encode();
  std::cout << "\n";
  safe.show_en_word();
  safe.decode();
  std::cout << "\n";
  safe.show_de_word();
  return 0;
}
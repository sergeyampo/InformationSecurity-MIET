//
// Created by sergeyampo on 02.06.2021.
//

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include "Constants.h"

using namespace std;

#define BUFF_SIZE 1024

// матрица подстановки


void key_extension(uint32_t* P, uint8_t* key_size, uint16_t len_bits);
size_t blowFish(uint8_t* block_to, uint8_t num_func, uint32_t* P, uint8_t* block_from, size_t length);

void feistel(uint8_t num_func, uint32_t* L, uint32_t* R, uint32_t* P);
void round_feistel(uint32_t* L, uint32_t* R, uint32_t P);
uint32_t F(uint32_t block32);

void into_64b_32b(uint64_t block64, uint32_t* block32_1, uint32_t* block32_2);
void into_64b_8b(uint64_t block64, uint8_t* blocks8);
void into_32b_8b(uint32_t block32, uint8_t* blocks8);

uint64_t into_32b_64b(uint32_t block32_1, uint32_t block32_2);
uint64_t into_8b_64b(uint8_t* blocks8);
uint64_t into_8b_32b(uint8_t* blocks8);

static inline size_t inputString(uint8_t* buffer);
static inline void swap(uint32_t* N1, uint32_t* N2);
static inline void printArray(uint8_t* array, size_t length);


// 18 * 32 = 576 bits
// PI = [K1..K18, S1..S4]

// матрица раундовых ключей


int main(void)
{
  uint8_t encoded[BUFF_SIZE], decoded[BUFF_SIZE];
  uint8_t buffer[BUFF_SIZE];
  uint8_t key[56] = "This is a crypto blowfish 448 bits key and 64 bits text"; // исходный ключ (448 бит)

  cout << "Введите сообщение: " << endl;
  size_t length = inputString(buffer); // к-во введённых символов
  size_t src_length = length;
  printArray(buffer, length); // вывод сообщения в виде чисел

  key_extension(FIXED_P, key, 448);

  length = blowFish(encoded, '1', FIXED_P, buffer, length);
  cout << "Зашифрованный вариант:\n";
  printArray(encoded, length);
  for (int i = 0; i < length; ++i) {
	cout << (char)encoded[i];
  }
  cout << "\n";

  length = blowFish(decoded, '2', FIXED_P, encoded, length);
  cout << "Дешифрованный вариант:\n";
  printArray(decoded, length);
  for (int i = 0; i < src_length; ++i) {
	cout << (char)decoded[i];
  }
  cout << "\n";

  return 0;
}

static inline size_t inputString(uint8_t* buffer) // ввод сообщения
{
  size_t ch_i = 0;
  uint8_t ch;
  // составляем массив из буферов
  while ((ch = getchar()) != '\n')
	buffer[ch_i++] = ch;
  buffer[ch_i] = '\0'; // терминальный 0
  return ch_i; // возвращает к-во введенных символов
}

// вывод информации в числах
static inline void printArray(uint8_t* array, size_t length)
{
  for (size_t i = 0; i < length; ++i)
	cout << (int)array[i] << " ";
  cout << endl;
}

// ********************************************************************************   Расширение ключа
// ***************************************************************************************************

void key_extension(uint32_t* P, uint8_t* key_size, uint16_t len_bits)
{
  const uint8_t block_size_in_bytes = len_bits / 8; // размер блока в байтах

  // Этап1. Исходный ключ преобразуется в матрицу раундовых ключей FIXED_P
  for (uint8_t i = 0; i < 18; ++i)
	P[i] ^= into_8b_32b(key_size + ((i * 4) % block_size_in_bytes)); // применяем операцию xor с FIXED_P для каждых 4-х байтов исходного ключа

  // Этап2
  uint8_t init_blocks8b[8] = { 0 }; // инициализирующий блок
  // зашифровываем инициализирующий блок новым ключом, полученным на этапе 1 с помощью BlowFish
  for (uint8_t i = 0; i < 18; i += 2)
  {
	blowFish(init_blocks8b, '1', P, init_blocks8b, 8); // шифрование
	P[i] = into_8b_32b(init_blocks8b); // складываем первые 4 байта и заносим значения в FIXED_P
	P[i + 1] = into_8b_32b(init_blocks8b + 4); // складываем вторые 4 байта и заносим значения в FIXED_P
  }

  // Этап3. Исходный ключ преобразуется в матрицу матрицу подстановки FIXED_S
  // зашифровываем инициализирующий блок новым ключом, полученным на этапе 1 с помощью BlowFish
  for (uint8_t i = 0; i < 4; ++i)
  {
	for (uint16_t j = 0; j < 256; j += 2)
	{
	  blowFish(init_blocks8b, '1', P, init_blocks8b, 8); // шифрование
	  FIXED_S[i][j] = into_8b_32b(init_blocks8b); // складываем первые 4 байта и заносим значения в FIXED_S (S[0])
	  FIXED_S[i][j + 1] = into_8b_32b(init_blocks8b + 4); // складываем вторые 4 байта и заносим значения в FIXED_S (S[1])
	}
  }
}

// **************************************************************************************** Шифрование
// ***************************************************************************************************

size_t blowFish(uint8_t* block_to, uint8_t num_func, uint32_t* P, uint8_t* block_from, size_t length)
{
  // вычисляем кратность блока
  if (length % 8 != 0) // если длина не кратна 8,
	length = length + (8 - (length % 8)); // то дополняем её

  uint32_t L, R; // блоки по 32 бита

  for (size_t i = 0; i < length; i += 8)
  {
	into_64b_32b(into_8b_64b(block_from + i), &L, &R);
	feistel(num_func, &L, &R, P);
	into_64b_8b(into_32b_64b(L, R), (block_to + i));
  }
  return length; // возвращает длину блока
}

// ------------------------------------------------------------------------------------- сеть Фейстеля

void feistel(uint8_t num_func, uint32_t* L, uint32_t* R, uint32_t* P)
{
  switch (num_func)
  {
	// шифрование
	case '1':
	{
	  // 16 раундов
	  for (int8_t round = 0; round < 16; ++round)
		round_feistel(L, R, P[round]);
	  swap(L, R); // меняем блоки местами
	  // последний раз применяем операцию xor
	  *R ^= P[16];
	  *L ^= P[17];
	  break;
	}

	  // дешифрование
	case '2':
	{
	  // 16 раундов
	  for (int8_t round = 17; round > 1; --round)
		round_feistel(L, R, P[round]);
	  swap(L, R); // меняем блоки местами
	  // последний раз применяем операцию xor
	  *L ^= P[0];
	  *R ^= P[1];
	  break;
	}
  }
}

// раунд сети Фейстеля
void round_feistel(uint32_t* L, uint32_t* R, uint32_t P)
{
  uint32_t t;
  *L ^= P; // xor с левым подблоком
  t = *L;
  *L = F(*L); // пименяем функцию F к левому подблоку
  *L ^= *R; // xor с правым подблоком
  *R = t;
}

// Функция F линейно преобразовывает входящие 32 бита данных в значение из матрицы подстановки
uint32_t F(uint32_t block32)
{
  uint8_t X[4];
  into_32b_8b(block32, X); // разделяем 32-битный блок на 4 по 8 бит
  block32 = FIXED_S[0][X[0]];
  block32 += FIXED_S[1][X[1]]; //сложение по модулю 2^32
  block32 ^= FIXED_S[2][X[2]];
  block32 += FIXED_S[3][X[3]];
  return block32;
}

// разделение 64 бита по 32
void into_64b_32b(uint64_t block64, uint32_t* block32_1, uint32_t* block32_2)
{
  *block32_1 = (uint32_t)(block64 >> 32);
  *block32_2 = (uint32_t)(block64);
}

// разделение 64 бита по 8
void into_64b_8b(uint64_t block64, uint8_t* blocks8)
{
  for (size_t i = 0; i < 8; ++i)
	blocks8[i] = (uint8_t)(block64 >> ((7 - i) * 8));
}

// разделение 32 бита по 8
void into_32b_8b(uint32_t block32, uint8_t* blocks8)
{
  for (uint8_t i = 0; i < 4; ++i)
	blocks8[i] = (uint8_t)(block32 >> (24 - (i * 8)));
}

// соединение 32-битных слов в 64-битное
uint64_t into_32b_64b(uint32_t block32_1, uint32_t block32_2)
{
  uint64_t block64 = (uint64_t)((uint64_t)block32_1 << 32) | block32_2;
  return block64;
}

// соединение 8 битов в 64 бита
uint64_t into_8b_64b(uint8_t* blocks8)
{
  uint64_t block64; // 64 бита
  for (uint8_t* i = blocks8; i < blocks8 + 8; ++i)
	block64 = (block64 << 8) | *i;
  return block64;
}

// соединение 8 битов в 32 бита
uint64_t into_8b_32b(uint8_t* blocks8)
{
  uint64_t block32;
  for (uint8_t* i = blocks8; i < blocks8 + 4; ++i)
	block32 = (block32 << 8) | *i;
  return block32;
}

// меняем 2 блока местами
static inline void swap(uint32_t* L, uint32_t* R)
{
  uint32_t t = *L;
  *L = *R;
  *R = t;
}
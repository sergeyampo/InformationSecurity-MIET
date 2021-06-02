//
// Created by sergeyampo on 30.04.2021.
//

#ifndef INFORMATIONSECURITYMIET_THIRD_BMPUTILS_BMPUTILS_H_
#define INFORMATIONSECURITYMIET_THIRD_BMPUTILS_BMPUTILS_H_

#include <cstddef>
#include <vector>
#include <array>
#include <fstream>
#include <algorithm>
#include <iostream>

#define HEADER_OFFSET 54

typedef struct tagRGBQUAD { char rgbBlue;char rgbGreen;char rgbRed;char rgbReserved; } RGBQUAD;

typedef unsigned long DWORD;    // Двойное слово - 32 бита (разряда)
typedef unsigned int WORD; //  Слово - 16 бит (разрядов)
typedef signed long LONG;
typedef unsigned int UINT;// Заголовок файла
typedef struct tagBITMAPFILEHEADER {
  WORD bfType;                // 'BM' = 4D42h
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;        // Смещениекрастру}
} BITMAPFILEHEADER;
// Заголовок Bitmap
typedef struct tagBITMAPINFOHEADER{        DWORD biSize;        LONG biWidth;        LONG biHeight;        WORD biPlanes;        WORD biBitCount;        DWORD biCompression;        DWORD biSizeImage;        LONG biXPelsPerMeter;        LONG biYPelsPerMeter;        DWORD biClrUsed;        DWORD biClrImportant;} BITMAPINFOHEADER;

auto readBMPContent(const std::string fileName) {
  std::ifstream ifs(fileName, std::ifstream::binary);

  std::array<std::byte, HEADER_OFFSET> headerBytes;

  ifs.read(reinterpret_cast<char*>(headerBytes.data()), headerBytes.size());

  std::vector<RGBQUAD> fileBytes;

  while (ifs.peek() != EOF) {
	char tmp[4] = {};
	ifs.read(tmp, 4);
	fileBytes.push_back(*(reinterpret_cast<RGBQUAD*>(tmp)));
  }

  ifs.close();

  return std::make_tuple(headerBytes, fileBytes);
}

void hideByteIntoPixel(RGBQUAD* pixel, std::byte hideByte) {
  std::byte three{3};
  pixel->rgbBlue &= (0xFC);
  pixel->rgbBlue |= std::to_integer<int>((hideByte >> 6) & three);
  pixel->rgbGreen &= (0xFC);
  pixel->rgbGreen |= std::to_integer<int>((hideByte >> 4) & three);
  pixel->rgbRed &= (0xFC);
  pixel->rgbRed |= std::to_integer<int>((hideByte >> 2) & three);
  pixel->rgbReserved &= (0xFC);
  pixel->rgbReserved |= std::to_integer<int>((hideByte) & three);
}

auto revealByteFromPixel(const RGBQUAD pixel) {
  std::byte result{0};
  int tmp = pixel.rgbBlue & 0x3;
  result |= *(reinterpret_cast<std::byte*>(&tmp));
  result <<= 2;
  tmp = pixel.rgbGreen & 0x3;
  result |= *(reinterpret_cast<std::byte*>(&tmp));
  result <<= 2;
  tmp = pixel.rgbRed & 0x3;
  result |= *(reinterpret_cast<std::byte*>(&tmp));
  result <<= 2;
  tmp = pixel.rgbReserved & 0x3;
  result |= *(reinterpret_cast<std::byte*>(&tmp));

  return result;
}

auto hideTextIntoPixels(std::vector<RGBQUAD> pixels, const std::string text) {
  if (pixels.size() < text.size()) {
	throw std::underflow_error("Not enough pixels to encode text!");
  }

  for (int i = 0; i < text.size(); i++) {
	char letter = text[i];
	std::byte letterByte = *(reinterpret_cast<std::byte*>(&letter));
	hideByteIntoPixel(&(pixels[i]), letterByte);
  }

  return pixels;
}

std::string revealTextFromPixels(std::vector<RGBQUAD> picturePixels) {
  std::string revealedText;
  for (auto pixel : picturePixels) {
	std::byte letterByte = revealByteFromPixel(pixel);
	char letter = *(reinterpret_cast<char*>(&letterByte));
	revealedText.push_back(letter);
  }

  return revealedText;
}

void savePicture(std::array<std::byte, HEADER_OFFSET> headerBytes,
				 std::vector<RGBQUAD> pixelsData,
				 std::string fileName) {
  std::ofstream ofs(fileName, std::ifstream::binary);

  for (auto byte : headerBytes) {
	ofs << *(reinterpret_cast<char*>(&byte));
  }

  for (auto pixel : pixelsData) {
	ofs << pixel.rgbBlue << pixel.rgbGreen << pixel.rgbRed << pixel.rgbReserved;
  }
  ofs.close();
}

#endif //INFORMATIONSECURITYMIET_THIRD_BMPUTILS_BMPUTILS_H_

#include <cstddef>
#include <vector>
#include <array>
#include <fstream>
#include <algorithm>
#include <iostream>
#include "./BMPUtils/BMPUtils.h"

#define HEADER_OFFSET 54

int main() {
  const std::string
	  fileName = "./1.bmp";
  auto pictureTuple = readBMPContent(fileName);

  std::string originalText = "You are awesome! Some text here.\nAnd some other!";

  auto revealedString = revealTextFromPixels(std::get<1>(pictureTuple));

  std::cout << revealedString << std::endl;

  auto encryptedPicturesPixels = hideTextIntoPixels(std::get<1>(pictureTuple), originalText);

  std::string outputFile = "result.bmp";
  savePicture(std::get<0>(pictureTuple), encryptedPicturesPixels, outputFile);

  pictureTuple = readBMPContent(outputFile);

  auto revealedString2 = revealTextFromPixels(std::get<1>(pictureTuple));

  std::cout << revealedString;

  return 0;
}
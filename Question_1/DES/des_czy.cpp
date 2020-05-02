#include "des_czy.h"

unsigned char* inBuff;
unsigned char* outBuff;

void encode() {
  memset(outBuff, 0x00, strlen((const char*)outBuff)); // initialized output buffer
  // unsigned char*
}

void decode() {

}

int main() {
  cout << "SELECT '1' for Encrypt; '2' for decrypt";
  int selection;
  cin >> selection;

  string plaintext;
  unsigned char buff[1024] = { 0 };

  // encrypt
  if (selection == 1) {
    ifstream plaintextFile("../Data/plaintext.txt");
    if (!plaintextFile.is_open()) {
      cout << "can't open plaintext file" << endl;
      exit(0);
    }
    getline(plaintextFile, plaintext);
    int dataLen = strlen(plaintext.c_str());
    inBuff = new unsigned char[dataLen + 8];
    memset(inBuff, 0, dataLen+8);
    memcpy(inBuff, (unsigned char*)plaintext.c_str(), dataLen);
  }
  // decrypt
  else {

  }
  return 0;
}
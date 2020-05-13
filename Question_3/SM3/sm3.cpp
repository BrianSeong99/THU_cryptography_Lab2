#include "sm3.h"

string plaintext;
string padded;

void padding() {
  padded = plaintext;
  padded += (char)(0x80);
  int size = padded.size() << 3;
  for (; size % 512 != 448; size += 8) {
    padded += (char)0;
  }
  for (int i = 56; i >= 0; i -= 8) {
    padded += (char)((plaintext.size()<<3)>>i);
  }
}

unsigned SLL(const unsigned &word,const int index) {
  return (word >> (32-index)) | (word << index);
}

string sm3() {
  unsigned int V[8] = { 0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600, 0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e };
  unsigned int W[68] = {0};
  unsigned int W1[64] = {0};
  for (int i = 0; i < padded.size(); i += 64) {
    for (int j = 0; j < 16; j ++) {
      W[j] =
        ((unsigned int) padded[i+4*j] & 0xff) << 24 |
        ((unsigned int) padded[i+4*j+1] & 0xff) << 16 |
        ((unsigned int) padded[i+4*j+2] & 0xff) << 8 |
        ((unsigned int) padded[i+4*j+3] & 0xff);
    }
    for (int t = 16; t < 68; t++) {
      unsigned int tmp = W[t-16]^W[t-9] ^ SLL(W[t-3],15);
      W[t] = tmp ^ SLL(tmp, 15) ^ SLL(tmp, 23) ^ SLL(W[t-13], 7) ^ W[t-6];
    }
    for (int t = 0; t < 64; t ++) {
      W1[t] = W[t] ^ W[t+4];
    }
    unsigned int a = V[0], b = V[1], c = V[2], d = V[3], e = V[4], f = V[5], g = V[6], h = V[7];
    for (int j=0;j<64;j++) {
      unsigned int ss1, ss2, tt1, tt2;
      if (j <= 15) {
        ss1 = SLL(SLL(a, 12) + e + SLL(0x79cc4519, j), 7);
        ss2 = ss1 ^ SLL(a, 12);
        tt1 = (a ^ b ^ c) + d + ss2 + W1[j];
        tt2 = (e ^ f ^ g) + h + ss1 + W[j];
      } else {
        ss1 = SLL(SLL(a, 12) + e + SLL(0x7a879d8a, j), 7);
        ss2 = ss1 ^ SLL(a, 12);
        tt1 = ((a & b) | (a & c) | (b & c)) + d + ss2 + W1[j];
        tt2 = ((e & f) | ((~e) & g)) + h + ss1 + W[j];
      }
      d = c;
      c = SLL(b, 9);
      b = a;
      a = tt1;
      h = g;
      g = SLL(f, 19);
      f = e;
      e = tt2 ^ SLL(tt2, 9) ^ SLL(tt2, 17);
    }
    V[0] = V[0] ^ a;
    V[1] = V[1] ^ b;
    V[2] = V[2] ^ c;
    V[3] = V[3] ^ d;
    V[4] = V[4] ^ e;
    V[5] = V[5] ^ f;
    V[6] = V[6] ^ g;
    V[7] = V[7] ^ h;
  }
  stringstream ss;
  ss << hex << V[0] << V[1] << V[2] << V[3] << V[4] << V[5] << V[6] << V[7];
  ss >> padded;
  return padded;
}

int main() {
  string srcPath = "../../Data/plaintext.txt";

  ifstream textFile(srcPath);
  if (!textFile.is_open()) {
    cout << "can't open src file" << endl;
    return 0;
  }
  getline(textFile, plaintext);

  clock_t begin = clock();
  padding();
  string digest = sm3();
  clock_t end = clock();

  double duration = double(end - begin) / CLOCKS_PER_SEC;
  cout << "SM3 Digest: " << digest << endl;
  cout << "SM3 duration: " << duration << endl;
  cout << "SM3 bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;
    return 0;
}

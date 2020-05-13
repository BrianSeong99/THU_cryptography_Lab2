#include "sha2.h"
using namespace std;

string plaintext;
string padded;

void padding() {
  padded += (char)(0x80);
  int size = padded.size() << 3;
  for (; size % 512 != 448; size += 8) {
    padded += (char)0;
  }
  for (int i = 56; i >= 0; i -= 8) {
    padded += (char)((plaintext.size()<<3)>>i);
  }
}

unsigned SRL(const unsigned &W, const int &index) {
  return ((W<<(32-index)) | (W>>index));
}

string sha256() {
  unsigned int H[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372,
    0xa54ff53a, 0x510e527f, 0x9b05688c,
    0x1f83d9ab, 0x5be0cd19
  };
  unsigned int W[64] = {0};
  for (int i = 0; i < padded.size(); i += 64) {
    for (int j = 0; j < 16; j ++) {
      W[j] = 
        ((unsigned int) padded[i+4*j] & 0xff) << 24 |
        ((unsigned int) padded[i+4*j+1] & 0xff) << 16 |
        ((unsigned int) padded[i+4*j+2] & 0xff) << 8 |
        ((unsigned int) padded[i+4*j+3] & 0xff);
    }
    for (int t = 16; t < 64; t ++) {
      W[t] = 
        W[t-16] + W[t-7] + (
          ((W[t-15] << 25) | (W[t-15] >> 7))
          ^ ((W[t-15] << 14) | (W[t-15] >> 18))
          ^ (W[t-15] >> 3)
        ) + (
          ((W[t-2] << 15) | (W[t-2] >> 17))
          ^ ((W[t-2] << 13) | (W[t-2] >> 19))
          ^ (W[t-2] >> 10)
        );
    }
    unsigned int a = H[0], b = H[1], 
      c = H[2], d = H[3], e = H[4],
      f = H[5], g = H[6], h = H[7];
    for (int t = 0; t < 64; t ++) {
      unsigned int s0, maj, t2, s1, ch, t1;
      s0 = ((a << 30) | (a >> 2))
        ^ ((a << 19) | (a >> 13))
        ^ ((a << 10) | (a >> 22));
      maj = (a & b) 
        ^ (a & c)
        ^ (b & c);
      t2 = s0 + maj;
      s1 = ((e << 26) | (e >> 6))
        ^ ((e << 21) | (e >> 11))
        ^ ((e << 7) | (e >> 25));
      ch = (e & f) ^ ((~e) & g);
      t1 = h + s1 + ch + K[t] + W[t];
      h = g, g = f, f = e, e = d+t1, d = c, c = b, b = a, a = t1+t2;
    }
    H[0] += a, H[1] += b, H[2] += c, H[3] += d, H[4] += e, H[5] += f, H[6] += g, H[7] += h;
  }
  stringstream ss;
  ss << hex << H[0] << H[1] << H[2] << H[3] << H[4] << H[5] << H[6] << H[7];
  ss >> padded;
  return padded;
}

int main() {
  string srcPath = "../../Data/plaintext.txt";
  string text = "";

  ifstream textFile(srcPath);
  if (!textFile.is_open()) {
    cout << "can't open src file" << endl;
    return 0;
  }
  getline(textFile, text);

  clock_t begin = clock();
  padding();
  string digest = sha256();
  clock_t end = clock();

  double duration = double(end - begin) / CLOCKS_PER_SEC;
  cout << "SHA-2 Digest: " << digest << endl;
  cout << "SHA-2 duration: " << duration << endl;
  cout << "SHA-2 bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

  return 0;
}

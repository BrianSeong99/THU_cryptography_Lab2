#include "sha3.h"

unsigned char *plaintext = new unsigned char[2048 + 512];
unsigned char *cyphertext = new unsigned char[(256>>3) + 1];
unsigned long long paddedLen = 16 * 1024;

unsigned long long SLL(const unsigned long long &word,const int &num) {
  return (word >> (64-num)) | (word << num);
}

void padding() {
  int left = (1088>>3) - ((paddedLen>>3) % (1088>>3));
  if (left) {
    if (left == 1) {
      plaintext[paddedLen>>3] = 0x86;
      paddedLen += 8;
    } else {
      plaintext[paddedLen>>3] = 0x06;
      paddedLen += 8;
      for (int j = 0; j < left-2; j ++) {
        plaintext[paddedLen>>3] =  0x00;
        paddedLen += 8;
      }
      plaintext[paddedLen>>3] = 0x80;
      paddedLen += 8;
    }
  }
}

void KECCAK_f(unsigned long long S[5][5]) {
  for (int i = 0; i < 24; i ++ ) {
    unsigned long long C[5], D[5];
    for (int j = 0; j < 5; j ++) {
      C[j] = S[j][0] ^ S[j][1] ^ S[j][2] ^ S[j][3] ^ S[j][4];
    }
    for (int j = 0; j < 5; j ++) {
      D[j] = C[(j+4)%5] ^ SLL(C[(j+1)%5], 1);
    }
    for (int j = 0; j < 5; j ++) {
      for(int k =0; k < 5; k ++){
        S[j][k] = S[j][k] ^ D[j];
      }
    }

    unsigned long long A_[5][5] = {};
    for (int j = 0; j < 5; j ++) {
      for (int k =0; k < 5; k ++) {
        A_[k][((j<<1) + k*3) % 5] = SLL(S[j][k], r[j][k]);
      }
    }
    for (int j = 0; j < 5; j ++) {
      for (int k =0; k < 5; k ++) {
        S[j][k] = A_[j][k] ^ ((~A_[(j+1) % 5][k]) & A_[(j+2) % 5][k]);
      }
    }

    S[0][0] = S[0][0] ^ RC[i];
  }
}

string sha3() {
  unsigned long long S[5][5] = {0};
  for (int i = 0; i < paddedLen/1088; i ++){
    unsigned char* block;
    for (int y = 0; y < 5; y++) {
      for (int x = 0; x < 5; x ++) {
        int cond = x+5*y;
        if (cond < 17){
          block = &plaintext[i * (1088>>3) + cond * 8];
          S[x][y] ^= *((unsigned long long*)block);
        }
      }
    }
    KECCAK_f(S);
  }
  for (int index = 0, y = 0, k = 0; y < 5; y ++) {
    for (int x = 0; x < 5; x ++) {
      int cond = x+5*y;
      if (cond < 17 && k < 4) {
        ((unsigned long long*)cyphertext)[index++] = S[x][y];
        k++;
      }
    }
  }
  stringstream ss;
  for(int i = 0; i < (256>>3); i++) {
    if((unsigned int)cyphertext[i] < 16) {
      ss << hex << '0';
    }
    ss << hex << (unsigned int) cyphertext[i];
  }
  string digest;
  ss >> digest;
  return digest;
}

int main() {
  cyphertext[256>>3] = '\0';
  string srcPath = "../../Data/plaintext.txt";
  string text = "";

  ifstream textFile(srcPath);
  if (!textFile.is_open()) {
    cout << "can't open src file" << endl;
    return 0;
  }
  getline(textFile, text);
  plaintext = (unsigned char*)text.c_str();

  clock_t begin = clock();
  padding();
  string digest = sha3();
  clock_t end = clock();
  
  double duration = double(end - begin) / CLOCKS_PER_SEC;
  cout << "SHA-3 Digest: " << digest << endl;
  cout << "SHA-3 duration: " << duration << endl;
  cout << "SHA-3 bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

  return 0;
}
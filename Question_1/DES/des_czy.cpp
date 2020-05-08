#include "des_czy.h"

unsigned char* inBuff;
unsigned char* outBuff;
bool keys[16][48];
int SELECTION;

class Utils {
public:
  static void byteToBit(bool *out, const unsigned char *in, int bits) {
    size_t i = 0;
    for (; i < bits; i ++) {
      out[i] = (in[i>>3] >> (7-i&7)) & 1;
    }
  }

  static void bitToByte(unsigned char *out, const bool *in, int bits) {
    memset(out, 0, bits >> 3);
    size_t i = 0;
    for (; i < bits; i ++) {
      out[i>>3] |= in[i] << (7-i&7);
    }
  }

  static void SLL(bool *in, int len, int loop) {
    bool tmp[256];
    memcpy(tmp, in, loop);
    memcpy(in, in+loop, len-loop);
    memcpy(in+len-loop, tmp, loop);
  }

  static void XOR(bool *inA, const bool *inB, int len) {
    size_t i = 0;
    for (; i<len; i ++) {
      inA[i] ^= inB[i];
    } 
  }

  static void permutationTransform(bool *out, bool *in, const char *table, int len) {
    bool tmp[256];
    size_t i = 0;
    for (; i<len; i ++) {
      tmp[i] = in[table[i] - 1];
    }
    memcpy(out, tmp, len);
  }

  static void sBoxTransform(bool out[32], const bool in[48]) {
    size_t i = 0;
	  for (; i < 8; i ++, in += 6, out += 4) {
      char j = (in[0] << 1) + in[5];
      char k = (in[1] << 3) + (in[2] << 2) + (in[3] << 1) + in[4];
      size_t l = 0;
      for (; l < 4; l ++) {
        out[l] = (SBox[i][j][k] >> (3 - l)) & 1;
      }
    }
  }

  static void fFunction(bool in[32], const bool subKey[48]) {
    bool tmp[48];
    permutationTransform(tmp, in, ETable, 48);
    XOR(tmp, subKey, 48);
    sBoxTransform(in, tmp);
    permutationTransform(in, in, PTable, 32);
  }
};

void setDESKeys(const unsigned char key[8]) {
  bool bitKey[64], *bitKeyLeft = &bitKey[0], *bitKeyRight = &bitKey[28];
  Utils::byteToBit(bitKey, key, 64);
  Utils::permutationTransform(bitKey, bitKey, PC1Table, 56);
  int i = 0;
  for (; i<16; i ++) {
    Utils::SLL(bitKeyLeft, 28, LoopTable[i]);
    Utils::SLL(bitKeyRight, 28, LoopTable[i]);
    Utils::permutationTransform(keys[i], bitKey, PC2Table, 48); // TODO check if keys have been updated
  }
}

void DES(unsigned char Out[8], const unsigned char In[8]) {
	bool M[64], tmp[32], *Li = &M[0], *Ri = &M[32];
	Utils::byteToBit(M, In, 64);
	Utils::permutationTransform(M, M, IPTable, 64);
	if (SELECTION == ENCRYPT) {
		for (int i = 0; i < 16; ++i) {
			memcpy(tmp, Ri, 32);        //Ri[i-1] 保存
			Utils::fFunction(Ri, keys[i]);  //Ri[i-1]经过转化和SBox输出为P
			Utils::XOR(Ri, Li, 32);            //Ri[i] = P XOR Li[i-1]
			memcpy(Li, tmp, 32);        //Li[i] = Ri[i-1]
		}
	}
	else {
		for (int i = 15; i >= 0; --i) {
			memcpy(tmp, Ri, 32);        //Ri[i-1] 保存
			Utils::fFunction(Ri, keys[i]);  //Ri[i-1]经过转化和SBox输出为P
			Utils::XOR(Ri, Li, 32);                   //Ri[i] = P XOR Li[i-1]
			memcpy(Li, tmp, 32);       //Li[i] = Ri[i-1]
		}
	}
	Utils::SLL(M, 64, 32);                   //Ri与Li换位重组M
	Utils::permutationTransform(M, M, IPInverseTable, 64);     //最后结果进行转化
	Utils::bitToByte(Out, M, 64);              //组织成字符
}

void run(unsigned char* iv, unsigned padlen) {
  unsigned char   cvec[8] = ""; // 扭转向量
	unsigned char   cvin[8] = ""; // 中间变量
  memcpy(cvec, iv, 8);

  for (int i = 0, j = padlen >> 3; i < j; ++i, outBuff += 8, inBuff += 8) {
    if (SELECTION == ENCRYPT) {
      for (int j = 0; j < 8; ++j)     //将输入与扭转变量异或
        cvin[j] = inBuff[j] ^ cvec[j];
    }
    else {
      memcpy(cvin, inBuff, 8);
    }

    DES(outBuff, cvin);
    if (SELECTION == ENCRYPT) {
      memcpy(cvec, outBuff, 8);         //将输出设定为扭转变量
    }
    else {
      for (int j = 0; j < 8; ++j)           //将输出与扭转变量异或 
        outBuff[j] = outBuff[j] ^ cvec[j];
      memcpy(cvec, cvin, 8);                //将输入设定为扭转变量
    }
  }

  cout << "COMPLETE" << endl;
}

int main() {
  string key;
  string iv;
  string text;
  string srcPath;
  string dstPath;

  // get key && initialize key variable && generate DES keys
  ifstream keyFile("../Data/key.txt");
  if (!keyFile.is_open()) {
    cout << "can't open key file" << endl;
    return 0;
  }
  getline(keyFile, key);
  int keyLen = strlen(key.c_str());
  unsigned char* rawKey = new unsigned char[keyLen];
  memcpy(rawKey, (unsigned char*)key.c_str(), keyLen);
  setDESKeys(rawKey);

  // get iv (Initial Vector)
  ifstream ivFile("../Data/iv.txt");
  if (!ivFile.is_open()) {
    cout << "can't open key file" << endl;
    return 0;
  }
  getline(ivFile, iv);
  int ivLen = strlen(iv.c_str());
  unsigned char* rawIV = new unsigned char[ivLen];
  memcpy(rawIV, (unsigned char*)iv.c_str(), ivLen);

  // get SELECTION for operation
  cout << "SELECT '1' for Encrypt; '2' for Decrypt" << endl << "> ";
  cin >> SELECTION;
  cout << endl;

  // encrypt
  if (SELECTION == 1) srcPath = "../Data/plaintext.txt";
  // decrypt
  else srcPath = "../Data/ciphertext.txt";

  // Read Plaintext Content && set inBuff
  ifstream textFile(srcPath);
  if (!textFile.is_open()) {
    cout << "can't open src file" << endl;
    return 0;
  }
  getline(textFile, text);
  int dataLen = strlen(text.c_str());
  inBuff = new unsigned char[dataLen + 8];
  memset(inBuff, 0, dataLen+8);
  memcpy(inBuff, (unsigned char*)text.c_str(), dataLen);

  // Initialize outBuff
  outBuff = new unsigned char[16010];
  memset(outBuff, 0x00, 16010);

  cout << outBuff << endl;
  // run DES algorithm
  run(rawIV, dataLen);

  dstPath = "../Data/ciphertext.txt";
	ofstream cipherFile(dstPath);
	if (!cin || !cipherFile.is_open()) {
    cout << "can't open dst file" << endl;
    return 0;
  }

  cout << "length of output: " << strlen((char*)outBuff) << endl;

  // for (int i = 0; i < strlen((char*)outBuff); i++) {
  string str(reinterpret_cast<char*>(outBuff));

  cipherFile << outBuff;
  cout << "output" << outBuff << endl;;
    // cout << outBuff[i];
  // }
	cout << "Write successfully!" << endl;
	cipherFile.close();
  return 0;
}
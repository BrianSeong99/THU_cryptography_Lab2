#include "des.h"
using namespace std;

static bool SubKey[16][48];// 16圈子密钥
char inputKey[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
char iv[8] = "0123456";
char plaintext[4096];
char ciphertext[4096];

void setDESKeys(const char Key[8]) {
	static bool K[64], *KL = &K[0], *KR = &K[28];
	ByteToBit(K, Key, 64);
	Transform(K, K, PC1_Table, 56);
	for(int i=0; i<16; i++) {
		RotateL(KL, 28, LOOP_Table[i]);
		RotateL(KR, 28, LOOP_Table[i]);
		Transform(SubKey[i], K, PC2_Table, 48);
	}
}

void encrypt() {
	int textLength = strlen(plaintext);
	int n = textLength / 8;
	char input[8];
	char output[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (int l = 0; l < n; l ++) {
		for (int i = 0; i < 8; i ++) {
			input[i] = plaintext[i+8*l];
		}
		if (l == 0) {
			for (int i = 0; i < 8; i ++) {
				input[i] = input[i] ^ iv[i];
			}
		} else {
			for (int i = 0; i < 8; i ++) {
				input[i] = input[i] ^ output[i];
			}
		}

		// DES encrypt
		static bool M[64], Tmp[32], *Li = &M[0], *Ri = &M[32];
		ByteToBit(M, input, 64); 
		Transform(M, M, IP_Table, 64);

		for(int i = 0; i < 16; i ++) {
			memcpy(Tmp, Ri, 32);
			F_func(Ri, SubKey[i]);   
			XOR(Ri, Li, 32);
			memcpy(Li, Tmp, 32);
		}

		for (int i = 0; i < 32; i ++) {
			bool temp = M[i];
			M[i] = M[i+32];
			M[i+32] = temp;
		}
			
		Transform(M, M, IPR_Table, 64);
		BitToByte(output, M, 64);

		for (int i = 0; i < 8; i ++) {
			ciphertext[i+8*l] = output[i];
		}
	}
}

void decrypt() {
	int textLength = strlen(ciphertext);
	int n = textLength / 8;
	char input[8];
	char output[8] = {0, 0, 0, 0, 0, 0, 0, 0};

	for (int i = 0; i < 8; i ++) {
		input[i] = ciphertext[i+8*(n-1)];
	}

	for (int l = n-1; l >= 0; l --) {
		static bool M[64], Tmp[32], *Li = &M[0], *Ri = &M[32];
		ByteToBit(M, input, 64); 
		Transform(M, M, IP_Table, 64);

		for(int i = 0; i < 16; i ++) {
			memcpy(Tmp, Ri, 32);
			F_func(Ri, SubKey[15-i]);
			XOR(Ri, Li, 32);
			memcpy(Li, Tmp, 32);
		}

		for (int i = 0; i < 32; i ++) {
			bool temp = M[i];
			M[i] = M[i+32];
			M[i+32] = temp;
		}
			
		Transform(M, M, IPR_Table, 64);
		BitToByte(output, M, 64);

		if (l == 0) {
			for (int i = 0; i < 8; i ++) {
				output[i] = output[i] ^ iv[i];
			}
		} else {
			for (int i = 0; i < 8; i ++) {
				input[i] = ciphertext[i+8*(l-1)];
				output[i] = input[i] ^ output[i];
			}
		}

		for (int i = 0; i < 8; i ++) {
			plaintext[i+8*l] = output[i];
		}
	}
}

bool judge(char text1[], char text2[], int size) {
	for (int i = size-1; i >=0; i --) {
		if (text1[i] != text2[i]) {
			cout << i << endl;
			return false;
		}
	}
	return true;
}

int main() {
	setDESKeys(inputKey);

	string srcPath = "../../Data/plaintext.txt";
	string dstPath = "../../Data/ciphertext.txt";
	string text = "";

  // Read Plaintext Content && set inBuff
  ifstream textFile(srcPath);
  if (!textFile.is_open()) {
    cout << "can't open src file" << endl;
    return 0;
  }
  getline(textFile, text);
	strcpy(plaintext, text.c_str());

	clock_t encryptBegin = clock();	
	encrypt();
	clock_t encryptEnd = clock();

	clock_t decryptBegin = clock();
	decrypt();
	clock_t decryptEnd = clock();

	char tmp[4096];
	strcpy(tmp, text.c_str());
	if(judge(tmp, plaintext, 2048)) {
		double duration = double(encryptEnd - encryptBegin) / CLOCKS_PER_SEC;
  	cout << "DES ENCRYPT duration: " << duration << endl;
  	cout << "DES ENCRYPT bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

		duration = double(decryptEnd - decryptBegin) / CLOCKS_PER_SEC;
		cout << "DES DECRYPT duration: " << duration << endl;
  	cout << "DES DECRYPT bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

	} else {
		cout << "DES encryption and decryption error" << endl;
	}

	return 0;
}
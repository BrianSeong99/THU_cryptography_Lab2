#include "des.h"
using namespace std;

static bool SubKey[16][48];// 16圈子密钥
char inputKey[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};
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
	int k = textLength % 8;
	int n = (textLength-1) / 8 + 1;
	char input[8];
	char output[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (int l = 0; l < n; l ++) {
		if (l == n-1 && k != 0) {
			for (int i = 0; i < k; i ++) {
				input[i] = plaintext[i+8*l];
			}
			for (int i = k; i < 8; i ++) {
				input[i] = ' ';
			}
		}
		else {
			for (int i = 0; i < 8; i ++) {
				input[i] = plaintext[i+8*l];
			}
		}
		for (int i = 0; i < 8; i ++) {
			input[i] = input[i] ^ output[i];
		}

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

int main() {
	setDESKeys(inputKey);
	for (int i = 0; i < 2048; i ++) {
		plaintext[i]='a';
	}
	
	encrypt();

	ofstream fout("2DES_CBC.txt");
	for (int i = 0; i < strlen(plaintext); i ++) {
		fout<<hex<<((int)(ciphertext[i]>>4)&0x0f)<<((int)ciphertext[i]&0x0f);
	}
	fout<<endl;
	fout.close();
	return 0;
}
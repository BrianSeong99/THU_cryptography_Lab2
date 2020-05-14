#include "aes128.h"
using namespace std;

unsigned char plaintext[4096];
unsigned char ciphertext[4096];
unsigned char State[4][4]; 
int keySize = 4;
unsigned char key[32]; 
unsigned char W[16*11];
unsigned char iv[16] = "012345678910111";


unsigned char GF(char multiNum, unsigned char bit) {
  switch(multiNum) {
    case '1':
      return bit;
    case '2':
      if (bit < 0x80) {
        return (unsigned char)(int)(bit << 1);
      } else {
        return (unsigned char)(
          (int)(bit << 1)
          ^ (int)(0x1b)
        );
      }
    case '3':
      return (unsigned char)(
        (int)GF('2', bit)
        ^ (int)bit
      );
    case '9':
      return (unsigned char)(
        (int)GF('2', GF('2', GF('2', bit)))
        ^ (int)bit
      );
    case 'b':
      return (unsigned char)(
        (int)GF('2', GF('2', GF('2', bit))) 
        ^ (int)GF('2', bit) 
        ^ (int)bit
      ); 
    case 'd':
      return (unsigned char)(
        (int)GF('2', GF('2', GF('2', bit)))
        ^ (int)GF('2', GF('2', bit))
        ^ (int)bit
      );
    case 'e':
      return (unsigned char)(
        (int)GF('2', GF('2', GF('2', bit)))
        ^ (int)GF('2', GF('2', bit))
        ^ (int)GF('2', bit)
      );
  }
}

void AddRoundKey(int round) {
  int i,j;
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) {
      State[i][j]=(unsigned char)((int)State[i][j]^(int)W[4*((round*4)+j)+i]); 
    }
  } 
} 

void SubBytes() {
  int i,j; 
  for(j=0; j<4; j++) {
    for(i=0; i<4; i++) {
      State[i][j]=Sbox[State[i][j]]; 
    }
  } 
} 

void InvSubBytes() {
  int i,j; 
  for(j = 0; j < 4; j ++) {
    for(i = 0; i < 4; i ++) { 
      State[i][j] = InverseSbox[State[i][j]];
    } 
  }
} 

void ShiftRows() {
  unsigned char temp[4*4];
  int i,j; 
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) {
      temp[4*i+j] = State[i][j]; 
    } 
  } 
  for (i=1; i<4; i++) { 
    for (j=0; j<4; j++) { 
      if (i == 1) {
        State[i][j] = temp[4*i+(j+1)%4];
      } else if (i == 2) {
        State[i][j] = temp[4*i+(j+2)%4];
      } else if (i == 3) {
        State[i][j] = temp[4*i+(j+3)%4];
      }
    } 
  }
} 

void InvShiftRows() {
  unsigned char temp[4*4]; 
  int i,j; 
  for(j = 0; j < 4; j ++) {
    for(i = 0; i < 4; i ++) {
      temp[4*i+j] = State[i][j]; 
    } 
  } 
  for (i = 1; i < 4; i ++) {
    for (j = 0; j < 4; j ++) {
      if (i == 1) {
        State[i][j] = temp[4*i+(j+3)%4];
      } else if (i == 2) {
        State[i][j] = temp[4*i+(j+2)%4];
      } else if (i == 3) {
        State[i][j] = temp[4*i+(j+1)%4];
      }
    } 
  } 
} 

void MixColumns() {
  unsigned char temp[4*4]; 
  int i,j; 
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) {
      temp[4*i+j]=State[i][j]; 
    }
  } 
  for(j=0;j<4;j++) {
    State[0][j] = (unsigned char)(
      (int)GF('2', temp[0+j])
      ^ (int)GF('3', temp[4*1+j])
      ^ (int)GF('1', temp[4*2+j]) 
      ^ (int)GF('1', temp[4*3+j])
    ); 
    State[1][j] = (unsigned char)(
      (int)GF('1', temp[0+j])
      ^ (int)GF('2', temp[4*1+j])
      ^ (int)GF('3', temp[4*2+j])
      ^ (int)GF('1', temp[4*3+j])
    ); 
    State[2][j] = (unsigned char)(
      (int)GF('1', temp[0+j]) 
      ^ (int)GF('1', temp[4*1+j]) 
      ^ (int)GF('2', temp[4*2+j]) 
      ^ (int)GF('3', temp[4*3+j])
    ); 
    State[3][j] = (unsigned char)(
      (int)GF('3', temp[0+j])
      ^ (int)GF('1', temp[4*1+j])
      ^ (int)GF('1', temp[4*2+j])
      ^ (int)GF('2', temp[4*3+j])
    ); 
  } 
} 

void InvMixColumns() {
  unsigned char temp[4*4]; 
  int i,j; 
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      temp[4*i+j] = State[i][j];
    }
  } 
     
  for (j = 0; j < 4; j++) { 
    State[0][j] = (unsigned char)(
      (int)GF('e', temp[j])
      ^ (int)GF('b', temp[4+j]) 
      ^ (int)GF('d', temp[4*2+j])
      ^ (int)GF('9', temp[4*3+j])
    ); 
    State[1][j] = (unsigned char)(
      (int)GF('9', temp[j]) 
      ^ (int)GF('e', temp[4+j]) 
      ^ (int)GF('b', temp[4*2+j]) 
      ^ (int)GF('d', temp[4*3+j])
    );
    State[2][j] = (unsigned char)(
      (int)GF('d', temp[j]) 
      ^ (int)GF('9', temp[4+j]) 
      ^ (int)GF('e', temp[4*2+j]) 
      ^ (int)GF('b', temp[4*3+j])
    ); 
    State[3][j] = (unsigned char)(
      (int)GF('b', temp[j]) 
      ^ (int)GF('d', temp[4+j])
      ^ (int)GF('9', temp[4*2+j])
      ^ (int)GF('e', temp[4*3+j])
    ); 
  }
} 

unsigned char* ROTWORD(unsigned char* word) {
  u_8* temp = new u_8[4];
  temp[3] = word[0];
  temp[2] = word[3];  
  temp[1] = word[2];
  temp[0] = word[1];
  return temp; 
} 

unsigned char* SUBWORD(unsigned char* word) {
  u_8* temp = new u_8[4]; 
  for(int j=0;j<4;j++) {
    temp[j] = Sbox[(word[j] & 0x0f) + 16 * (word[j] >> 4)];
  } 
  return temp; 
}

// reference from ppt Lecture5 pg23
void generateKey(unsigned char* inputKey) {
  memcpy(key, inputKey, 16);
  memset(W,0,16*11); 
  u_8* temp = new u_8[4]; 
  for (int row = 0; row < keySize; row ++) {
    for (int i = 0; i < 4; i ++) {
      W[4*row+i] =  key[4*row+i]; 
    }
  }
  for (int row = keySize; row < 4*(10+1); row ++) {
    for (int i = 0; i < 4; i ++) {
      temp[i] = W[4*row-(4-i)];
    }
    if (row%keySize == 0) {
      temp = ROTWORD(temp);
      temp = SUBWORD(temp);
      for (int i = 0; i < 4; i ++) {
        temp[i] = (u_8)( (int)temp[i] ^ (int) Rcon[4*(row/keySize)+i] ); 
      }
    } else if ( keySize > 6 && (row % keySize == 4) ) { 
      temp = SUBWORD(temp);
    }
    for (int i = 0; i < 4; i ++) {
      W[4*row+i] = (u_8) ( (int) W[4*(row-keySize)+i] ^ (int)temp[i] ); 
    }
  }
}

void encrypt() {
  int textLength = strlen((char*)plaintext);
  int n = textLength / 16;
  unsigned char input[16];
  unsigned char output[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // CBC setup
  for (int l = 0; l < n; l ++) {
    for (int i=0; i<16; i++) {
      input[i] = plaintext[i+16*l];
    }
    if (l == 0) {
			for (int i = 0; i < 16; i ++) {
				input[i] = input[i] ^ iv[i];
			}
		} else {
			for (int i = 0; i < 16; i ++) {
				input[i] = input[i] ^ output[i];
			}
		}

    // AES encrypt
    memset(&State[0][0], 0, 16); 
    for(int i = 0; i < 16; i ++) {
      State[i%4][i/4]=input[i];
    } 
    AddRoundKey(0);
     
    for (int round = 1; round < 10; round ++) {
      SubBytes();
      ShiftRows();
      MixColumns();
      AddRoundKey(round);
    }
     
    SubBytes();
    ShiftRows();
    AddRoundKey(10);
     
    // output = state 
    for (int i = 0; i < 16; i++) {
      output[i] = State[i % 4][ i / 4];
    } 

    // storing ciphertext
    for (int i = 0; i < 16; i ++) {
      ciphertext[i+16*l] = output[i];
    }
  }
}

void decrypt() {
  int textLength = strlen((char*)ciphertext);
  int n = textLength / 16;
  unsigned char input[16];
  unsigned char output[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  // CBC setup
  for (int i = 0; i < 16; i ++) {
    input[i] = ciphertext[i+16*(n-1)];
  }

  for (int l = n-1; l >= 0; l --) {
    // AES encrypt
    memset(&State[0][0], 0, 16); 
    for(int i = 0; i < 16; i ++) {
      State[i%4][i/4]=input[i];
    } 
    AddRoundKey(10);
     
    for (int round = 1; round < 10; round ++) {
      InvShiftRows();
      InvSubBytes();
      AddRoundKey(10-round);
      InvMixColumns();
    }
    
    InvShiftRows();
    InvSubBytes();
    AddRoundKey(0);
     
    // output = state 
    for (int i = 0; i < 16; i++) {
      output[i] = State[i % 4][i / 4];
    }

    if (l == 0) {
			for (int i = 0; i < 16; i ++) {
				output[i] = output[i] ^ iv[i];
			}
		} else {
			for (int i = 0; i < 16; i ++) {
				input[i] = ciphertext[i+16*(l-1)];
				output[i] = input[i] ^ output[i];
			}
		}

    // storing ciphertext
    for (int i = 0; i < 16; i ++) {
      plaintext[i+16*l] = output[i];
    }
  }
}

bool judge(unsigned char text1[], unsigned char text2[], int size) {
	for (int i = size-1; i >= 0; i --) {
		if (text1[i] != text2[i]) {
			cout << "error at: " << i << endl;
			return false;
		}
	}
	return true;
}

int main() {
  // set keys
  unsigned char inputKey[16] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};
  memcpy(key, inputKey, 16);
  generateKey(inputKey);

  memset(plaintext, 0, 4096);
  memset(ciphertext, 0, 4096);

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
	strcpy((char*)plaintext, text.c_str());

	clock_t encryptBegin = clock();	
	encrypt();
	clock_t encryptEnd = clock();

	clock_t decryptBegin = clock();
	decrypt();
	clock_t decryptEnd = clock();

	unsigned char tmp[4096];
	strcpy((char*)tmp, text.c_str());
	if(judge(tmp, plaintext, 2048)) {
    cout << "AES Key: ";
		for (int i = 0; i < 16; i ++) {
			cout << inputKey[i];
		}
		cout << endl;

		cout << "AES IV: ";
		for (int i = 0; i < 16; i ++) {
			cout << iv[i];
		}
		cout << endl;

		double duration = double(encryptEnd - encryptBegin) / CLOCKS_PER_SEC;
  	cout << "AES ENCRYPT duration: " << duration << endl;
  	cout << "AES ENCRYPT bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

		duration = double(decryptEnd - decryptBegin) / CLOCKS_PER_SEC;
		cout << "AES DECRYPT duration: " << duration << endl;
  	cout << "AES DECRYPT bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;

	} else {
		cout << "AES encryption and decryption error" << endl;
	}
  
  return 0;                           
}
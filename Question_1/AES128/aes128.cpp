#include "aes128.h"
using namespace std;

unsigned char plaintext[2048];
unsigned char ciphertext[2048];
unsigned char State[4][4]; 
int blockSize = 4;
int keySize = 4;
int rounds = 10;
unsigned char key[32]; 
unsigned char W[16*15];

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
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) { 
      State[i][j]=InverseSbox[State[i][j]];
    } 
  }
} 

void ShiftRows() {
  unsigned char temp[4*4];
  int i,j; 
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) {
      temp[4*i+j]=State[i][j]; 
    } 
  } 
  for(i=1;i<4;i++) { 
    for(j=0;j<4;j++) { 
      if(i==1)State[i][j]=temp[4*i+(j+1)%4];
      else if(i==2)State[i][j]=temp[4*i+(j+2)%4];
      else if(i==3)State[i][j]=temp[4*i+(j+3)%4];
    } 
  }
} 

void InvShiftRows() {
  unsigned char temp[4*4]; 
  int i,j; 
  for(j=0;j<4;j++) {
    for(i=0;i<4;i++) {
      temp[4*i+j]=State[i][j]; 
    } 
  } 
  for(i=1;i<4;i++) {
    for(j=0;j<4;j++) {
      if (i == 1) State[i][j] = temp[4*i+(j+3)%4];
      else if(i==2) State[i][j] = temp[4*i+(j+2)%4];
      else if(i==3) State[i][j] = temp[4*i+(j+1)%4];
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
      ^ (int)GF('3', temp[4*3+j])
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
  memset(W,0,16*15); 
  u_8* temp = new u_8[4]; 
  for (int row = 0; row < keySize; row ++) {
    for (int i = 0; i < 4; i ++) {
      W[4*row+i] =  key[4*row+i]; 
    }
  }
  for (int row = keySize; row < 4*(rounds+1); row ++) {
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
  int k = textLength % 16;
  int n = (textLength - 1) / 16 + 1;
  unsigned char input[16];
  unsigned char output[16] = {0};

  // CBC setup
  for (int l = 0; l < n; l ++) {
    if (l == n-1 && k != 0) {
      for (int i = 0; i < k; i ++) {
        input[i] = plaintext[i+16*l];
      }
      for (int i=k; i<16; i++) {
        input[i] = ' ';
      }
    }
    else {
      for (int i=0; i<16; i++) {
        input[i] = plaintext[i+16*l];
      }
    }
    for (int i=0; i<16; i++) {
      input[i] = input[i] ^ output[i]; // CBC
    }

    // AES encrypt
    memset(&State[0][0], 0, 16); 
    for(int i = 0; i < 4*blockSize; i ++) {
      State[i%4][i/4]=input[i];
    } 
    AddRoundKey(0);
     
    for (int round = 1; round < rounds; round ++) {
      SubBytes();
      ShiftRows();
      MixColumns();
      AddRoundKey(round);
    }
     
    SubBytes();
    ShiftRows();
    AddRoundKey(rounds);
     
    // output = state 
    for (int i = 0; i < (4 * blockSize); i++) {
      output[i] = State[i % 4][ i / 4];
    } 

    // storing ciphertext
    for (int i=0; i<16; i++) {
      ciphertext[i+16*l] = output[i];
    }
  }
}

int main() {
  // set keys
  unsigned char inputKey[16] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p'};
  memcpy(key, inputKey, 16);
  generateKey(inputKey);

  // set plaintext
  for (int i = 0; i < 2048; i ++) {
    plaintext[i] = 'a';
  }

  encrypt();
  
  ofstream fout("2AES_CBC_128.txt");
  for (int i=0;i<strlen((char*)plaintext);i++) {
    fout<<hex<<((int)(ciphertext[i]>>4)&0x0f)<<((int)ciphertext[i]&0x0f);
  }
  fout<<endl;
  fout.close();
  cout << "done" << endl;
  
  return 0;                           
}
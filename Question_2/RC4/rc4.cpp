#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include<ctime>
using namespace std;

unsigned char S[256];

void swap(int i ,int j){
    unsigned char temp=S[i];
    S[i]=S[j];
    S[j]=temp;
}

void init(char *plaintext, unsigned char *key, int keySize) {
    for (size_t i = 0; i < 256; i ++) {
        S[i] = (unsigned char)i;
    }
    unsigned char j = 0;
    for (size_t i = 0; i < 256; i++) {
        j = (j + S[i] + key[i%keySize]) % 256;
        swap(S[i], S[j]);
    }
}

void generateKeystream(unsigned char *key, unsigned char *keystream, int keystreamSize) {
    unsigned char a = 0, b = 0;
    for (int k = 0; k < keystreamSize; k ++) {
        a = (a + 1) % 256;
        b = (b + S[a]) % 256;
        swap(a, b);
        keystream[k] = S[(S[a]+S[b])%256];
    }
}

void output(unsigned char *keystream, int keystreamSize){
    cout << "keystream is: " << endl;
    for (int i = 0; i < int(keystreamSize/4); i ++) {
        cout << (unsigned int)keystream[i];
    }
    cout << endl;
}

int main() {
    char plaintext[] = "testing";
    unsigned char key[] = "thiskeyisjustsetfortestingandprobablyjustuseitbydefault";
    unsigned char keystream[2048];
    clock_t begin = clock();
    init(plaintext, key, int(sizeof(key)/sizeof(*key)));
    generateKeystream(key, keystream, 2048);
    clock_t end = clock();

    output(keystream, 2048);

    double duration = double(end - begin) / CLOCKS_PER_SEC;
    cout << "duration: " << duration << endl;
    cout << "bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;
}
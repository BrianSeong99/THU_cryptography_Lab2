#include<iostream>
#include<stdlib.h>
#include<ctime>
using namespace std;

int main() {
  unsigned int *a;
  unsigned long long N;

  cout << "Please input the sequence length: ";
  cin >> N;
  srand(1);
  a = new unsigned int[N];
  // long long num = 0;
  // cout << "Please input the sequence: ";
  // cin >> num;
  for (int i=0; i<N; i++) {
    a[i] = rand() % 2 ? 0 : 1;
  }

  clock_t begin = clock();
  unsigned int **f = new unsigned int*[N+1];
  unsigned int *l = new unsigned int[N+1];
  unsigned int *d = new unsigned int[N];
  for (unsigned long long n = 0; n < N+1; n ++) {
    f[n] = new unsigned int[N];
    memset(f[n], 0, N*sizeof(unsigned int));
  }
  memset(l, 0, (N+1)*sizeof(unsigned int));
  memset(d, 0, N*sizeof(unsigned int));

  f[0][0] = 1;
  unsigned long long n = 0, nn = 0, m = -1;
  for (; n < N; n++) {
    for (nn = 0; nn <= l[n]; nn++) {
      d[n] += f[n][nn] * a[n-nn];
    }
    d[n] &= 1;

    if (d[n] == 0) {
      memcpy(f[n+1], f[n], N*sizeof(unsigned int));
      l[n+1] = l[n];
    } else {
      if (l[n] > l[n-1]) {
        m = n - 1;
      }
      if (m != -1) {
        memcpy(f[n+1], f[n], N*sizeof(unsigned int));
        for (nn = 0; nn < N-n+m; nn++) {
          f[n+1][n+nn-m] += f[m][nn];
          f[n+1][n+nn-m] &= 1;
        }
        l[n+1] = l[n] >= n+1-l[n] ? l[n] : n+1-l[n];
      } else {
        f[n+1][0] = 1;
        f[n+1][n+1] = 1;
        l[n+1] = n + 1;
      }
    }
  }
  clock_t end = clock();

  cout << "f_" << N << "(x) = ";
  if (f[N][0] > 0) {
    printf("1");
  }
  for (unsigned long long nn = 1; nn < N; nn++) {
    if(f[N][nn] > 0) {
      cout << " + x^" << nn;
    }
  }
  cout << endl << "l[" << N << "] = " << l[N] << endl;

  double duration = double(end - begin) / CLOCKS_PER_SEC;
  cout << "duration: " << duration << endl;
  cout << "bandwidth: " << 1.0 / 64.0 / duration << "Mbps" << endl;
  return 0;
}
#include <iostream>
using namespace std;

static union {
  short _s;
  char _ca[sizeof(short)];
};

template <typename T>
void printBytes(const T &v)
{
  const char *b = (const char *)&v;
  int s = sizeof(T);
  for (int i = 0; i < s; i++) {
    if (i > 0) cout << ",";
    printf("%hhx", b[i]);
  }
  cout << endl;
}

int main(int argc, const char *argv[])
{
  _s = 0x1ff;
  if (_ca[0] == 0x1)
    cout << "big endian";
  else
    cout << "little endian";
  cout << endl;
  printBytes(_s);
  return 0;
}

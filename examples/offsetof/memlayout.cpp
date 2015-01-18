#include <iostream>
#include <stddef.h>     /* offsetof */

using namespace std;

class A {
public:
  int x;
  int y[10];
  char z;
};

int main() {

  cout << "A.x: " << offsetof(struct A, x) << " bytes" << endl;
  cout << "A.y: " << offsetof(struct A, y) << " bytes" << endl;
  cout << "A.z: " << offsetof(struct A, z) << " bytes" << endl;
  
  A a; 
  a.x = 10; a.y[0] = 12; a.z = 'W';
  

  void* ap = &a;
  int* iap = reinterpret_cast<int*>(ap);
  int* iarr = reinterpret_cast<int*>(ap);
  int iarr_v = *(iarr+4);
  char* cap = reinterpret_cast<char*>(ap);

  cout << "a.x: " << *iap << ", a.y[0]: " << *(iarr+8) << *(iarr+4) << ", a.z:" << *(cap+44) << endl;
  
  return 0;
};

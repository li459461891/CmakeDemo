#include <zmq/zmq.h>
#include <iostream>

int main() {
  int major, minor, patch = 0;
  zmq_version (&major, &minor, &patch);
  std::cout << "Zmq version is :" << major << "." << minor << "." << patch
    << std::endl;
  return 0;
}
#include "../timestamp.h"

#include <iostream>

int main() {
  std::cout << muduo_core::Timestamp::now().toString() << std::endl;
}


#include "../logger.h"

int main(){
  LOG_INFO("%s %s", "123", "123");
  LOG_DEBUG("123");
  LOG_FATAL("123");
  LOG_ERROR("123");
}


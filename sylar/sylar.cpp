#include <iostream>

#include "log.h"
#include "memory_test.h"

int main() {
  std::cout << "Hello, world!" << std::endl;
  std::cout << "size of time_t: " << sizeof(uint64_t) << '\n';
  auto logMessage = std::make_shared<sylar::LogEvent>("Hello World!");
  auto logger = sylar::Logger();
  auto stdoutAppender = std::make_shared<sylar::StdoutLogAppender>();
  auto formatter =
      std::make_shared<sylar::LogFormatter>("[%t] File:%f Line:%l [%v]");
  stdoutAppender->setLogFormatter(formatter);
  logger.addAppender(stdoutAppender);
  logger.debug(logMessage);
  return 0;
}

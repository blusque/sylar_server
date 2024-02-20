#include "log.h"

#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// class Logger {
//  public:
//   Logger(const std::string& filename) : m_filename(filename) {
//     m_file.open(filename, std::ios::app);
//   }

//   ~Logger() {
//     if (m_file.is_open()) {
//       m_file.close();
//     }
//   }

//   void log(const std::string& message) {
//     std::time_t now = std::time(nullptr);
//     std::string timestamp = std::ctime(&now);
//     timestamp.pop_back();  // Remove the newline character

//     m_file << "[" << timestamp << "] " << message << std::endl;
//   }

//  private:
//   std::string m_filename;
//   std::ofstream m_file;
// };

// int main() {
//   Logger logger("/home/ubuntu/sylar_server/log/log.txt");
//   std::cout << "Hello, world!\n";  // This will not be logged
//   // auto a = std::string();
//   // auto p_a = &a;
//   // std::cout << sizeof(a) << ' ' << sizeof(p_a) << ' ' << sizeof(int) << '
//   '
//   //           << sizeof(float) << '\n';
//   auto t = sylar::LogEvent();
//   std::cout << sizeof(t) << '\n';
//   logger.log("Hello, world!");

//   MyStruct obj;
//   MyStruct2 obj2;
//   MyStruct3 obj3;
//   MyStruct4 obj4;
//   MyStruct5 obj5;

//   std::cout << obj << obj2 << obj3 << obj4 << obj5 << std::endl;

//   return 0;
// }

namespace sylar {
std::string LogLevel::toString(Level level) {
  switch (level) {
#define XX(x)              \
  case LogLevel::Level::x: \
    return #x;

    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    default:
      return "UNKONW";
  }
}

LogEvent::LogEvent(const std::string& message)
    : m_file(__FILE__),
      m_line(__LINE__),
      m_content(message),
      m_time(std::time(nullptr)) {}

LogFormatter::LogFormatter(const std::string& pattern) : m_pattern(pattern) {
  init();
}

std::string LogFormatter::format(LogLevel::Level level, LogEvent::ptr event) {
  auto ss = std::stringstream();
  for (auto&& pair : m_pairs) {
    // std::cout << pair.first << '\n';
    ss << pair.first;
    // std::cout << pair.second << '\n';
    if (auto item = pair.second) {
      item->format(ss, level, event);
    }
  }
  ss << '\n';
  return ss.str();
}

/*
  %f  file  文件名
  %l  line  行号
  %t  time  日志时间 yyyy-mm-dd HH:MM:SS
  %e  elaspe 程序启动开始到现在的毫秒数
  %n	logger_name	创建logger时填入的名称
  %L	log_level	info,debug,track,error
  %T	threadID	线程ID
  %F  fiberID  协程ID
  %v	messageContent	日志正文
*/
void LogFormatter::init() {
  auto ss = std::stringstream();
  for (auto i = 0; i < m_pattern.size(); ++i) {
    // std::cout << "i: " << i << '\n';
    if (m_pattern[i] != '%') {
      ss << m_pattern[i];
      // std::cout << ss.str() << '\n';
      if (i == m_pattern.size() - 1) {
        m_pairs.emplace_back(ss.str(), nullptr);
      }
      continue;
    }
    if (++i == m_pattern.size()) {
      break;
    }
    if (m_pattern[i] == '%') {
      ss << m_pattern[i];
    } else {
      if (m_pattern[i] == 'f') {
        m_pairs.emplace_back(ss.str(), new FileItem);
      } else if (m_pattern[i] == 'l') {
        m_pairs.emplace_back(ss.str(), new LineItem);
      } else if (m_pattern[i] == 't') {
        m_pairs.emplace_back(ss.str(), new TimeItem);
      } else if (m_pattern[i] == 'e') {
        m_pairs.emplace_back(ss.str(), new ElapseItem);
      } else if (m_pattern[i] == 'L') {
        m_pairs.emplace_back(ss.str(), new LogLevelItem);
      } else if (m_pattern[i] == 'T') {
        m_pairs.emplace_back(ss.str(), new ThreadItem);
      } else if (m_pattern[i] == 'F') {
        m_pairs.emplace_back(ss.str(), new FiberItem);
      } else if (m_pattern[i] == 'v') {
        m_pairs.emplace_back(ss.str(), new ContentItem);
      }
      ss = std::stringstream();
    }
    // std::cout << ss.str() << '\n';
  }
}

Logger::Logger(const std::string& name) : m_name(name) {}

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    std::cout << "start logging!\n";
    for (auto&& app : m_appenders) {
      app->log(level, event);
    }
  }
}

void Logger::debug(LogEvent::ptr event) { log(LogLevel::Level::DEBUG, event); }

void Logger::info(LogEvent::ptr event) { log(LogLevel::Level::INFO, event); }

void Logger::warn(LogEvent::ptr event) { log(LogLevel::Level::WARN, event); }

void Logger::error(LogEvent::ptr event) { log(LogLevel::Level::ERROR, event); }

void Logger::fatal(LogEvent::ptr event) { log(LogLevel::Level::FATAL, event); }

void Logger::addAppender(LogAppender::ptr appender) {
  if (appender) {
    m_appenders.emplace_back(appender);
  }
}

void Logger::delAppender(LogAppender::ptr appender) {
  if (appender) {
    for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it) {
      if (*it == appender) {
        m_appenders.erase(it);
        break;
      }
    }
  }
}

void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    std::cout << "stdout logging!\n";
    std::cout << m_formatter->format(level, event);
  }
}

FileLogAppender::FileLogAppender(const std::string& filename)
    : m_filename(filename) {
  m_fstream.open(filename, std::ios::app);
}

void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event) {
  if (level >= m_level) {
    m_fstream << m_formatter->format(level, event);
  }
}

bool FileLogAppender::reopen() {
  if (m_fstream.is_open()) {
    m_fstream.close();
  }
  m_fstream.open(m_filename, std::ios::app);
  return m_fstream.is_open();
}

}  // namespace sylar

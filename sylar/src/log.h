#pragma once

#include <cstdio>
#include <ctime>
#include <fstream>
#include <list>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace sylar {

// 日志级别
class LogLevel {
 public:
  enum class Level {
    UNKNOW = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
  };
  [[nodiscard]] static std::string toString(Level level);
};

// 日志事件
class LogEvent {
 public:
  using ptr = std::shared_ptr<LogEvent>;
  LogEvent(const std::string& message);

  [[nodiscard]] const char* getFile() const { return m_file; }
  [[nodiscard]] int32_t getLine() const { return m_line; }
  [[nodiscard]] uint32_t getElapse() const { return m_elapse; }
  [[nodiscard]] uint32_t getThreadID() const { return m_threadId; }
  [[nodiscard]] uint32_t getFiberID() const { return m_fiberId; }
  [[nodiscard]] std::time_t getTime() const { return m_time; }
  [[nodiscard]] std::string getContent() const { return m_content; }

 private:
  const char* m_file = nullptr;  // 文件名
  int32_t m_line = 0;            // 行号
  uint32_t m_elapse = 0;         // 程序启动开始到现在的毫秒数
  uint32_t m_threadId = 0;       // 线程id
  uint32_t m_fiberId = 0;        // 协程id
  std::time_t m_time;            // 时间戳
  std::string m_content;
};

// 日志格式器
// "%t %l %m"
// "message{0}: {1}, {2}, {3}"
class LogFormatter {
 public:
  using ptr = std::shared_ptr<LogFormatter>;
  LogFormatter(const std::string& pattern);

  std::string format(LogLevel::Level level, LogEvent::ptr event);

 private:
  class Item {
   public:
    using ptr = std::shared_ptr<Item>;
    virtual ~Item() {}
    virtual void format(std::stringstream& ss, LogLevel::Level level,
                        LogEvent::ptr event) = 0;
  };

  friend class FileItem;
  friend class LineItem;
  friend class ElapseItem;
  friend class ThreadItem;
  friend class FiberItem;
  friend class TimeItem;
  friend class ContentItem;
  friend class LogLevelItem;

 private:
  void init();
  std::string m_pattern;
  std::vector<std::pair<std::string, Item::ptr>> m_pairs;
};

// 日志输出地
class LogAppender {
 public:
  using ptr = std::shared_ptr<LogAppender>;
  virtual ~LogAppender() {}

  virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

  [[nodiscard]] LogFormatter::ptr getLogFormatter() const {
    return m_formatter;
  }
  void setLogFormatter(LogFormatter::ptr formatter) { m_formatter = formatter; }

 protected:
  LogLevel::Level m_level{LogLevel::Level::DEBUG};
  LogFormatter::ptr m_formatter;
};

// 日志器
class Logger {
 public:
  using ptr = std::shared_ptr<Logger>;

  Logger(const std::string& name = "root");

  void log(LogLevel::Level level, LogEvent::ptr event);

  void debug(LogEvent::ptr event);
  void info(LogEvent::ptr event);
  void warn(LogEvent::ptr event);
  void error(LogEvent::ptr event);
  void fatal(LogEvent::ptr event);

  void addAppender(LogAppender::ptr appender);
  void delAppender(LogAppender::ptr appender);

  [[nodiscard]] LogLevel::Level getLogLevel() const { return m_level; }
  void setLogLevel(LogLevel::Level level) { m_level = level; }

  [[nodiscard]] static Logger& getLogger() {
    static auto logger = Logger();
    return logger;
  }

 private:
  std::string m_name;                               // 日志名称
  LogLevel::Level m_level{LogLevel::Level::DEBUG};  // 输出最低日志级别
  std::list<LogAppender::ptr> m_appenders;          // appender集合
};

class StdoutLogAppender : public LogAppender {
 public:
  using ptr = std::shared_ptr<StdoutLogAppender>;
  ~StdoutLogAppender() override {}

  void log(LogLevel::Level level, LogEvent::ptr event) override;

 private:
};

class FileLogAppender : public LogAppender {
 public:
  using ptr = std::shared_ptr<FileLogAppender>;
  FileLogAppender(const std::string& filename);
  ~FileLogAppender() override {}

  void log(LogLevel::Level level, LogEvent::ptr event) override;

  bool reopen();

 private:
  std::string m_filename;
  std::fstream m_fstream;
};

class FileItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~FileItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::string(event->getFile());
  }
};

class LineItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~LineItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::to_string(event->getLine());
  }
};

class ElapseItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~ElapseItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::to_string(event->getElapse());
  }
};

class ThreadItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~ThreadItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::to_string(event->getThreadID());
  }
};

class FiberItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~FiberItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::to_string(event->getFiberID());
  }
};

class TimeItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~TimeItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    auto time = event->getTime();
    auto timestamp = std::string(std::ctime(&time));
    timestamp.pop_back();
    ss << timestamp;
  }
};

class ContentItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~ContentItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << std::string(event->getContent());
  }
};

class LogLevelItem : public LogFormatter::Item {
 public:
  using ptr = std::shared_ptr<Item>;
  ~LogLevelItem() override {}
  void format(std::stringstream& ss, LogLevel::Level level,
              LogEvent::ptr event) override {
    ss << LogLevel::toString(level);
  }
};

}  // namespace sylar
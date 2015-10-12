#ifndef SERVER_STATE_H_
#define SERVER_STATE_H_

#include <queue>
#include <memory>
#include <string>

enum ServerStatus {
  DATA_PROCESSED = 0,
  COMMAND_PENDING = 1
};

class ServerState {
 public:
  ServerState();
  ~ServerState() = default;

  int GetSpeechRate() const { return speech_rate_; }

  void SetSpeechRate(const int speech_rate) { speech_rate_ = speech_rate; }

  std::string* GetMutableLastArgs() const { return last_args_.get(); }

  const std::string& GetLastArgs() const { return *last_args_.get(); }

  const std::string GetPrefixString() const;

  ServerStatus GetServerStatus() const { return server_status_; }

  void SetServerStatus(const ServerStatus server_status) {
    server_status_ = server_status;
  }

 private:
  int speech_rate_ = 50;
  std::queue<std::string> messages_;
  std::string last_command_;
  ServerStatus server_status_;

  std::unique_ptr<std::string> last_args_;
};

#endif  // SERVER_STATE_H_

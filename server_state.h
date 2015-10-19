#ifndef SERVER_STATE_H_
#define SERVER_STATE_H_

#include <queue>
#include <memory>
#include <string>

#include "audio_manager.h"
#include "audio_tasks.h"

enum ServerStatus { DATA_PROCESSED = 0, COMMAND_PENDING = 1 };

class ServerState {
 public:
  explicit ServerState(AudioManager* audio);
  ~ServerState() = default;

  std::string* GetMutableLastArgs() const { return last_args_.get(); }

  const std::string& GetLastArgs() const { return *last_args_.get(); }

  ServerStatus GetServerStatus() const { return server_status_; }

  void SetServerStatus(const ServerStatus server_status) {
    server_status_ = server_status;
  }

  AudioManager* audio() { return audio_; }
  std::queue<std::unique_ptr<AudioTask>>& queue() { return queue_; }

  void ClearQueue();

 private:
  AudioManager* audio_;
  std::queue<std::unique_ptr<AudioTask>> queue_;

  std::string last_command_;
  ServerStatus server_status_;

  std::unique_ptr<std::string> last_args_;
};

#endif  // SERVER_STATE_H_

#ifndef MESSAGES_H_
#define MESSAGES_H_

#include "tts.h"

#include <string>

class ServerState;

class Message {
 protected:
  Message() = default;

 public:
  virtual ~Message() = default;
  virtual void Do(TTS* tts, ServerState* server_state) = 0;
};

class SpeechMessage : public Message {
 public:
  SpeechMessage(std::string text);
  virtual void Do(TTS* tts, ServerState* server_state) override;

 private:
  std::string text_;
};

class CodeMessage : public Message {
 public:
  CodeMessage(std::string text);
  virtual void Do(TTS* tts, ServerState* server_state) override;

 private:
  std::string text_;
};


class SilenceMessage : public Message {
 public:
  SilenceMessage(const int duration);
  virtual void Do(TTS* tts, ServerState* server_state) override;

 private:
  int duration_;
};

#endif  // MESSAGES_H_

#include "messages.h"

#include "server_state.h"

SpeechMessage::SpeechMessage(std::string text) : text_(std::move(text)) {}

void SpeechMessage::Do(TTS* tts, ServerState* server_state) {
  // TODO: Implement clean (punctuation processing) before say.
  tts->Say(text_);
}

CodeMessage::CodeMessage(std::string text) : text_(std::move(text)) {}

void CodeMessage::Do(TTS* tts, ServerState* server_state) {
  tts->Output(text_);
}

SilenceMessage::SilenceMessage(const int duration) : duration_(duration) {}

void SilenceMessage::Do(TTS* tts, ServerState* server_state) {
  tts->GenerateSilence(duration_);
}

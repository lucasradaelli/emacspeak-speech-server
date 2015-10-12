#include "messages.h"

#include "server_state.h"

SpeechMessage::SpeechMessage(std::string text) : text_(std::move(text)) {}

void SpeechMessage::Do(TTS* tts, ServerState* server_state) {
  // TODO: Implement clean (punctuation processing) before say.
  tts->Say(server_state->GetPrefixString() + text_);
}

CodeMessage::CodeMessage(std::string text) : text_(std::move(text)) {}

void CodeMessage::Do(TTS* tts, ServerState* server_state) {
  tts->Say(text_);
}

#include "alsa_player.h"
#include "audio_manager.h"
#include "tts.h"
#include "speech_server.h"

int main(int argc, char** argv) {
  TTS::InitECI();

  AudioManager audio(std::unique_ptr<AlsaPlayer>(new AlsaPlayer));
  TTS tts(&audio);

  SpeechServer speech_server(&audio, &tts);
  speech_server.MainLoop();
}

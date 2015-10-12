#include <iostream>
#include <cstdio>
#include <cctype>

#include "command_generator.h"
#include "tts.h"
#include "speech_server.h"

int main(int argc, char** argv) {
  TTS::InitECI();
  AlsaPlayer alsa_player;
  TTS tts(&alsa_player);

  SpeechServer speech_server(&tts);
  speech_server.MainLoop();
}

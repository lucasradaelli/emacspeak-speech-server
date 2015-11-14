// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <cstdlib>
#include <iostream>

#include <boost/program_options.hpp>

#include "alsa_player.h"
#include "audio_manager.h"
#include "eci-c++.h"
#include "tts.h"
#include "speech_server.h"

namespace po = boost::program_options;

po::options_description GetOptionsDescription() {
  /* clang-format off */
  po::options_description options("Server options");
  options.add_options()
      ("help,h", "Display this help message.")
      ("verbose", "Be verbose about what is happening.")
      ("eci-library", po::value<std::string>()->value_name("path"),
       "Path to libibmeci.so library file to load.");

  po::options_description audio_options("Audio options");
  audio_options.add_options()
      ("device,D", po::value<std::string>()->value_name("name"),
       "ALSA device to play audio output.")
      ("rate,r", po::value<unsigned int>()->value_name("hz"),
       "Audio sample rate.")
      ("realtime",
       "Force a small buffer size, equivalent to --buffer-time=0.025.")
      ("buffer-time", po::value<double>()->value_name("seconds"),
       "Set the desired audio buffer time in seconds, which also affects the "
       "maximum audio latency.");
  options.add(audio_options);

  /* clang-format on */
  return options;
}

int main(int argc, char** argv) {
  po::options_description options = GetOptionsDescription();
  po::variables_map args;

  // Parse command line options.
  try {
    po::store(po::parse_command_line(argc, argv, options), args);
    po::notify(args);
  } catch (po::error& e) {
    std::cerr << "Error parsing command line options:\n"
              << e.what() << "\n";
    return EXIT_FAILURE;
  }

  if (args.count("help")) {
    std::cout << "speech_server [ OPTIONS ]\n"
              << "An IBM ViaVoice TTS speech server for Emacspeak.\n\n";
    std::cerr << options << "\n";
    return EXIT_SUCCESS;
  }

  const bool verbose = args.count("verbose");

  // Initialize the ECI library.
  const std::string eci_library_path =
      args.count("eci-library") > 0 ? args["eci-library"].as<std::string>()
                                    : TTS::kEciLibraryName;
  try {
    ECI::Init(eci_library_path.c_str());
  } catch (std::exception& e) {
    std::cerr << "Failed to load the IBM ViaVoice TTS ECI library:\n"
              << e.what() << "\n\n";
    return EXIT_FAILURE;
  }

  // Initialize the ALSA player.
  AlsaPlayer::Options alsa_options;
  alsa_options.verbose = verbose;
  if (args.count("devive")) {
    alsa_options.device = args["device"].as<std::string>();
  }
  if (args.count("rate")) {
    alsa_options.sample_rate = args["rate"].as<unsigned int>();
  }

  if (args.count("realtime")) {
    alsa_options.buffer_time = std::chrono::milliseconds(25);
  } else if (args.count("buffer-time")) {
    std::chrono::duration<double, std::chrono::seconds::period> duration(
        args["buffer-time"].as<double>());
    alsa_options.buffer_time =
        std::chrono::duration_cast<std::chrono::microseconds>(duration);
  }

  std::unique_ptr<AlsaPlayer> alsa_player(new AlsaPlayer(alsa_options));

  // Initialize the audio manager and the TTS manager.
  AudioManager audio(std::move(alsa_player));
  TTS tts(&audio);

  // Run the speech server.
  try {
    SpeechServer speech_server(&audio, &tts);
    speech_server.set_verbose(verbose);

    speech_server.MainLoop();
  } catch (std::exception& e) {
    std::cerr << "Fatal error while running the speech server:\n"
              << e.what() << "\n\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

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

#ifndef ECI_CPP_H_
#define ECI_CPP_H_

#include "eci.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

// Wrapper class for the IBM ViaVoice TTS Eloquence Command Interface (ECI)
// library.
//
// This class opens the IBM ViaVoice TTS ECI library dynamically, loads its
// symbols, and presents its API to the application as a convenient C++ object.
class ECI {
 public:
  using Callback = std::function<ECICallbackReturn(long)>;

  // Dynamically loads the IBM Text-to-Speech library.
  // This method must be called before instantiating this class. It throws an
  // exception if it fails to open the library or to load any of its symbols.
  static void Init(const std::string& library_path);

  ECI();
  ECI(ECILanguageDialect language);
  virtual ~ECI();

  // System control.
  void Reset();
  static void SpeakText(const std::string& text, bool annotations);
  static void SpeakTextEx(const std::string& text, bool annotations,
                          ECILanguageDialect language);
  static std::vector<ECILanguageDialect> GetAvailableLanguages();

  // Synthesis control.
  void AddText(const std::string& text);
  void ClearInput();
  void GeneratePhonemes(int size, void* buffer);
  int GetIndex();
  void InsertIndex(int index);
  void Pause(bool pause);
  bool Speaking();
  void Stop();
  void Synchronize();
  void Synthesize();
  void SynthesizeFile(const std::string& filename);

  // Output control.
  void SetOutputBuffer(int size, short* buffer);
  void SetOutputDevice(int num);
  void eciSetOutputFilename(const std::string& filename);

  // Speech environment parameter selection.
  static int GetDefaultParam(ECIParam parameter);
  static int SetDefaultParam(ECIParam parameter, int value);
  int GetParam(ECIParam parameter);
  int SetParam(ECIParam parameter, int value);

  // VoiceParameterControl.
  // TODO: Missing: eciCopyVoice, eciGetVoiceName, eciGetVoiceParam,
  //       eciSetVoiceName.

  // Dynamic Dictionary Maintenance.
  // TODO: Missing: (everything).

  // Diagnostics.
  int ProgStatus();
  std::string ErrorMessage();
  void ClearErrors();
  void TestPhrase();
  static std::string Version();

  // Callback.
  void SetCallback(ECIMessage message, Callback callback);

  // Custom Filters.
  // TODO: Missing: (everything).

 private:
  struct Library;

  static ECICallbackReturn Demuxer(ECIHand /*handle*/, ECIMessage message,
                                   long lparam, void* data);

  void Check(bool value);
  int Check(int value);

  static Library lib_;
  const ECIHand handle_;
  std::vector<Callback> callbacks_;
};

class ECIError : public std::runtime_error {
 public:
  ECIError(int status, const std::string& arg)
      : runtime_error(GenerateMessage(status, arg)), status_(status) {}
  virtual int status() const { return status_; }

 private:
  static std::string GenerateMessage(int status, const std::string& arg);
  const int status_;
};

#endif  // ECI_CPP_H_

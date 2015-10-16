#ifndef ECI_CPP_H_
#define ECI_CPP_H_

#include "eci.h"

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

// Frontend C++ class for the IBM TTS library.
//
// This class opens the IBM TTS library dynamically, loads its symbols,
// and presents its API to the application as a convenient C++ object.
class ECI {
 public:
  using Callback = std::function<ECICallbackReturn(long)>;

  static void Init(const char* library_path);

  ECI();
  ECI(ECILanguageDialect value);
  virtual ~ECI();

  // System control.
  // TODO: Missing: eciDeactivateFilter, eciSpeakText, eciSpeakTextEx.
  void Reset();

  // Synthesis control.
  // TODO: Missing: eciGeneratePhonemes, eciGetIndex, eciInsertIndex,
  //       eciSynthesizeFile.
  void AddText(const std::string& text);
  void ClearInput();
  void Pause(bool pause);
  bool Speaking();
  void Stop();
  void Synchronize();
  void Synthesize();

  // Output control.
  // TODO: Missing: eciSetOutputFilename.
  void SetOutputBuffer(int size, short* buffer);
  void SetOutputDevice(int num);

  // Speech environment parameter selection.
  // TODO: Missing: eciGetDefaultParam, eciGetParam, eciSetDefaultParam.
  int SetParam(ECIParam parameter, int value);

  // VoiceParameterControl.
  // TODO: Missing: eciCopyVoice, eciGetVoiceName, eciGetVoiceParam,
  //       eciSetVoiceName.

  // Dynamic Dictionary Maintenance.
  // TODO: Missing: (everything).

  // Diagnostics.
  // TODO: Missing: eciClearErrors, eciTestPhrase.
  int ProgStatus();
  std::string ErrorMessage();
  static std::string Version();

  // Callback.
  void SetCallback(ECIMessage message, Callback callback);

  // Custom Filters.
  // TODO: Missing: (everything).

  // Languages, other.
  static std::vector<ECILanguageDialect> GetAvailableLanguages();

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

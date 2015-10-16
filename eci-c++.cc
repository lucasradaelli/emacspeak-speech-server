#include "eci-c++.h"

#include <dlfcn.h>
#include <sstream>

struct ECI::Library {
  // dlopen() handle.
  void* handle;

  // Library Functions.
  ECIHand (*eciNew)();
  ECIHand (*eciNewEx)(ECILanguageDialect);
  ECIHand (*eciDelete)(ECIHand);
  Boolean (*eciReset)(ECIHand);
  Boolean (*eciAddText)(ECIHand, ECIInputText);
  Boolean (*eciClearInput)(ECIHand);
  Boolean (*eciPause)(ECIHand, Boolean);
  Boolean (*eciSpeaking)(ECIHand);
  Boolean (*eciStop)(ECIHand);
  Boolean (*eciSynchronize)(ECIHand);
  Boolean (*eciSynthesize)(ECIHand);
  Boolean (*eciSetOutputBuffer)(ECIHand, int, short*);
  Boolean (*eciSetOutputDevice)(ECIHand, int);
  int (*eciSetParam)(ECIHand, ECIParam, int);
  int (*eciProgStatus)(ECIHand);
  void (*eciErrorMessage)(ECIHand, void*);
  void (*eciVersion)(void*);
  Boolean (*eciRegisterCallback)(ECIHand, ECICallback, void*);
  int (*eciGetAvailableLanguages)(ECILanguageDialect*, int*);
};

ECI::Library ECI::lib_;

template <typename Type>
static void LoadSymbol(void* handle, Type** dest, const char* name)  {
  *dest = reinterpret_cast<Type*>(dlsym(handle, name));
  if (*dest == nullptr) {
    throw ECIError(-1, dlerror());
  }
}

void ECI::Init(const char* library_path) {
  lib_.handle = dlopen(library_path, RTLD_LAZY);
  if (lib_.handle == nullptr) {
    throw ECIError(-1, dlerror());
  }

  LoadSymbol(lib_.handle, &lib_.eciNew, "eciNew");
  LoadSymbol(lib_.handle, &lib_.eciNewEx, "eciNewEx");
  LoadSymbol(lib_.handle, &lib_.eciDelete, "eciDelete");
  LoadSymbol(lib_.handle, &lib_.eciReset, "eciReset");
  LoadSymbol(lib_.handle, &lib_.eciAddText, "eciAddText");
  LoadSymbol(lib_.handle, &lib_.eciClearInput, "eciClearInput");
  LoadSymbol(lib_.handle, &lib_.eciPause, "eciPause");
  LoadSymbol(lib_.handle, &lib_.eciSpeaking, "eciSpeaking");
  LoadSymbol(lib_.handle, &lib_.eciStop, "eciStop");
  LoadSymbol(lib_.handle, &lib_.eciSynchronize, "eciSynchronize");
  LoadSymbol(lib_.handle, &lib_.eciSynthesize, "eciSynthesize");
  LoadSymbol(lib_.handle, &lib_.eciSetOutputBuffer, "eciSetOutputBuffer");
  LoadSymbol(lib_.handle, &lib_.eciSetOutputDevice, "eciSetOutputDevice");
  LoadSymbol(lib_.handle, &lib_.eciSetParam, "eciSetParam");
  LoadSymbol(lib_.handle, &lib_.eciProgStatus, "eciProgStatus");
  LoadSymbol(lib_.handle, &lib_.eciErrorMessage, "eciErrorMessage");
  LoadSymbol(lib_.handle, &lib_.eciVersion, "eciVersion");
  LoadSymbol(lib_.handle, &lib_.eciRegisterCallback, "eciRegisterCallback");
  LoadSymbol(lib_.handle, &lib_.eciGetAvailableLanguages,
             "eciGetAvailableLanguages");
}

ECI::ECI() : handle_(lib_.eciNew()) {
  Check(handle_ != nullptr);
}

ECI::ECI(ECILanguageDialect value) : handle_(lib_.eciNewEx(value)) {
  Check(handle_ != nullptr);
}

ECI::~ECI() {
  lib_.eciDelete(handle_);
}

void ECI::Reset() {
  Check(lib_.eciReset(handle_));
}

void ECI::AddText(const std::string& text) {
  Check(lib_.eciAddText(handle_, text.c_str()));
}

void ECI::ClearInput() {
  Check(lib_.eciClearInput(handle_));
}

void ECI::Pause(bool pause) {
  Check(lib_.eciPause(handle_, pause));
}

bool ECI::Speaking() {
  return lib_.eciSpeaking(handle_);
}

void ECI::Stop() {
  Check(lib_.eciStop(handle_));
}

void ECI::Synchronize() {
  Check(lib_.eciSynchronize(handle_));
}

void ECI::Synthesize() {
  Check(lib_.eciSynthesize(handle_));
}

void ECI::SetOutputBuffer(int size, short* buffer) {
  Check(lib_.eciSetOutputBuffer(handle_, size, buffer));
}

void ECI::SetOutputDevice(int num) {
  Check(lib_.eciSetOutputDevice(handle_, num));
}

int ECI::SetParam(ECIParam parameter, int value) {
  return Check(lib_.eciSetParam(handle_, parameter, value));
}

int ECI::ProgStatus() {
  return lib_.eciProgStatus(handle_);
}

std::string ECI::ErrorMessage() {
  char buffer[100];
  lib_.eciErrorMessage(handle_, buffer);
  return buffer;
}

std::string ECI::Version() {
  char buffer[20];
  lib_.eciVersion(buffer);
  return buffer;
}

void ECI::SetCallback(ECIMessage message, Callback callback) {
  if (callbacks_.empty()) {
    lib_.eciRegisterCallback(handle_, &ECI::Demuxer, this);
  }
  const std::size_t min_messages = message + 1;
  if (callbacks_.size() < min_messages) {
    callbacks_.resize(min_messages);
  }
  callbacks_[message] = std::move(callback);
}

ECICallbackReturn ECI::Demuxer(ECIHand, ECIMessage message, long int lparam,
                               void* data) {
  ECI* eci = static_cast<ECI*>(data);
  if (message < eci->callbacks_.size() && eci->callbacks_[message]) {
    return eci->callbacks_[message](lparam);
  }
  return eciDataProcessed;
}

std::vector<ECILanguageDialect> ECI::GetAvailableLanguages() {
  int num_languages = 0;
  if (lib_.eciGetAvailableLanguages(nullptr, &num_languages) != 0) {
    throw ECIError(0, "Failed to get available languages from ECI.");
  }
  std::vector<ECILanguageDialect> languages(num_languages);
  if (lib_.eciGetAvailableLanguages(languages.data(), &num_languages) != 0) {
    throw ECIError(0, "Failed to get available languages from ECI.");
  }
  languages.resize(num_languages);
  return languages;
}

void ECI::Check(bool value) {
  if (!value) {
    throw ECIError(ProgStatus(), ErrorMessage());
  }
}

int ECI::Check(int value) {
  if (value < 0) {
    throw ECIError(ProgStatus(), ErrorMessage());
  }
  return value;
}

std::string ECIError::GenerateMessage(int status, const std::string& arg) {
  std::ostringstream sstr;
  sstr << "ECI Error (" << status << "): " << arg;
  return sstr.str();
}

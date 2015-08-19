#include "tts.h"
#include "alsa_player.h"

#include <sys/time.h>
#include <dlfcn.h>
#include <alloca.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>




using std::string;
using std::vector;

namespace {


 void     (*eciVersion) (char *);
 void    *(*eciNewEx) (enum ECILanguageDialect);
 int      (*eciGetAvailableLanguages) (enum
                                              ECILanguageDialect *, int *);
 void     (*eciDelete) (void *);
 int      (*eciReset) (void *);
 int      (*eciStop) (void *);
 int      (*eciClearInput) (void *);
 int      (*eciPause) (void *, int);
 int      (*eciSynthesize) (void *);
 int      (*eciSynchronize) (void *);
 int      (*eciSpeaking) (void *);
 int      (*eciAddText) (void *, char *);
 int      (*eciInsertIndex) (void *, int);
 int      (*eciSetParam) (void *, int, int);
 int      (*eciGetVoiceParam) (void *, int, int);
 int      (*eciSetVoiceParam) (void *, int, int, int);
 int      (*eciSetOutputBuffer) (void *, int, short *);
 int      (*eciSetOutputDevice) (void *, int);
 void     (*eciRegisterCallback) (void *,
                                         int (*)(void *, int,
                                                 long, void *), void *);


} // namespace



bool TTS::InitECI() {
  void           *eciLib;
  // < configure shared library symbols

  eciLib = dlopen(ECILIBRARYNAME, RTLD_LAZY);
  if (eciLib == NULL) {
    std::cerr <<    "Could not load   " <<  ECILIBRARYNAME << "\n";
    return false;
  }

  eciVersion =
      (void (*)(char *)) (unsigned long) dlsym(eciLib, "eciVersion");
  eciGetAvailableLanguages =
      (int (*)(enum ECILanguageDialect *, int *)) (unsigned long)
      dlsym(eciLib, "eciGetAvailableLanguages");
  eciNewEx = (void *(*)(enum ECILanguageDialect)) (unsigned long)
      dlsym(eciLib, "eciNewEx");
  eciDelete =
      (void (*)(void *)) (unsigned long) dlsym(eciLib, "eciDelete");
  eciReset = (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciReset");
  eciStop = (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciStop");
  eciClearInput =
      (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciClearInput");
  eciPause =
      (int (*)(void *, int)) (unsigned long) dlsym(eciLib, "eciPause");
  eciSynthesize =
      (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciSynthesize");
  eciSynchronize =
      (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciSynchronize");
  eciSpeaking =
      (int (*)(void *)) (unsigned long) dlsym(eciLib, "eciSpeaking");
  eciInsertIndex =
      (int (*)(void *, int)) (unsigned long) dlsym(eciLib,
                                                   "eciInsertIndex");
  eciAddText =
      (int (*)(void *, char *)) (unsigned long) dlsym(eciLib,
                                                      "eciAddText");
  eciSetParam =
      (int (*)(void *, int, int)) (unsigned long) dlsym(eciLib,
                                                        "eciSetParam");
  eciGetVoiceParam = (int (*)(void *, int, int))
      (unsigned long) dlsym(eciLib, "eciGetVoiceParam");
  eciSetVoiceParam = (int (*)(void *, int, int, int))
      (unsigned long) dlsym(eciLib, "eciSetVoiceParam");
  eciRegisterCallback = (void
                          (*)(void *,
                              int (*)(void *, int, long,
                                      void *), void *)) (unsigned long)
      dlsym(eciLib, "eciRegisterCallback");
  eciSetOutputBuffer = (int (*)(void *, int, short *)) (unsigned long)
      dlsym(eciLib, "eciSetOutputBuffer");
  eciSetOutputDevice =
      (int (*)(void *, int)) (unsigned long) dlsym(eciLib,
                                                   "eciSetOutputDevice");

  // >
  // < check for needed symbols

  int             okay = 1;
  if (!eciNewEx) {
    okay = 0;
    std::cerr <<   "eciNewEx undef\n";
  }
  if (!eciDelete) {
    okay = 0;
    std::cerr <<   "eciDelete undef\n";
  }
  if (!eciReset) {
    okay = 0;
    std::cerr <<  "eciReset undef\n";
  }
  if (!eciStop) {
    okay = 0;
    std::cerr <<   "eciStop undef\n";
  }
  if (!eciClearInput) {
    okay = 0;
std::cerr <<  "eciClearInput undef\n";
  }
  if (!eciPause) {
    okay = 0;
 std::cerr << "eciPause undef\n";
  }
  if (!eciSynthesize) {
    okay = 0;
  std::cerr << "eciSynthesize undef\n";
  }
  if (!eciSpeaking) {
    okay = 0;
    std::cerr << "eciSpeaking undef\n";
  }
  if (!eciInsertIndex) {
    okay = 0;
    std:: cerr <<  "eciInsertIndex undef\n";
  }
  if (!eciAddText) {
    okay = 0;
    std:: cerr <<  "eciAddText undef\n";
  }
  if (!eciSetParam) {
    okay = 0;
    std::cerr <<  "eciSetParam undef\n";
  }
  if (!eciSetParam) {
    okay = 0;
    std:: cerr <<   "eciSetParam undef\n";
  }
  if (!eciGetVoiceParam) {
    okay = 0;
    std:: cerr <<  "eciGetVoiceParam undef\n";
  }
  if (!eciSetVoiceParam) {
    okay = 0;
    std:: cerr <<  "eciSetVoiceParam undef\n";
  }
  if (!eciRegisterCallback) {
    okay = 0;
    std::cerr <<  "eciRegisterCallback undef\n";
  }
  if (!eciSetOutputBuffer) {
    okay = 0;
    std::cerr <<   "eciSetOutputBuffer undef\n";
  }
  if (!eciSetOutputDevice) {
    okay = 0;
    std::cerr <<  "eciSetOutputDevice undef\n";
  }
  if (!eciGetAvailableLanguages) {
    okay = 0;
    std::cerr <<  "eciGetAvailableLanguages undef\n";
  }

  if (!okay) {
    std::cerr << "Failed to  load the lib\n";
return false;
  }

  return true;
}



TTS::TTS(AlsaPlayer* alsa_player, const Options& options) : alsa_player_(alsa_player) {
   int             rc;
   size_t          chunk_bytes = 0;
   ECILanguageDialect a_languages[LANG_INFO_MAX];
   int             n_languages = LANG_INFO_MAX;
   eciGetAvailableLanguages(a_languages, &n_languages);

   lang_switcher_.reset( new LangSwitcher(a_languages,n_languages));

    ECILanguageDialect a_default_language =
       lang_switcher_.InitLanguage( );

   if (a_default_language == NODEFINEDCODESET) {
     throw TTSError("No languages found\n");
   }

   eci_handle_ = eciNewEx(a_default_language);
   if (eci_handle_ == nullptr) {
     throw TTSError("Could not open text-to-speech engine.\n");
   }

   // Initialize TTS
   if ((eciSetParam(eci_handle_, eciInputType, 1) == -1)
       || (eciSetParam(eci_handle_, eciSynthMode, 1) == -1)
       || (eciSetParam(eci_handle_, eciSampleRate, options.sample_rate) == -1)) {
     eciDelete(eci_handle_);
     throw ECIError("Could not initialized tts");
   }

   eciRegisterCallback(eci_handle_, +[](void* eci_handle, ECIMessage msg, long lparam, void* data) {
   TTS* tts  = static_cast<TTS*>(data);
  if (msg == eciIndexReply) {
    tts->SetLastReply(lparam);
  } else if ((msg == eciWaveformBuffer) && (lparam > 0)) {
    tts->PlayTTS(lparam);
  }
       return eciDataProcessed;
     } , this);

   // Set output to bufferl.
   rc = eciSynchronize(eci_handle_);
   if (!rc) {
     throw ECIError("Error  resetting TTS engine.\n");
   }
   rc = eciSetOutputBuffer(eci_handle_, alsa_player_->period_size(), alsa_player_->buffer());
   if (!rc) {
     throw ECIError("Error setting output buffer.\n");
   }

 }

~TTS::TTS() {
     eciDelete(eci_handle_);
}


int TTS::PlayTTS(const int count) {
  alsa_player_->Play(count);
  return eciDataProcessed;
}

bool TTs::IsSpeaking() {
  if (eciSpeaking(eci_handle_)) {
    return true;
  } else {
    return false;
  }
}


bool TTs::Synchronize() {
  int rc = eciSynchronize(eci_handle_);
  if (rc) {
    return true;
  } else {
    return false;
  }
}



string LangSwitcher::GetDefaultLanguageCode() {
  const char* a_default_lang = getenv("LANGUAGE");
  if (a_default_lang == NULL)
    {
      a_default_lang = getenv("LANG");
      if (a_default_lang == NULL)
        {
          a_default_lang = "en";
        }
    }
  if (strlen(a_default_lang) < 2)
  {
    a_default_lang = "en";
  }
  // TODO: Test if the language is available on the list of supported languages.
  return a_default_lang;
}

bool LangSwitcher::GetValidLanguages( vector<int>* available_languages_index) {
  for (int i = 0; i < n_languages_; i++)
    {
      for (int j = 0; j < static_cast<int>(the_languages_.size()); j++)
      {
        if (a_languages_[i] == the_languages_[j].lang) {
          available_languages_index->push_back(j);
        }
    }
    }
  return !available_languages_index->empty();
}

 ECILanguageDialect LangSwitcher::InitLanguage() {
  vector<int> available_languages_index;
  if (!GetValidLanguages(&available_languages_index)) {
    return NODEFINEDCODESET;
  }

  const string a_default_lang_code = GetDefaultLanguageCode();
   ECILanguageDialect a_current_language, a_english_language, a_first_language;
  a_current_language = a_english_language = a_first_language = NODEFINEDCODESET;
  for (const int index: available_languages_index) {
      if (a_current_language == NODEFINEDCODESET)
        {
          if (strncmp(a_default_lang_code.c_str(), the_languages_[index].code, 2) == 0)
            {
              a_current_language = the_languages_[index].lang;
            }
          else if (strncmp("en", the_languages_[index].code, 2) == 0)
            {
              a_english_language = the_languages_[index].lang;
            }
        }
  }

  a_first_language = the_languages_[available_languages_index[0]].lang;
  if (a_current_language == NODEFINEDCODESET)
    {
      if (a_english_language != NODEFINEDCODESET)
      {
        a_current_language = a_english_language;
      }
      else if (a_first_language != NODEFINEDCODESET)
        {
          a_current_language = a_first_language;
        }
    }


  return a_current_language;
}

#ifndef INCLUDE_UNICODE2GSM_UNICODE2GSM_H_
#define INCLUDE_UNICODE2GSM_UNICODE2GSM_H_

#include "common.h"
#include <string>

namespace unicode2gsm {
    bool init_transliteration_map(bool transliterate_gsm_extended);
    
    bool requires_transliteration(const char* utf8);
    bool requires_transliteration(const char* utf8, size_t length);
    
    std::string transliterate(const char* utf8);
    std::string transliterate(const char* utf8, size_t length);
}

#endif

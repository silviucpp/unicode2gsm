#ifndef INCLUDE_UNICODE2GSM_CHARSET_H_
#define INCLUDE_UNICODE2GSM_CHARSET_H_

#include "unicode2gsm/common.h"
#include <string>

namespace unicode2gsm {

bool init_lookup_tables(bool map_gsm_extended, const char** charmap_lookup_table, bool* gsm_lookup_table, size_t max_elements);

}

#endif

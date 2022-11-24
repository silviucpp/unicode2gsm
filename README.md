# unicode2gsm

[![Build Status](https://travis-ci.com/silviucpp/unicode2gsm.svg?branch=master)](https://travis-ci.com/github/silviucpp/unicode2gsm)
[![GitHub](https://img.shields.io/github/license/silviucpp/unicode2gsm)](https://github.com/silviucpp/unicode2gsm/blob/master/LICENSE)

## What it is ?

A library that transliterates Unicode characters outside of GSM alphabet with a similar GSM-encoded character. This helps ensure that your message gets segmented at 160 characters and saves you from sending multiple message segments, which increases your spend.

When Unicode characters are used in an SMS message, they must be encoded as UCS-2. However, UCS-2 characters take 16 bits to encode, so when a message includes a Unicode character, it will be split or segmented between the 70th and 71st characters. This is shorter than the 160-character per message segment that you get with GSM-7 character encoding.

For example, sometimes a Unicode character such as a smart quote ( 〞), a long dash (—), or a Unicode whitespace accidentally slips into your carefully crafted 125-character message. Now, your message is segmented and priced at two messages instead of one.

## Building

Building the static library:

```sh
mkdir build
cd build
cmake ..
make
```

In case you want to build the unit tests make sure you have `libboost-test-dev` installed and run `cmake` as follow:

```sh
ENABLE_UNIT_TEST=1 cmake ..
```

## API

### Initialize the library

You can include the library as follow:

```c++
#include <unicode2gsm/unicode2gsm.h>
```

Before using any API function you need to initialize the library using:

```c++
bool init_transliteration_map(bool transliterate_gsm_extended)
```

Extended GSM alphabet symbols are escaped so each one will count as 2 characters. For the symbols that have a decent GSM-encoded replacement you can optionally enable transliteration using `transliterate_gsm_extended` parameter and these symbols will be mapped as follow:

- `{` -> `(`
- `}` -> `)`
- `[` -> `(`
- `]` -> `)`
- `~` -> `-`

### Check if a string requires transliteration

```c++
// for NULL terminated strings
bool requires_transliteration(const char* utf8);                

// can be used for strings that are not NULL terminated.
bool requires_transliteration(const char* utf8, size_t length); 
```
    
### Perform transliteration

```c++
// for NULL terminated strings
std::string transliterate(const char* utf8);

// can be used for strings that are not NULL terminated.
std::string transliterate(const char* utf8, size_t length);
```

### Example

The following example is passing to transliteration process all characters from the GSM alphabet. The output will be equal with the input 
but if we change the parameter passed to `init_transliteration_map` to `true` the result will be different as `{ } [ ] ~` will be transliterated.

```c++

#include <unicode2gsm/unicode2gsm.h>
#include <string>

int main()
{
    if(!unicode2gsm::init_transliteration_map(false))
    {
        printf("Failed to init\n");
        return 1;
    }
    
    const char *only_gsm=u8"@£$¥èéùìòÇØøÅåΔ_ΦΓΛΩΠΨΣΘΞÆæßÉ!\"#¤%&'()*+,-./0123456789:;<=>?¡ABCDEFGHIJKLMNOPQRSTUVWXYZÄÖÑÜ§¿abcdefghijklmnopqrstuvwxyzäöñüà^{}\[~]|€";
    
    // the output here is equal with the value of `only_gsm` variable
    printf("Output: %s", unicode2gsm::transliterate(only_gsm).c_str());    
    return 0;
}

```

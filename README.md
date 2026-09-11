# StringUtils.hpp - Mini String Library

String helper functions missing from the standard library, in a single C++23 header. No dependencies besides the standard library, just drop the file into your project.

## Why

The standard library covers the basics but leaves out a lot of the small things every project ends up rewriting: trimming, splitting, padding, case conversion, wildcard matching. This puts all of that in one place.

## Features

- Header only, one file, zero dependencies
- Works with `std::string_view` wherever possible, avoids unnecessary copies
- Trimming: `Trim`, `TrimLeft`, `TrimRight`, `TrimChars`
- Case conversion: `ToLower`, `ToUpper`, `Capitalize`, `TitleCase`, `SwapCase`
- Predicates: `StartsWith`, `EndsWith`, `Contains`, `ContainsIgnoreCase`, `EqualsIgnoreCase`, `IsBlank`, `IsNumeric`, `IsAlpha`, `IsAlphaNumeric`, `IsUpper`, `IsLower`
- Split and join: `Split`, `SplitWhitespace`, `SplitSkipEmpty`, `Join`
- Replace and clean: `Replace`, `ReplaceFirst`, `RemoveAll`, `RemoveChar`, `CollapseWhitespace`
- Padding: `PadLeft`, `PadRight`, `PadCenter`
- Repeat and reverse: `Repeat`, `Reverse`
- Substring extraction: `Truncate`, `TruncateWithSuffix`, `Between`, `Before`, `After`, `BeforeLast`, `AfterLast`
- Counting: `Count`, `CountChar`
- Numeric conversion without exceptions: `ToNumber<T>`, `FromNumber<T>`
- Encoding: `ToHex`, `FromHex`, `EscapeQuotes`, `EscapeHtml`
- String distance: `LevenshteinDistance`, `SimilarityRatio`
- Random string generation: `RandomAlphaNumeric`
- Wildcard matching with `*` and `?`: `WildcardMatch`

## Requirements

C++23. Uses `std::from_chars` / `std::to_chars`, ranges, and `std::string_view`, so you need a reasonably recent compiler (GCC 13+, Clang 17+, MSVC 2022 latest).

## Usage

Just include the header:

```cpp
#include "StringUtils.hpp"
using namespace strutil;

std::string clean = Trim("  hello world  ");
auto parts = Split("a,b,c", ',');
std::string joined = Join(parts, " | ");

bool matches = WildcardMatch("report_2026.csv", "report_*.csv");
```

### Numeric conversion

```cpp
auto value = ToNumber<int>("42");
if (value) {
    // *value == 42
}
```

### String similarity

```cpp
double ratio = SimilarityRatio("kitten", "sitting"); // 0.0 to 1.0
size_t distance = LevenshteinDistance("kitten", "sitting");
```

## License

MIT, see LICENSE.
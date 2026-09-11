// StringUtils.hpp
// A collection of string helper functions missing from the standard library, C++23, header only, no dependencies.
//
// Example:
//   auto parts = strutil::Split("a,b,c", ',');
//   std::string s = strutil::Trim("  hello  ");

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <optional>
#include <charconv>
#include <cstdint>
#include <random>
#include <format>

namespace strutil {

// Trimming

[[nodiscard]] inline std::string_view TrimLeft(std::string_view s) {
    size_t start = s.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string_view::npos) return {};
    return s.substr(start);
}

[[nodiscard]] inline std::string_view TrimRight(std::string_view s) {
    size_t end = s.find_last_not_of(" \t\n\r\f\v");
    if (end == std::string_view::npos) return {};
    return s.substr(0, end + 1);
}

[[nodiscard]] inline std::string_view Trim(std::string_view s) {
    return TrimLeft(TrimRight(s));
}

[[nodiscard]] inline std::string_view TrimChars(std::string_view s, std::string_view chars) {
    size_t start = s.find_first_not_of(chars);
    if (start == std::string_view::npos) return {};
    size_t end = s.find_last_not_of(chars);
    return s.substr(start, end - start + 1);
}

// Case conversion

[[nodiscard]] inline std::string ToLower(std::string_view s) {
    std::string result(s);
    std::ranges::transform(result, result.begin(),
        [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

[[nodiscard]] inline std::string ToUpper(std::string_view s) {
    std::string result(s);
    std::ranges::transform(result, result.begin(),
        [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

// Capitalizes the first character, lowercases the rest.
[[nodiscard]] inline std::string Capitalize(std::string_view s) {
    if (s.empty()) return {};
    std::string result = ToLower(s);
    result[0] = static_cast<char>(std::toupper(static_cast<unsigned char>(result[0])));
    return result;
}

// Uppercases the first letter of every word, words are split on whitespace.
[[nodiscard]] inline std::string TitleCase(std::string_view s) {
    std::string result(s);
    bool startOfWord = true;
    for (char& c : result) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            startOfWord = true;
        } else if (startOfWord) {
            c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
            startOfWord = false;
        } else {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
    }
    return result;
}

// Toggles the case of every character.
[[nodiscard]] inline std::string SwapCase(std::string_view s) {
    std::string result(s);
    for (char& c : result) {
        if (std::isupper(static_cast<unsigned char>(c))) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        else if (std::islower(static_cast<unsigned char>(c))) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    }
    return result;
}

// Predicates

[[nodiscard]] inline bool StartsWith(std::string_view s, std::string_view prefix) {
    return s.size() >= prefix.size() && s.compare(0, prefix.size(), prefix) == 0;
}

[[nodiscard]] inline bool EndsWith(std::string_view s, std::string_view suffix) {
    return s.size() >= suffix.size() && s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

[[nodiscard]] inline bool Contains(std::string_view s, std::string_view needle) {
    return s.find(needle) != std::string_view::npos;
}

[[nodiscard]] inline bool ContainsIgnoreCase(std::string_view s, std::string_view needle) {
    return Contains(ToLower(s), ToLower(needle));
}

[[nodiscard]] inline bool EqualsIgnoreCase(std::string_view a, std::string_view b) {
    if (a.size() != b.size()) return false;
    return std::ranges::equal(a, b, [](unsigned char x, unsigned char y) {
        return std::tolower(x) == std::tolower(y);
    });
}

[[nodiscard]] inline bool IsBlank(std::string_view s) {
    return Trim(s).empty();
}

[[nodiscard]] inline bool IsNumeric(std::string_view s) {
    if (s.empty()) return false;
    return std::ranges::all_of(s, [](unsigned char c) { return std::isdigit(c); });
}

[[nodiscard]] inline bool IsAlpha(std::string_view s) {
    if (s.empty()) return false;
    return std::ranges::all_of(s, [](unsigned char c) { return std::isalpha(c); });
}

[[nodiscard]] inline bool IsAlphaNumeric(std::string_view s) {
    if (s.empty()) return false;
    return std::ranges::all_of(s, [](unsigned char c) { return std::isalnum(c); });
}

[[nodiscard]] inline bool IsUpper(std::string_view s) {
    bool hasCase = false;
    for (unsigned char c : s) {
        if (std::isalpha(c)) {
            hasCase = true;
            if (!std::isupper(c)) return false;
        }
    }
    return hasCase;
}

[[nodiscard]] inline bool IsLower(std::string_view s) {
    bool hasCase = false;
    for (unsigned char c : s) {
        if (std::isalpha(c)) {
            hasCase = true;
            if (!std::islower(c)) return false;
        }
    }
    return hasCase;
}

// Split / Join

[[nodiscard]] inline std::vector<std::string> Split(std::string_view s, char delim) {
    std::vector<std::string> result;
    size_t start = 0;
    while (start <= s.size()) {
        size_t pos = s.find(delim, start);
        if (pos == std::string_view::npos) {
            result.emplace_back(s.substr(start));
            break;
        }
        result.emplace_back(s.substr(start, pos - start));
        start = pos + 1;
    }
    return result;
}

[[nodiscard]] inline std::vector<std::string> Split(std::string_view s, std::string_view delim) {
    std::vector<std::string> result;
    if (delim.empty()) {
        result.emplace_back(s);
        return result;
    }
    size_t start = 0;
    while (true) {
        size_t pos = s.find(delim, start);
        if (pos == std::string_view::npos) {
            result.emplace_back(s.substr(start));
            break;
        }
        result.emplace_back(s.substr(start, pos - start));
        start = pos + delim.size();
    }
    return result;
}

// Splits on any run of whitespace, empty tokens are discarded.
[[nodiscard]] inline std::vector<std::string> SplitWhitespace(std::string_view s) {
    std::vector<std::string> result;
    size_t i = 0;
    while (i < s.size()) {
        while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        size_t start = i;
        while (i < s.size() && !std::isspace(static_cast<unsigned char>(s[i]))) ++i;
        if (i > start) result.emplace_back(s.substr(start, i - start));
    }
    return result;
}

// Like Split, but empty tokens between consecutive delimiters are dropped.
[[nodiscard]] inline std::vector<std::string> SplitSkipEmpty(std::string_view s, char delim) {
    std::vector<std::string> result;
    for (auto& piece : Split(s, delim)) {
        if (!piece.empty()) result.push_back(std::move(piece));
    }
    return result;
}

template <typename Range>
[[nodiscard]] inline std::string Join(const Range& parts, std::string_view delim) {
    std::string result;
    bool first = true;
    for (const auto& part : parts) {
        if (!first) result.append(delim);
        result.append(part);
        first = false;
    }
    return result;
}

// Replace

[[nodiscard]] inline std::string Replace(std::string_view s, std::string_view from, std::string_view to) {
    if (from.empty()) return std::string(s);
    std::string result;
    result.reserve(s.size());
    size_t start = 0;
    while (true) {
        size_t pos = s.find(from, start);
        if (pos == std::string_view::npos) {
            result.append(s.substr(start));
            break;
        }
        result.append(s.substr(start, pos - start));
        result.append(to);
        start = pos + from.size();
    }
    return result;
}

[[nodiscard]] inline std::string ReplaceFirst(std::string_view s, std::string_view from, std::string_view to) {
    if (from.empty()) return std::string(s);
    size_t pos = s.find(from);
    if (pos == std::string_view::npos) return std::string(s);
    std::string result;
    result.reserve(s.size());
    result.append(s.substr(0, pos));
    result.append(to);
    result.append(s.substr(pos + from.size()));
    return result;
}

[[nodiscard]] inline std::string RemoveAll(std::string_view s, std::string_view target) {
    return Replace(s, target, "");
}

[[nodiscard]] inline std::string RemoveChar(std::string_view s, char c) {
    std::string result;
    result.reserve(s.size());
    for (char ch : s) if (ch != c) result.push_back(ch);
    return result;
}

// Collapses any run of consecutive whitespace into a single space, and trims the ends.
[[nodiscard]] inline std::string CollapseWhitespace(std::string_view s) {
    std::string result;
    result.reserve(s.size());
    bool lastWasSpace = true;
    for (char c : s) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!lastWasSpace) result.push_back(' ');
            lastWasSpace = true;
        } else {
            result.push_back(c);
            lastWasSpace = false;
        }
    }
    while (!result.empty() && result.back() == ' ') result.pop_back();
    return result;
}

// Padding / alignment

[[nodiscard]] inline std::string PadLeft(std::string_view s, size_t width, char fill = ' ') {
    if (s.size() >= width) return std::string(s);
    return std::string(width - s.size(), fill) + std::string(s);
}

[[nodiscard]] inline std::string PadRight(std::string_view s, size_t width, char fill = ' ') {
    if (s.size() >= width) return std::string(s);
    return std::string(s) + std::string(width - s.size(), fill);
}

[[nodiscard]] inline std::string PadCenter(std::string_view s, size_t width, char fill = ' ') {
    if (s.size() >= width) return std::string(s);
    size_t total = width - s.size();
    size_t left = total / 2;
    size_t right = total - left;
    return std::string(left, fill) + std::string(s) + std::string(right, fill);
}

// Repeat / reverse

[[nodiscard]] inline std::string Repeat(std::string_view s, size_t count) {
    std::string result;
    result.reserve(s.size() * count);
    for (size_t i = 0; i < count; ++i) result.append(s);
    return result;
}

[[nodiscard]] inline std::string Reverse(std::string_view s) {
    return std::string(s.rbegin(), s.rend());
}

// Substring helpers

[[nodiscard]] inline std::string_view Truncate(std::string_view s, size_t maxLength) {
    if (s.size() <= maxLength) return s;
    return s.substr(0, maxLength);
}

// Truncates to maxLength and appends suffix if truncation happened, total length capped at maxLength.
[[nodiscard]] inline std::string TruncateWithSuffix(std::string_view s, size_t maxLength, std::string_view suffix = "...") {
    if (s.size() <= maxLength) return std::string(s);
    if (suffix.size() >= maxLength) return std::string(suffix.substr(0, maxLength));
    return std::string(s.substr(0, maxLength - suffix.size())) + std::string(suffix);
}

[[nodiscard]] inline std::string_view Between(std::string_view s, std::string_view left, std::string_view right) {
    size_t start = s.find(left);
    if (start == std::string_view::npos) return {};
    start += left.size();
    size_t end = s.find(right, start);
    if (end == std::string_view::npos) return {};
    return s.substr(start, end - start);
}

[[nodiscard]] inline std::string_view Before(std::string_view s, std::string_view delim) {
    size_t pos = s.find(delim);
    if (pos == std::string_view::npos) return s;
    return s.substr(0, pos);
}

[[nodiscard]] inline std::string_view After(std::string_view s, std::string_view delim) {
    size_t pos = s.find(delim);
    if (pos == std::string_view::npos) return {};
    return s.substr(pos + delim.size());
}

[[nodiscard]] inline std::string_view BeforeLast(std::string_view s, std::string_view delim) {
    size_t pos = s.rfind(delim);
    if (pos == std::string_view::npos) return s;
    return s.substr(0, pos);
}

[[nodiscard]] inline std::string_view AfterLast(std::string_view s, std::string_view delim) {
    size_t pos = s.rfind(delim);
    if (pos == std::string_view::npos) return {};
    return s.substr(pos + delim.size());
}

// Counting

[[nodiscard]] inline size_t Count(std::string_view s, std::string_view needle) {
    if (needle.empty()) return 0;
    size_t count = 0;
    size_t pos = 0;
    while ((pos = s.find(needle, pos)) != std::string_view::npos) {
        ++count;
        pos += needle.size();
    }
    return count;
}

[[nodiscard]] inline size_t CountChar(std::string_view s, char c) {
    return static_cast<size_t>(std::ranges::count(s, c));
}

// Numeric conversion (no exceptions, uses std::from_chars / to_chars)

template <typename T>
[[nodiscard]] inline std::optional<T> ToNumber(std::string_view s) {
    T value{};
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
    if (ec != std::errc{} || ptr != s.data() + s.size()) return std::nullopt;
    return value;
}

template <typename T>
[[nodiscard]] inline std::string FromNumber(T value) {
    return std::to_string(value);
}

// Encoding / escaping

[[nodiscard]] inline std::string ToHex(std::string_view s, bool uppercase = false) {
    std::string result;
    result.reserve(s.size() * 2);
    const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";
    for (unsigned char c : s) {
        result.push_back(digits[c >> 4]);
        result.push_back(digits[c & 0x0F]);
    }
    return result;
}

[[nodiscard]] inline std::optional<std::string> FromHex(std::string_view hex) {
    if (hex.size() % 2 != 0) return std::nullopt;
    std::string result;
    result.reserve(hex.size() / 2);
    for (size_t i = 0; i < hex.size(); i += 2) {
        uint8_t hi, lo;
        auto r1 = std::from_chars(hex.data() + i, hex.data() + i + 1, hi, 16);
        auto r2 = std::from_chars(hex.data() + i + 1, hex.data() + i + 2, lo, 16);
        if (r1.ec != std::errc{} || r2.ec != std::errc{}) return std::nullopt;
        result.push_back(static_cast<char>((hi << 4) | lo));
    }
    return result;
}

[[nodiscard]] inline std::string EscapeQuotes(std::string_view s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        if (c == '"' || c == '\\') result.push_back('\\');
        result.push_back(c);
    }
    return result;
}

// Minimal HTML entity escaping for < > & " '
[[nodiscard]] inline std::string EscapeHtml(std::string_view s) {
    std::string result;
    result.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '<': result.append("&lt;"); break;
            case '>': result.append("&gt;"); break;
            case '&': result.append("&amp;"); break;
            case '"': result.append("&quot;"); break;
            case '\'': result.append("&#39;"); break;
            default: result.push_back(c);
        }
    }
    return result;
}

// Distance / similarity

[[nodiscard]] inline size_t LevenshteinDistance(std::string_view a, std::string_view b) {
    const size_t n = a.size(), m = b.size();
    if (n == 0) return m;
    if (m == 0) return n;

    std::vector<size_t> prev(m + 1), curr(m + 1);
    for (size_t j = 0; j <= m; ++j) prev[j] = j;

    for (size_t i = 1; i <= n; ++i) {
        curr[0] = i;
        for (size_t j = 1; j <= m; ++j) {
            size_t cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            curr[j] = std::min({
                prev[j] + 1,
                curr[j - 1] + 1,
                prev[j - 1] + cost
            });
        }
        std::swap(prev, curr);
    }
    return prev[m];
}

// Similarity ratio in the range [0, 1], based on Levenshtein distance.
[[nodiscard]] inline double SimilarityRatio(std::string_view a, std::string_view b) {
    if (a.empty() && b.empty()) return 1.0;
    size_t maxLen = std::max(a.size(), b.size());
    if (maxLen == 0) return 1.0;
    size_t dist = LevenshteinDistance(a, b);
    return 1.0 - static_cast<double>(dist) / static_cast<double>(maxLen);
}

// Random / generation

[[nodiscard]] inline std::string RandomAlphaNumeric(size_t length) {
    static constexpr std::string_view charset =
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    static thread_local std::mt19937_64 rng{std::random_device{}()};
    std::uniform_int_distribution<size_t> dist(0, charset.size() - 1);

    std::string result;
    result.reserve(length);
    for (size_t i = 0; i < length; ++i) result.push_back(charset[dist(rng)]);
    return result;
}

// Wildcard matching (supports * and ?)

[[nodiscard]] inline bool WildcardMatch(std::string_view text, std::string_view pattern) {
    size_t t = 0, p = 0, starIdx = std::string_view::npos, matchIdx = 0;

    while (t < text.size()) {
        if (p < pattern.size() && (pattern[p] == '?' || pattern[p] == text[t])) {
            ++t; ++p;
        } else if (p < pattern.size() && pattern[p] == '*') {
            starIdx = p;
            matchIdx = t;
            ++p;
        } else if (starIdx != std::string_view::npos) {
            p = starIdx + 1;
            ++matchIdx;
            t = matchIdx;
        } else {
            return false;
        }
    }

    while (p < pattern.size() && pattern[p] == '*') ++p;
    return p == pattern.size();
}

} // namespace strutil
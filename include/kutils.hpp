#include <string>
#include <unordered_map>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <ctype.h>
#include <chrono>
#include <iostream>
#include <ostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <type_traits>

namespace constants {
static const char* SIMPLE_DATE_FORMAT{"%Y-%m-%dT%H:%M:%S"};
} // namespace constants

///////////////////////////////////////////////////////////////
template<typename... Args>
static void log(Args... args) { for (const auto& s : { args... }) std::cout << s; std::cout << std::endl; }

[[ maybe_unused ]]
static std::string DecodeHTML(const std::string& text)
{
  std::string                                  decoded{};
  std::unordered_map<std::string, std::string> convert({
    {"&quot;",  "\""},
    {"&apos;",  "'"},
    {"&amp;",   "&"},
    {"&gt;",    ">"},
    {"&lt;",    "<"},
    {"&frasl;", "/"}});

  for (size_t i = 0; i < text.size(); ++i)
  {
    bool flag = false;
    for (const auto& [key, value] : convert)
    {
      if (i + key.size() - 1 < text.size())
      {
        if (text.substr(i, key.size()) == key)
        {
          decoded += value;
          i += key.size() - 1;
          flag = true;
          break;
        }
      }
    }

  if (!flag)
    decoded += text[i];

  }

  return decoded;
}

[[ maybe_unused ]]
static std::string FloatToDecimalString(float n)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << n;
  return ss.str();
}

[[ maybe_unused ]]
static std::string ToLower(const std::string& s)
{
  std::string t{s};
  std::transform(t.begin(), t.end(), t.begin(), [](char c) { return tolower(c); });
  return t;
}
struct MimeType
{
  std::string name;
  bool        video;

  bool IsVideo() const { return video;    }
  bool IsPhoto() const { return !(video); }
};

[[ maybe_unused ]]
static MimeType GetMimeType(const std::string& path)
{
  const auto it = path.find_last_of('.');
  if (it != std::string::npos)
  {
    const auto extension = ToLower(path.substr(it + 1));

    if (extension == "jpg" || extension == "jpeg")
      return MimeType{"image/jpeg", false};
    else
    if (extension == "png")
      return MimeType{"image/png", false};
    else
    if (extension == "gif")
      return MimeType{"image/gif", false};
    else
    if (extension == "mp4")
      return MimeType{"video/mp4", true};
    else
    if (extension == "mkv")
      return MimeType{"video/mkv", true};
    else
    if (extension == "webm")
      return MimeType{"video/webm", true};
    else
    if (extension == "mpeg")
      return MimeType{"video/mpeg", true};
    else
    if (extension == "mov")
      return MimeType{"video/quicktime", true};
  }
  return MimeType{"unknown", false};
}

[[ maybe_unused ]]
static std::string GetExecutableCWD()
{
  std::string full_path{realpath("/proc/self/exe", NULL)};
  return full_path.substr(0, full_path.size() - (13));
}



/**
 * SanitizeJSON
 *
 * Helper function to remove escaped double quotes from a string
 *
 * @param [in] {std::string&} A reference to a string object
 */
inline std::string StripLineBreaks(std::string s) {
  s.erase(
    std::remove(s.begin(), s.end(),'\n'),
    s.end()
  );

  return s;
}

/**
 * SanitizeInput
 *
 * Helper function to remove quotes from a string
 *
 * @param   [in] {std::string}
 * @returns [in] {std::string}
 */
inline std::string SanitizeInput(std::string s) {
  s.erase(
    std::remove_if(s.begin(), s.end(), [](char c){
      return c == '\'' || c == '\"';
    }),
  s.end());

  return s;
}


[[ maybe_unused ]]
static bool IsAllNum(const std::string& s)
{
  for (const char& c : s)
    if (!std::isdigit(c)) return false;
  return true;
}

static std::string AlphaNumericOnly(std::string s)
{
  s.erase(std::remove_if(
    s.begin(), s.end(),
    [](char c)
    {
      return !isalnum(c);
    }),
    s.end()
  );

  return s;
}

/**
 * SaveToFile
 */
inline void SaveToFile(std::string data, std::string path) {
  std::ofstream o{path};
  o << data;
}

std::string read_file(const std::string& path)
{
  std::ifstream fs{path};
  std::stringstream ss;
  ss << fs.rdbuf();
  return ss.str();
}

/**
 * to_unixtime
 *
 * @param
 * @returns
 */
inline const std::time_t to_unixtime(const char* datetime) {
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  return mktime(&t);
}

/**
 * to_readable_time
 *
 * @param
 * @returns
 */
inline std::string to_readable_time(const char* datetime) {
  uint8_t            buffer_size{24};
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[buffer_size];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, buffer_size, "%B %d %H:%M:%S", &t);

  return std::string{b};
}

/**
 * to_readable_time
 *
 * @param
 * @returns
 */
inline std::string to_readable_time(const std::string datetime) {
  uint8_t            buffer_size{24};
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[buffer_size];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, buffer_size, "%B %d %H:%M:%S", &t);

  return std::string{b};
}

inline std::string get_simple_datetime() {
  uint8_t            buffer_size{24};
  char               b[buffer_size];
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm{};
  if (::gmtime_r(&now, &tm))
    if (std::strftime(b, sizeof(b), constants::SIMPLE_DATE_FORMAT, &tm))
      return std::string{b};
  throw std::runtime_error{"Failed to get current date as string"};
}

// template <typename T = float>
inline std::string human_readable_duration(std::chrono::duration<int64_t> delta) {
  using namespace std;
  using namespace std::chrono;
  using days = duration<int, ratio<86400>>;

  std::stringstream ss{};

  char fill = ss.fill();
  ss.fill('0');
  auto d = duration_cast<days>(delta);
  delta -= d;
  auto h = duration_cast<hours>(delta);
  delta -= h;
  auto m = duration_cast<minutes>(delta);
  delta -= m;
  auto s = duration_cast<seconds>(delta);

  ss  << setw(2) << d.count() << "d:"
      << setw(2) << h.count() << "h:"
      << setw(2) << m.count() << "m:"
      << setw(2) << s.count() << 's';

  ss.fill(fill);

  return ss.str();
};


// template <typename T = float>
inline std::string delta_to_string(std::chrono::duration<int64_t, std::nano> d) {
  return human_readable_duration(std::chrono::duration_cast<std::chrono::seconds>(d));
}

// template <typename T = float>
inline std::chrono::duration<int64_t, std::nano> get_datetime_delta(std::string dt1, std::string dt2) {
  std::tm            t{};
  std::istringstream ss{dt1};
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  std::chrono::time_point tp_1 = std::chrono::system_clock::from_time_t(mktime(&t));

  ss.clear();

  ss.str(dt2);
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  std::chrono::time_point tp_2 = std::chrono::system_clock::from_time_t(mktime(&t));

  std::chrono::duration<int64_t, std::nano> elapsed = tp_1 - tp_2;
  return elapsed;
}

// template <typename T = float>
inline std::string datetime_delta_string(std::string dt1, std::string dt2) {
  std::chrono::duration<int64_t, std::nano> datetime_delta = get_datetime_delta(dt1, dt2);
  return delta_to_string(datetime_delta);
}

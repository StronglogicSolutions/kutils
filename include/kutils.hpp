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

namespace kutils {
namespace constants {
static const char* SIMPLE_DATE_FORMAT{"%Y-%m-%dT%H:%M:%S"};
} // namespace constants
///////////////////////////////////////////////////////////////
template <typename T>
constexpr bool is_string_type = std::is_convertible_v<T, std::string_view>;
template<typename... Args>
static void log(Args... args)
{
  if (!sizeof ...(args))
    return;
  for (const auto& s : { args... })
  {
    using T = decltype(std::decay_t(s));
    if constexpr (is_string_type<T>)
      std::cout << s << std::endl;
  }
}
////////////////////////////////////////////////
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

    if (!flag) decoded += text[i];
  }

  return decoded;
}
////////////////////////////////////////////////
[[ maybe_unused ]]
static std::string FloatToDecimalString(float n)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << n;
  return ss.str();
}
////////////////////////////////////////////////
[[ maybe_unused ]]
static std::string ToLower(const std::string& s)
{
  std::string t{s};
  std::transform(t.begin(), t.end(), t.begin(), [](char c) { return tolower(c); });
  return t;
}
////////////////////////////////////////////////
struct MimeType
{
  std::string name;
  bool        video;

  bool IsVideo() const { return video;    }
  bool IsPhoto() const { return !(video); }
};
////////////////////////////////////////////////

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
////////////////////////////////////////////////
[[ maybe_unused ]]
static std::string GetCWD(size_t name_length)
{
  std::string full_path{realpath("/proc/self/exe", NULL)};
  return full_path.substr(0, full_path.size() - (name_length));
}
////////////////////////////////////////////////
static std::string StripBreaks(std::string s)
{
  s.erase(std::remove(s.begin(), s.end(),'\n'), s.end());
  return s;
}
////////////////////////////////////////////////
static std::string SanitizeInput(std::string s)
{
  s.erase(std::remove_if(s.begin(), s.end(), [](char c){ return c == '\'' || c == '\"'; }), s.end());
  return s;
}
////////////////////////////////////////////////
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
////////////////////////////////////////////////
[[ maybe_unused ]]
static std::string ParseFilename(const std::string& full_url)
{
  auto FindProt  = []         (const auto& s) { auto i = s.find_last_of("://"); return (i != s.npos) ? i + 1 : 0; };
  auto FindQuery = []         (const auto& s) { auto i = s.find_first_of('?');  return (i != s.npos) ? i : 0;     };
  auto FindExt   = []         (const auto& s) { auto i = s.find_last_of('.');   return (i != s.npos) ? i : 0;     };
  auto SimpleURL = [FindQuery](const auto& s) {                                 return s.substr(0, FindQuery(s)); };
  auto Filename  = [SimpleURL, FindProt, FindExt](const auto& full_url)
  {
    const auto url       = SimpleURL(full_url);
    const auto uri       = url.substr(FindProt(url));
    const auto ext       = FindExt(uri);
    const auto sub_uri   = (ext) ? uri.substr(0, ext) : uri;
    const auto extension = uri.substr(ext);
    const auto sub_ext   = FindExt(sub_uri);
    return (sub_ext) ? sub_uri.substr(sub_ext) + extension : sub_uri + extension;
  };

  return Filename(full_url);
}
////////////////////////////////////////////////
static void SaveToFile(std::string data, std::string path)
{
  std::ofstream o{path};
  o << data;
}
////////////////////////////////////////////////
std::string ReadFile(const std::string& path)
{
  std::ifstream fs{path};
  std::stringstream ss;
  ss << fs.rdbuf();
  return ss.str();
}
////////////////////////////////////////////////
static const std::time_t to_unixtime(const char* datetime)
{
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  return mktime(&t);
}
////////////////////////////////////////////////
static std::string to_readable_time(const char* datetime)
{
  uint8_t            buffer_size{24};
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[buffer_size];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, buffer_size, "%B %d %H:%M:%S", &t);

  return std::string{b};
}
////////////////////////////////////////////////
static std::string to_readable_time(const std::string datetime)
{
  uint8_t            buffer_size{24};
  std::tm            t{};
  std::istringstream ss{datetime};
  char               b[buffer_size];
  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  strftime(b, buffer_size, "%B %d %H:%M:%S", &t);

  return std::string{b};
}
////////////////////////////////////////////////
static std::string get_simple_datetime()
{
  uint8_t            buffer_size{24};
  char               b[buffer_size];
  auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  struct tm tm{};
  if (::gmtime_r(&now, &tm))
    if (std::strftime(b, sizeof(b), constants::SIMPLE_DATE_FORMAT, &tm))
      return std::string{b};
  throw std::runtime_error{"Failed to get current date as string"};
}
////////////////////////////////////////////////
static int32_t get_unixtime()
{
  return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
////////////////////////////////////////////////
static std::string human_readable_duration(std::chrono::duration<int64_t> delta)
{
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
////////////////////////////////////////////////
static std::string delta_to_string(std::chrono::duration<int64_t, std::nano> d)
{
  return human_readable_duration(std::chrono::duration_cast<std::chrono::seconds>(d));
}
////////////////////////////////////////////////
static std::chrono::duration<int64_t, std::nano> get_datetime_delta(std::string dt1, std::string dt2)
{
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
////////////////////////////////////////////////
static std::string datetime_delta_string(std::string dt1, std::string dt2)
{
  std::chrono::duration<int64_t, std::nano> datetime_delta = get_datetime_delta(dt1, dt2);
  return delta_to_string(datetime_delta);
}
} // ns kutils


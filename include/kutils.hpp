#pragma once

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
#include <thread>
#include <filesystem>

namespace kutils {
namespace constants {
static const char* SIMPLE_DATE_FORMAT{"%Y-%m-%dT%H:%M:%S"};
} // namespace constants
///////////////////////////////////////////////////////////////
template <typename T>
constexpr bool is_string_type = std::is_convertible_v<T, std::string_view>;
template<typename... Args>
inline void log(Args... args)
{
  if (!sizeof ...(args))
    return;

  for (const auto& s : { args... })
  {
    using T = std::decay_t<decltype(s)>;
    if constexpr (is_string_type<T>)
      std::cout << s;
  }
  std::cout << std::endl;
}
////////////////////////////////////////////////
[[ maybe_unused ]]
inline std::string DecodeHTML(const std::string& text)
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
inline std::string FloatToDecimalString(float n)
{
  std::stringstream ss;
  ss << std::fixed << std::setprecision(2) << n;
  return ss.str();
}
////////////////////////////////////////////////
[[ maybe_unused ]]
inline std::string ToLower(const std::string& s)
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
inline MimeType GetMimeType(const std::string& path)
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
inline std::string GetCWD(size_t name_length)
{
  std::string full_path{realpath("/proc/self/exe", NULL)};
  return full_path.substr(0, full_path.size() - (name_length));
}
////////////////////////////////////////////////
inline std::string StripBreaks(std::string s)
{
  s.erase(std::remove(s.begin(), s.end(),'\n'), s.end());
  return s;
}
////////////////////////////////////////////////
inline std::string SanitizeInput(std::string s)
{
  s.erase(std::remove_if(s.begin(), s.end(), [](char c){ return c == '\'' || c == '\"'; }), s.end());
  return s;
}
////////////////////////////////////////////////
[[ maybe_unused ]]
inline bool IsAllNum(const std::string& s)
{
  for (const char& c : s)
    if (!std::isdigit(c)) return false;
  return true;
}
inline std::string AlphaNumericOnly(std::string s)
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
inline std::string ParseFilename(const std::string& full_url)
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
inline bool create_dir(const std::string& path)
{
  namespace fs = std::filesystem;

  try
  {
    if (!fs::exists(path))
      fs::create_directory(path);
    return true;
  }
  catch (const std::exception& e)
  {
    return false;
  }
}
////////////////////////////////////////////////
inline void SaveToFile(std::string data, std::string path)
{
  std::ofstream o{path};
  o << data;
}
////////////////////////////////////////////////
inline std::string ReadFile(const std::string& path)
{
  std::ifstream fs{path};
  std::stringstream ss;
  ss << fs.rdbuf();
  return ss.str();
}
////////////////////////////////////////////////
inline std::time_t to_unixtime(const char* datetime)
{
  std::tm            t{};
  std::istringstream ss{datetime};

  ss >> std::get_time(&t, "%Y-%m-%dT%H:%M:%S");

  return mktime(&t);
}
////////////////////////////////////////////////
inline std::string to_readable_time(const char* datetime)
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
inline std::string to_readable_time(const std::string datetime)
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
inline std::string get_simple_datetime()
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
inline int32_t get_unixtime()
{
  return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
////////////////////////////////////////////////
inline std::string get_unix_tstring()
{
  return std::to_string(get_unixtime());
}
////////////////////////////////////////////////
[[ maybe_unused ]]
static std::string from_unixtime(time_t unix_timestamp)
{
  char time_buf[80];
  struct tm ts;
  ts = *localtime(&unix_timestamp);
  strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
  return time_buf;
}
////////////////////////////////////////////////
inline std::string human_readable_duration(std::chrono::duration<int64_t> delta)
{
  using days = std::chrono::duration<int, std::ratio<86400>>;

  std::stringstream ss{};

  char fill = ss.fill();
  ss.fill('0');
  auto d = std::chrono::duration_cast<days>(delta);
  delta -= d;
  auto h = std::chrono::duration_cast<std::chrono::hours>(delta);
  delta -= h;
  auto m = std::chrono::duration_cast<std::chrono::minutes>(delta);
  delta -= m;
  auto s = std::chrono::duration_cast<std::chrono::seconds>(delta);

  ss  << std::setw(2) << d.count() << "d:"
      << std::setw(2) << h.count() << "h:"
      << std::setw(2) << m.count() << "m:"
      << std::setw(2) << s.count() << 's';

  ss.fill(fill);

  return ss.str();
};
////////////////////////////////////////////////
inline std::string delta_to_string(std::chrono::duration<int64_t, std::nano> d)
{
  return human_readable_duration(std::chrono::duration_cast<std::chrono::seconds>(d));
}
////////////////////////////////////////////////
inline std::chrono::duration<int64_t, std::nano> get_datetime_delta(std::string dt1, std::string dt2)
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
inline std::string datetime_delta_string(std::string dt1, std::string dt2)
{
  std::chrono::duration<int64_t, std::nano> datetime_delta = get_datetime_delta(dt1, dt2);
  return delta_to_string(datetime_delta);
}
////////////////////////////////////////////////
inline std::string generate_random_chars()
{
  srand(get_unixtime());
  std::string r;
  for (int n = rand(), t = n; t > 0;)
  {
    r += static_cast<char>(t % 10);
    t /= 10;
  }
  return r;
}
////////////////////////////////////////////////
inline std::string UnescapeQuotes(std::string s)
{
  s.erase(std::remove(s.begin(), s.end(),'\"'), s.end());
  return s;
}
////////////////////////////////////////////////
inline std::vector<std::string> urls_from_string(std::string input_string, char delim = '>')
{
  std::string              s = UnescapeQuotes(input_string);
  std::vector<std::string> urls;

  if (!s.empty())
  {
    auto pos = s.find_first_of(delim);
    while (pos != s.npos)
    {
      urls.emplace_back(s.substr(0, pos));
      s   = s.substr(pos + 1);
      pos = s.find_first_of(delim);
    }

    urls.emplace_back(s);
  }
  return urls;
}
////////////////////////////////////////////////
inline void make_event(std::function<void()> f, int ms = 5000)
{
  std::thread{[f, ms]
  {
    if (ms)
      std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    f();
  }}.detach();
}
////////////////////////////////////////////////
class kargs
{
  using argmap_t = std::unordered_map<std::string, std::string>;
public:
  kargs(int argc, char* argv[])
  {
    for (int i = 1; i < argc; i++)
    {
      std::string arg = argv[i];
      if (it_ = arg.find('='); it_ != std::string::npos)
        args_[arg.front() == '-' ? arg.substr(2, it_ - 2) : arg.substr(0, it_)] = arg.substr(it_ + 1);
    }
  }

  std::string get(const char* key)
  {
    if (const auto it = args_.find(key); it != args_.end())
      return it->second;
    return "";
  }

private:
  argmap_t args_;
  size_t   it_{0};
};
//-----------------------------------------------------------------------
template <int64_t INTERVAL = 3000>
class timer
{
using time_point_t = std::chrono::time_point<std::chrono::system_clock>;
using ms_t         = std::chrono::milliseconds;
using duration_t   = std::chrono::duration<std::chrono::system_clock, ms_t>;

public:
  bool
  check_and_update()
  {
    if (const auto tp = now(); ready(tp))
    {
      _last = tp;
      return true;
    }
    return false;
  }
//-----------------------------------------------------------------------
  void
  stop()
  {
    _last = now() - std::chrono::milliseconds(INTERVAL);
  }
  //-----------------------------------------------------------------------
  void
  reset()
  {
    _last = now();
  }

private:
  bool
  ready(const time_point_t t) const
  {
    return (std::chrono::duration_cast<ms_t>(t - _last).count() > INTERVAL);
  }
//-----------------------------------------------------------------------
  time_point_t
  now()
  {
    return std::chrono::system_clock::now();
  }
//-----------------------------------------------------------------------
  time_point_t _last{now()};
};
//----------------------------------------------------------------------------------------------
template <int32_t MAX = 4, int32_t SECONDS = 60>
class bucket
{
using clock_t    = std::chrono::steady_clock;
using duration_t = clock_t::duration;
static constexpr auto g_max = clock_t::duration{std::chrono::seconds(SECONDS)};
//------------------------------------
public:
  bool request(int quantity)
  {
    refill();

    const auto required = (quantity * (rate_));
    const auto result   = required <= available_;

    if (result)
      available_ -= required;

    return result;
  }

  bool has_token() const
  {
    return (available_ >= rate_);
  }
//------------------------------------
private:
  void refill()
  {
    const auto now  = clock_t::now();
    available_     += now - last_refill_;
    available_      = std::min(available_, g_max);
    last_refill_    = now;
  }
//------------------------------------
  clock_t::duration   available_  {g_max};
  clock_t::duration   rate_       {g_max / MAX};
  clock_t::time_point last_refill_{clock_t::now()};
};
} // ns kutils

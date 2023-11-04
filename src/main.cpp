#include "../include/kutils.hpp"

int main(int argc, char* argv[])
{
  kutils::kargs args{argc, argv};
  kutils::log("loglevel is: ", args.get("loglevel").c_str());
  kutils::bucket<4, 60> bucket;

  return 0;
}


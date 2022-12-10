// Unit test infrastructure.
// @PENGUINLIONG
#include <map>
#include <memory>
#include <string>
#include <functional>

namespace liong {

namespace test {

struct TestReport {
  uint64_t nsucc;
  uint64_t nfail;
};

struct TestRegistry {
  struct Entry {
    std::function<void()> f;
  };
  std::map<std::string, Entry> tests;

  TestRegistry();

  static TestRegistry& get_inst();
  static TestReport run_all();

  int reg(const std::string& name, std::function<void()>&& func);
};

} // namespace test

} // namespace liong

#define L_TEST(name) \
  extern void l_test_##name();\
  int L_TEST_MARKER_##name = ::liong::test::TestRegistry::get_inst().reg(#name, l_test_##name); \
  void l_test_##name()

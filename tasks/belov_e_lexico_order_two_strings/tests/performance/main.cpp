#include <gtest/gtest.h>

#include <algorithm>
#include <ctime>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "belov_e_lexico_order_two_strings/common/include/common.hpp"
#include "belov_e_lexico_order_two_strings/mpi/include/ops_mpi.hpp"
#include "belov_e_lexico_order_two_strings/seq/include/ops_seq.hpp"
#include "util/include/perf_test_util.hpp"

namespace belov_e_lexico_order_two_strings {

class BelovERunPerfTestsProcesses : public ppc::util::BaseRunPerfTests<InType, OutType> {
  const size_t size_params_ = 1000;
  InType input_data_;
  OutType output_data_{};

  void SetUp() override {
    std::tuple<std::string, std::string, int> generated = GenerateInputData(size_params_);
    input_data_ = std::make_tuple(std::get<0>(generated), std::get<1>(generated));
    output_data_ = static_cast<bool>(std::get<2>(generated));
  }

  bool CheckTestOutputData(OutType &output_data) final {
    return output_data_ == output_data;
  }

  InType GetTestInputData() final {
    return input_data_;
  }

  static std::string GenerateWord(size_t size) {
    static const std::string kAlphabet = "abcdefghijklmnopqrstuvwxyz";
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> len_dist(static_cast<int>(size) / 4, static_cast<int>(size) / 2);
    std::uniform_int_distribution<int> char_dist(0, static_cast<int>(kAlphabet.size()) - 1);

    int len = len_dist(gen);
    std::string word;
    for (int i = 0; i < len; ++i) {
      word += kAlphabet[char_dist(gen)];
    }

    return word;
  }
  static std::tuple<std::vector<std::string>, std::vector<std::string>> GenerateTwoSortedSentence(size_t size) {
    std::vector<std::string> vec;
    vec.reserve(size * 2);
    for (size_t i = 0; i < size * 2; i++) {
      vec.push_back(GenerateWord(size));
    }
    std::ranges::sort(vec);
    std::vector<std::string> first;
    std::vector<std::string> second;
    first.reserve(size);
    second.reserve(size);

    for (size_t i = 0; i < size; i++) {
      first.push_back(vec[i]);
    }
    for (size_t i = size; i < size * 2; i++) {
      second.push_back(vec[i]);
    }
    return std::make_tuple(first, second);
  }
  static std::string VectorOfWordsToString(const std::vector<std::string> &sentence) {
    std::string temp;
    for (size_t i = 0; i < sentence.size() - 1; i++) {
      temp += sentence[i] + " ";
    }
    temp += sentence.back();
    return temp;
  }

  static std::tuple<std::string, std::string, int> GenerateInputData(size_t size) {
    std::tuple<std::vector<std::string>, std::vector<std::string>> ans = GenerateTwoSortedSentence(size);
    return std::make_tuple(VectorOfWordsToString(std::get<0>(ans)), VectorOfWordsToString(std::get<1>(ans)), 1);
  }
};

TEST_P(BelovERunPerfTestsProcesses, RunPerfModes) {
  ExecuteTest(GetParam());
}

const auto kAllPerfTasks =
    ppc::util::MakeAllPerfTasks<InType, BelovELexicoOrderTwoStringsMPI, BelovELexicoOrderTwoStringsSEQ>(
        PPC_SETTINGS_belov_e_lexico_order_two_strings);

const auto kGtestValues = ppc::util::TupleToGTestValues(kAllPerfTasks);

const auto kPerfTestName = BelovERunPerfTestsProcesses::CustomPerfTestName;

INSTANTIATE_TEST_SUITE_P(RunModeTests, BelovERunPerfTestsProcesses, kGtestValues, kPerfTestName);

}  // namespace belov_e_lexico_order_two_strings

#include "belov_e_lexico_order_two_strings/seq/include/ops_seq.hpp"

#include "belov_e_lexico_order_two_strings/common/include/common.hpp"
#include "task/include/task.hpp"

namespace belov_e_lexico_order_two_strings {

BelovELexicoOrderTwoStringsSEQ::BelovELexicoOrderTwoStringsSEQ(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = 0;
}

bool BelovELexicoOrderTwoStringsSEQ::ValidationImpl() {
  return std::get<0>(GetInput()).size() > 0 && std::get<1>(GetInput()).size() > 0;
}
bool BelovELexicoOrderTwoStringsSEQ::PreProcessingImpl() {
  std::vector<std::string> temp;
  std::string current = "";
  for (size_t i = 0; i < std::get<0>(GetInput()).size(); i++) {
    if (std::get<0>(GetInput())[i] == ' ') {
      temp.push_back(current);
      current = "";
    } else {
      current += std::get<0>(GetInput())[i];
    }
  }
  if (!current.empty()) {
    temp.push_back(current);
  }
  std::get<0>(GetProccesedInput()) = temp;

  std::vector<std::string>().swap(temp);
  current = "";
  for (size_t i = 0; i < std::get<1>(GetInput()).size(); i++) {
    if (std::get<1>(GetInput())[i] == ' ') {
      temp.push_back(current);
      current = "";
    } else {
      current += std::get<1>(GetInput())[i];
    }
  }
  if (!current.empty()) {
    temp.push_back(current);
  }
  std::get<1>(GetProccesedInput()) = temp;
  return std::get<0>(GetProccesedInput()).size() > 0 && std::get<1>(GetProccesedInput()).size() > 0;
}

bool BelovELexicoOrderTwoStringsSEQ::RunImpl() {
  const std::vector<std::string> &first = std::get<0>(GetProccesedInput());
  const std::vector<std::string> &second = std::get<1>(GetProccesedInput());

  for (size_t i = 0; i < first.size() - 1; i++) {
    if (first[i] > first[i + 1]) {
      GetOutput() = false;
      return true;
    }
  }

  for (size_t i = 0; i < second.size() - 1; i++) {
    if (second[i] > second[i + 1]) {
      GetOutput() = false;
      return true;
    }
  }

  GetOutput() = first.back() <= second.front();

  return true;
}

bool BelovELexicoOrderTwoStringsSEQ::PostProcessingImpl() {
  return true;
}
}  // namespace belov_e_lexico_order_two_strings

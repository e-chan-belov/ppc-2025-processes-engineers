#include "belov_e_lexico_order_two_strings/mpi/include/ops_mpi.hpp"

#include <mpi.h>
#include <vector>
#include <string>
#include <algorithm>
#include <cstddef>

#include "belov_e_lexico_order_two_strings/common/include/common.hpp"

namespace belov_e_lexico_order_two_strings {
BelovELexicoOrderTwoStringsMPI::BelovELexicoOrderTwoStringsMPI(const InType &in) {
  SetTypeOfTask(GetStaticTypeOfTask());
  GetInput() = in;
  GetOutput() = false;
}

bool BelovELexicoOrderTwoStringsMPI::ValidationImpl() {
  return !std::get<0>(GetInput()).empty() && !std::get<1>(GetInput()).empty();
}

bool BelovELexicoOrderTwoStringsMPI::PreProcessingImpl() {
  std::vector<std::string> temp;
  std::string current;
  for (auto& ch : std::get<0>(GetInput())) {
    if (ch == ' ') {
      temp.push_back(current);
      current = "";
    } else {
      current += ch;
    }
  }
  if (!current.empty()) {
    temp.push_back(current);
  }
  std::get<0>(GetProccesedInput()) = temp;

  std::vector<std::string>().swap(temp);
  current = "";
  for (auto& ch : std::get<1>(GetInput())) {
    if (ch == ' ') {
      temp.push_back(current);
      current = "";
    } else {
      current += ch;
    }
  }
  if (!current.empty()) {
    temp.push_back(current);
  }
  std::get<1>(GetProccesedInput()) = temp;

  return !std::get<0>(GetProccesedInput()).empty() && !std::get<1>(GetProccesedInput()).empty();
}

bool IsPartSorted(const std::vector<std::string> &words, int begin, int end) {
  if (end <= begin) {
    return true;
  }
  for (size_t i = begin; i < static_cast<size_t>(end) - 1; i++) {
    if (words[i] > words[i + 1]) {
      return false;
    }
  }
  return true;
}

int CeilDiv(int a, int b) {
  return (a + b - 1) / b;
}

bool BelovELexicoOrderTwoStringsMPI::RunImpl() {
  int mpi_size = 0;
  int rank = 0;

  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  const std::vector<std::string> &first = std::get<0>(GetProccesedInput());
  const std::vector<std::string> &second = std::get<1>(GetProccesedInput());

  int n1 = static_cast<int>(first.size());
  int n2 = static_cast<int>(second.size());

  bool local_ans = true;

  if (rank < mpi_size / 2) {
    int chunk = CeilDiv(n1, mpi_size / 2);
    int begin = rank * chunk;
    int end = std::min(begin + chunk + 1, n1);
    local_ans = IsPartSorted(first, begin, end);
  } else {
    int chunk = CeilDiv(n2, (mpi_size - (mpi_size / 2)));
    int begin = (rank - (mpi_size / 2)) * chunk;
    int end = std::min(begin + chunk + 1, n2);
    local_ans = IsPartSorted(second, begin, end);
  }

  bool ans = false;
  MPI_Allreduce(&local_ans, &ans, 1, MPI_C_BOOL, MPI_LAND, MPI_COMM_WORLD);
  GetOutput() = ((first.back() <= second.front()) && ans);
  MPI_Barrier(MPI_COMM_WORLD);
  return true;
}

bool BelovELexicoOrderTwoStringsMPI::PostProcessingImpl() {
  return true;
}
}  // namespace belov_e_lexico_order_two_strings

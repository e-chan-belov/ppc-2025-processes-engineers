#pragma once

#include "belov_e_bubble_sort/common/include/common.hpp"
#include "task/include/task.hpp"

namespace belov_e_bubble_sort {
class BelovEBubbleSortMPI : public BaseTask {
 public:
  static constexpr ppc::task::TypeOfTask GetStaticTypeOfTask() {
    return ppc::task::TypeOfTask::kMPI;
  }
  explicit BelovEBubbleSortMPI(const InType &in);

  bool ValidationImpl() override;
  bool PreProcessingImpl() override;
  bool RunImpl() override;
  bool PostProcessingImpl() override;
};
int CeilDiv(int a, int b);
void BubbleSort(std::vector<int> &arr);
std::vector<int> LeftMerge(const std::vector<int> &left, const std::vector<int> &right);
std::vector<int> RightMerge(const std::vector<int> &left, const std::vector<int> &right);
}  // namespace belov_e_bubble_sort

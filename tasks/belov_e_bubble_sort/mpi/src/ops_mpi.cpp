#include "belov_e_bubble_sort/mpi/include/ops_mpi.hpp"

#include <vector>
#include <utility>
#include <algorithm>
#include <iterator>
#include <mpi.h>

#include "belov_e_bubble_sort/common/include/common.hpp"

namespace belov_e_bubble_sort {

BelovEBubbleSortMPI::BelovEBubbleSortMPI(const InType &in) {
    SetTypeOfTask(GetStaticTypeOfTask());
    GetInput() = in;
    std::vector<int> temp;
    temp.resize(GetInput().size());
    GetOutput() = temp;
}

bool BelovEBubbleSortMPI::ValidationImpl() {
    return !GetInput().empty();
}

bool BelovEBubbleSortMPI::PreProcessingImpl() {
    return true;
}

int CeilDiv(int a, int b) {
    return (a + b - 1) / b;
}

void BubbleSort(std::vector<int>& arr) {
    const std::size_t n = arr.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = 0; j + 1 < n - i; ++j) {
            if (arr[j] > arr[j + 1]) {
                std::swap(arr[j], arr[j + 1]);
            }
        }
    }
}

std::vector<int> LeftMerge(const std::vector<int>& left, const std::vector<int>& right) {
    std::vector<int> result;
    result.reserve(left.size() + right.size());

    std::merge(
        left.begin(), left.end(),
        right.begin(), right.end(),
        std::back_inserter(result)
    );

    return std::vector<int>(
        result.begin(),
        result.begin() + left.size()
    );

}
std::vector<int> RightMerge(const std::vector<int>& left, const std::vector<int>& right) {
    std::vector<int> result;
    result.reserve(left.size() + right.size());

    std::merge(
        left.begin(), left.end(),
        right.begin(), right.end(),
        std::back_inserter(result)
    );

    return std::vector<int>(
        result.end() - right.size(),
        result.end()
    );
}

bool BelovEBubbleSortMPI::RunImpl() {
    int mpi_size = 0;
    int rank = 0;
  
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    std::vector<int> arr;
    int n = 0;

    if (rank == 0) {
        arr = GetInput();
        n = static_cast<int>(arr.size());
    }
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_arr_size = 0;
    std::vector<int> local_arr;
    int base = n / mpi_size;
    int rem  = n % mpi_size;
    local_arr_size = base + (rank < rem ? 1 : 0);
    local_arr.resize(local_arr_size);

    std::vector<int> arrays_sizes;
    arrays_sizes.resize(mpi_size);
    MPI_Allgather(&local_arr_size, 1, MPI_INT,
        arrays_sizes.data(), 1, MPI_INT, MPI_COMM_WORLD);

    std::vector<int> displs;
    displs.resize(mpi_size);
    displs[0] = 0;
    for (int i = 1; i < mpi_size; i++) {
        displs[i] = displs[i - 1] + arrays_sizes[i - 1];
    }

    MPI_Scatterv(
        arr.data(), arrays_sizes.data(), displs.data(), MPI_INT,
        local_arr.data(), local_arr_size, MPI_INT,
        0, MPI_COMM_WORLD
    );

    BubbleSort(local_arr);
    
    for (int i = 0; i < mpi_size + 1; i++) {
        if (i % 2 == 0) {
            if (mpi_size % 2 != 0 && rank == mpi_size - 1) { continue; }
            if (rank % 2 == 0) {
                std::vector<int> right_arr;
                right_arr.resize(arrays_sizes[rank + 1]);
                MPI_Sendrecv(
                    local_arr.data(), local_arr_size, MPI_INT, rank + 1, 0,
                    right_arr.data(), arrays_sizes[rank + 1], MPI_INT, rank + 1, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                local_arr = LeftMerge(local_arr, right_arr);
            }
            else {
                std::vector<int> left_arr;
                left_arr.resize(arrays_sizes[rank - 1]);
                MPI_Sendrecv(
                    local_arr.data(), local_arr_size, MPI_INT, rank - 1, 0,
                    left_arr.data(), arrays_sizes[rank - 1], MPI_INT, rank - 1, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                local_arr = RightMerge(left_arr, local_arr);
            }
        }
        else {
            if (rank == 0) { continue; }
            if (mpi_size % 2 == 0 && rank == mpi_size - 1) { continue; }
            if (rank % 2 == 0) {
                std::vector<int> left_arr;
                left_arr.resize(arrays_sizes[rank - 1]);
                MPI_Sendrecv(
                    local_arr.data(), local_arr_size, MPI_INT, rank - 1, 0,
                    left_arr.data(), arrays_sizes[rank - 1], MPI_INT, rank - 1, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                local_arr = RightMerge(left_arr, local_arr);
            }
            else {
                std::vector<int> right_arr;
                right_arr.resize(arrays_sizes[rank + 1]);
                MPI_Sendrecv(
                    local_arr.data(), local_arr_size, MPI_INT, rank + 1, 0,
                    right_arr.data(), arrays_sizes[rank + 1], MPI_INT, rank + 1, 0,
                    MPI_COMM_WORLD, MPI_STATUS_IGNORE
                );
                local_arr = LeftMerge(local_arr, right_arr);
            }
        }
    }

    arr.resize(n);
    MPI_Allgatherv(
        local_arr.data(), local_arr_size, MPI_INT,
        arr.data(), arrays_sizes.data(), displs.data(), MPI_INT,
        MPI_COMM_WORLD
    );
    GetOutput() = arr;
    return true;
}

bool BelovEBubbleSortMPI::PostProcessingImpl() {
    return true;
}

}  // namespace belov_e_bubble_sort
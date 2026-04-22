#pragma once

#include <map>
#include <vector>

namespace femsolver::kernel::algebra {

class SparseMatrix {
 public:
  SparseMatrix(int rows,
               int cols,
               std::vector<int> row_offsets,
               std::vector<int> col_indices,
               std::vector<double> values);

  [[nodiscard]] int rows() const;
  [[nodiscard]] int cols() const;
  [[nodiscard]] const std::vector<int>& row_offsets() const;
  [[nodiscard]] const std::vector<int>& col_indices() const;
  [[nodiscard]] const std::vector<double>& values() const;
  [[nodiscard]] double Entry(int row, int col) const;
  [[nodiscard]] std::vector<double> Multiply(const std::vector<double>& x) const;
  [[nodiscard]] std::vector<double> Diagonal() const;

 private:
  int rows_ = 0;
  int cols_ = 0;
  std::vector<int> row_offsets_;
  std::vector<int> col_indices_;
  std::vector<double> values_;
};

// Mutable row-wise sparse assembly structure that can still be edited before CSR finalization.
class SparseMatrixBuilder {
 public:
  SparseMatrixBuilder(int rows, int cols);

  [[nodiscard]] int rows() const;
  [[nodiscard]] int cols() const;
  void Add(int row, int col, double value);
  void Set(int row, int col, double value);
  void Erase(int row, int col);
  void ClearRow(int row);
  [[nodiscard]] bool HasEntry(int row, int col) const;
  [[nodiscard]] double Entry(int row, int col) const;
  [[nodiscard]] const std::map<int, double>& RowEntries(int row) const;
  [[nodiscard]] SparseMatrix Build() const;

 private:
  int rows_ = 0;
  int cols_ = 0;
  std::vector<std::map<int, double>> rows_data_;
};

struct LinearSystem {
  SparseMatrix matrix;
  std::vector<double> rhs;
};

}  // namespace femsolver::kernel::algebra

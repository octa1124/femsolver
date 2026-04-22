#include "femsolver/kernel/algebra/sparse_matrix.hpp"

#include <stdexcept>

namespace femsolver::kernel::algebra {

namespace {

void CheckRowColBounds(const int row, const int col, const int rows, const int cols) {
  if (row < 0 || row >= rows || col < 0 || col >= cols) {
    throw std::out_of_range("Sparse matrix index out of range");
  }
}

void CheckRowBounds(const int row, const int rows) {
  if (row < 0 || row >= rows) {
    throw std::out_of_range("Sparse matrix row out of range");
  }
}

}  // namespace

SparseMatrix::SparseMatrix(int rows,
                           int cols,
                           std::vector<int> row_offsets,
                           std::vector<int> col_indices,
                           std::vector<double> values)
    : rows_(rows),
      cols_(cols),
      row_offsets_(std::move(row_offsets)),
      col_indices_(std::move(col_indices)),
      values_(std::move(values)) {}

int SparseMatrix::rows() const {
  return rows_;
}

int SparseMatrix::cols() const {
  return cols_;
}

const std::vector<int>& SparseMatrix::row_offsets() const {
  return row_offsets_;
}

const std::vector<int>& SparseMatrix::col_indices() const {
  return col_indices_;
}

const std::vector<double>& SparseMatrix::values() const {
  return values_;
}

double SparseMatrix::Entry(const int row, const int col) const {
  CheckRowColBounds(row, col, rows_, cols_);
  for (int index = row_offsets_.at(static_cast<std::size_t>(row));
       index < row_offsets_.at(static_cast<std::size_t>(row + 1));
       ++index) {
    if (col_indices_.at(static_cast<std::size_t>(index)) == col) {
      return values_.at(static_cast<std::size_t>(index));
    }
  }
  return 0.0;
}

std::vector<double> SparseMatrix::Multiply(const std::vector<double>& x) const {
  if (static_cast<int>(x.size()) != cols_) {
    throw std::invalid_argument("Sparse matrix-vector multiply dimension mismatch");
  }

  std::vector<double> result(static_cast<std::size_t>(rows_), 0.0);
  for (int row = 0; row < rows_; ++row) {
    double row_sum = 0.0;
    for (int index = row_offsets_.at(static_cast<std::size_t>(row));
         index < row_offsets_.at(static_cast<std::size_t>(row + 1));
         ++index) {
      row_sum += values_.at(static_cast<std::size_t>(index)) *
                 x.at(static_cast<std::size_t>(col_indices_.at(static_cast<std::size_t>(index))));
    }
    result.at(static_cast<std::size_t>(row)) = row_sum;
  }
  return result;
}

std::vector<double> SparseMatrix::Diagonal() const {
  std::vector<double> diagonal(static_cast<std::size_t>(rows_), 0.0);
  for (int row = 0; row < rows_; ++row) {
    diagonal.at(static_cast<std::size_t>(row)) = Entry(row, row);
  }
  return diagonal;
}

SparseMatrixBuilder::SparseMatrixBuilder(const int rows, const int cols)
    : rows_(rows), cols_(cols), rows_data_(static_cast<std::size_t>(rows)) {}

int SparseMatrixBuilder::rows() const {
  return rows_;
}

int SparseMatrixBuilder::cols() const {
  return cols_;
}

void SparseMatrixBuilder::Add(const int row, const int col, const double value) {
  CheckRowColBounds(row, col, rows_, cols_);
  rows_data_.at(static_cast<std::size_t>(row))[col] += value;
}

void SparseMatrixBuilder::Set(const int row, const int col, const double value) {
  CheckRowColBounds(row, col, rows_, cols_);
  rows_data_.at(static_cast<std::size_t>(row))[col] = value;
}

void SparseMatrixBuilder::Erase(const int row, const int col) {
  CheckRowColBounds(row, col, rows_, cols_);
  rows_data_.at(static_cast<std::size_t>(row)).erase(col);
}

void SparseMatrixBuilder::ClearRow(const int row) {
  CheckRowBounds(row, rows_);
  rows_data_.at(static_cast<std::size_t>(row)).clear();
}

bool SparseMatrixBuilder::HasEntry(const int row, const int col) const {
  CheckRowColBounds(row, col, rows_, cols_);
  const auto& row_entries = rows_data_.at(static_cast<std::size_t>(row));
  return row_entries.find(col) != row_entries.end();
}

double SparseMatrixBuilder::Entry(const int row, const int col) const {
  CheckRowColBounds(row, col, rows_, cols_);
  const auto& row_entries = rows_data_.at(static_cast<std::size_t>(row));
  const auto iterator = row_entries.find(col);
  return iterator == row_entries.end() ? 0.0 : iterator->second;
}

const std::map<int, double>& SparseMatrixBuilder::RowEntries(const int row) const {
  CheckRowBounds(row, rows_);
  return rows_data_.at(static_cast<std::size_t>(row));
}

SparseMatrix SparseMatrixBuilder::Build() const {
  std::vector<int> row_offsets(static_cast<std::size_t>(rows_ + 1), 0);
  std::vector<int> col_indices;
  std::vector<double> values;

  int running_offset = 0;
  for (int row = 0; row < rows_; ++row) {
    row_offsets.at(static_cast<std::size_t>(row)) = running_offset;
    for (const auto& [col, value] : rows_data_.at(static_cast<std::size_t>(row))) {
      col_indices.push_back(col);
      values.push_back(value);
      ++running_offset;
    }
  }
  row_offsets.at(static_cast<std::size_t>(rows_)) = running_offset;

  return SparseMatrix(rows_, cols_, std::move(row_offsets), std::move(col_indices),
                      std::move(values));
}

}  // namespace femsolver::kernel::algebra

#include <gtest/gtest.h>

#include "femsolver/kernel/common/discretization_descriptor.hpp"

TEST(KernelDiscretizationDescriptorTest, SupportsTetraAndHexCellFamilies) {
  EXPECT_EQ(femsolver::kernel::common::ToString(
                femsolver::kernel::common::CellType::kTetrahedron),
            "tetrahedron");
  EXPECT_EQ(femsolver::kernel::common::ToString(
                femsolver::kernel::common::CellType::kHexahedron),
            "hexahedron");
}

TEST(KernelDiscretizationDescriptorTest, SupportsH1NedelecAndRaviartThomasFamilies) {
  EXPECT_EQ(femsolver::kernel::common::ToString(
                femsolver::kernel::common::SpaceFamily::kH1Nodal),
            "h1-nodal");
  EXPECT_EQ(femsolver::kernel::common::ToString(
                femsolver::kernel::common::SpaceFamily::kNedelec),
            "nedelec");
  EXPECT_EQ(femsolver::kernel::common::ToString(
                femsolver::kernel::common::SpaceFamily::kRaviartThomas),
            "raviart-thomas");
}

TEST(KernelDiscretizationDescriptorTest, DescriptorCapturesCellFamilySpaceFamilyAndOrder) {
  femsolver::kernel::common::ElementDescriptor descriptor{
      femsolver::kernel::common::CellType::kHexahedron,
      femsolver::kernel::common::SpaceFamily::kNedelec,
      {3}};

  EXPECT_TRUE(descriptor.IsValid());
  EXPECT_EQ(descriptor.Summary(), "hexahedron:nedelec:p3");

  descriptor.order.value = 0;
  EXPECT_FALSE(descriptor.IsValid());
}

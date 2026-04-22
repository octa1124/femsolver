#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#include "femsolver/io/text_report.hpp"

namespace {

constexpr std::string_view kUsage =
    "Usage: motor_pre [--help] --case <path> --variant <section_2d|extruded_3d|full_3d> "
    "[--output-dir <path>]\n"
    "Generate simplified joint-motor preprocessing artifacts from a case catalog entry.\n";

struct CliOptions {
  bool help = false;
  std::string case_path;
  std::string variant;
  std::string output_dir;
};

std::string ShellQuote(const std::string& value) {
  return "\"" + value + "\"";
}

std::filesystem::path FindRepositoryRoot(const std::filesystem::path& start) {
  auto current = std::filesystem::absolute(start);
  while (!current.empty()) {
    const auto script_path = current / "tools" / "gmsh" / "generate_joint_motor_case.py";
    if (std::filesystem::exists(script_path)) {
      return current;
    }

    if (current == current.root_path()) {
      break;
    }
    current = current.parent_path();
  }
  return {};
}

CliOptions ParseArguments(const int argc, char** argv) {
  CliOptions options;
  for (int index = 1; index < argc; ++index) {
    const std::string argument = argv[index];
    if (argument == "--help") {
      options.help = true;
      continue;
    }
    if (argument == "--case" && index + 1 < argc) {
      options.case_path = argv[++index];
      continue;
    }
    if (argument == "--variant" && index + 1 < argc) {
      options.variant = argv[++index];
      continue;
    }
    if (argument == "--output-dir" && index + 1 < argc) {
      options.output_dir = argv[++index];
      continue;
    }

    throw std::runtime_error("Unsupported or incomplete argument: " + argument);
  }
  return options;
}

}  // namespace

int main(const int argc, char** argv) {
  try {
    const CliOptions options = ParseArguments(argc, argv);
    if (options.help) {
      std::cout << kUsage;
      return 0;
    }

    if (options.case_path.empty() || options.variant.empty()) {
      std::cerr << "motor_pre requires both --case and --variant.\n" << kUsage;
      return 1;
    }

    const std::filesystem::path repository_root = FindRepositoryRoot(std::filesystem::current_path());
    if (repository_root.empty()) {
      std::cerr << "Unable to locate repository root for tools/gmsh/generate_joint_motor_case.py\n";
      return 1;
    }

    const std::filesystem::path script_path =
        repository_root / "tools" / "gmsh" / "generate_joint_motor_case.py";
    const std::filesystem::path case_path =
        std::filesystem::absolute(repository_root / options.case_path);
    const std::filesystem::path output_dir =
        options.output_dir.empty()
            ? (std::filesystem::current_path() / "preprocess-output")
            : std::filesystem::absolute(options.output_dir);

    std::ostringstream command;
    command << "python3 " << ShellQuote(script_path.string()) << " --case "
            << ShellQuote(case_path.string()) << " --variant " << ShellQuote(options.variant)
            << " --output-dir " << ShellQuote(output_dir.string());

    std::cout << femsolver::io::RenderBanner("motor_pre") << '\n';
    const int status = std::system(command.str().c_str());
    return status == 0 ? 0 : 1;
  } catch (const std::exception& error) {
    std::cerr << error.what() << '\n' << kUsage;
    return 1;
  }
}

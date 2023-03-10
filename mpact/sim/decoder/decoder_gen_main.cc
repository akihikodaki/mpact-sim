// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/log.h"
#include "absl/strings/str_split.h"
#include "mpact/sim/decoder/instruction_set_visitor.h"

// This is the main executable for the encoding independent decoder generator.
// It takes a grammar (.isa) file as input and produces C++ source and header
// files that contain classes that are used to create a full instruction decoder
// for MPACT-Sim. The generated classes have pure virtual (or if base classes
// are generated, virtual with trivial bodies) methods for obtaining instruction
// semantic functions, instruction operands and basic instruction encoding
// values. It is the intent that the user uses these classes as base classes
// for the real decoder, only overriding the methods left pure virtual (or when
// generating the base classes, the method bodies tagged with TODO).
//
// A class is generated for the named base instruction set architecture as well
// as one for each slot and bundle that it references (directly or indirectly).
// Slots that act as base classes for other slots, also have classes generated.

// Flag specifying the output directory of the generated code. When building
// using blaze, this is automatically set by the build command in the .bzl file.
ABSL_FLAG(std::string, output_dir, "./", "output file directory");
// The name prefix for the generated files. The output names will be:
//   <prefix>_decoder.h
//   <prefix>_decoder.cc
// and if the 'generate_base_classes' is given:
//   <prefix>_decoder_base.h
//   <prefix>_decoder_base.cc
// When building using blaze, this is set to the to the base name of the input
// grammar (.isa) file.
ABSL_FLAG(std::string, prefix, "", "prefix for generated files");
// Name of isa to generate for.
ABSL_FLAG(std::string, isa_name, "", "isa name");
ABSL_FLAG(std::string, include, "", "include file root(s)");

int main(int argc, char **argv) {
  auto arg_vec = absl::ParseCommandLine(argc, argv);

  std::string file_name;

  // Open input file as stream if specified.
  if (arg_vec.size() > 1) {
    file_name = arg_vec[1];
  }

  mpact::sim::machine_description::instruction_set::InstructionSetVisitor
      visitor;

  std::string output_dir = absl::GetFlag(FLAGS_output_dir);
  std::string prefix = absl::GetFlag(FLAGS_prefix);
  std::string isa_name = absl::GetFlag(FLAGS_isa_name);
  std::vector<std::string> include_roots;
  auto include_roots_string = absl::GetFlag(FLAGS_include);
  include_roots =
      absl::StrSplit(include_roots_string, ',', absl::SkipWhitespace());

  if (prefix.empty()) {
    std::cerr << "Error: prefix must be specified and non-empty" << std::endl;
    exit(-1);
  }
  auto status =
      visitor.Process(file_name, prefix, isa_name, include_roots, output_dir);
  return status.ok() ? 0 : -1;
}

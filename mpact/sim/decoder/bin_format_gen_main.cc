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

#include "mpact/sim/decoder/bin_format_visitor.h"
#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/log/log.h"
#include "absl/strings/str_split.h"

// Flag specifying the output directory of the generated code. When building
// using blaze, this is automatically set by the build command in the .bzl file.
ABSL_FLAG(std::string, output_dir, "./", "output file directory");
// The name prefix for the generated files. The output names will be:
//   <prefix>_decoder.h
//   <prefix>_decoder.cc
// When building using blaze, this is set to the to the base name of the input
// grammar (.bin) file.
ABSL_FLAG(std::string, prefix, "", "prefix for generated files");
ABSL_FLAG(std::string, decoder_name, "", "decoder name to generate");
ABSL_FLAG(std::string, include, "", "include file root(s)");

int main(int argc, char **argv) {
  auto arg_vec = absl::ParseCommandLine(argc, argv);

  std::string file_name;

  // Open input file as stream if specified.
  if (arg_vec.size() > 1) {
    file_name = arg_vec[1];
  }

  ::mpact::sim::decoder::bin_format::BinFormatVisitor visitor;

  std::string output_dir = absl::GetFlag(FLAGS_output_dir);
  std::string prefix = absl::GetFlag(FLAGS_prefix);
  std::string decoder_name = absl::GetFlag(FLAGS_decoder_name);
  std::vector<std::string> include_roots;
  auto include_roots_string = absl::GetFlag(FLAGS_include);
  include_roots =
      absl::StrSplit(include_roots_string, ',', absl::SkipWhitespace());

  if (prefix.empty()) {
    std::cerr << "Error: prefix must be specified and non-empty" << std::endl;
    exit(-1);
  }

  auto status = visitor.Process(file_name, decoder_name, prefix, include_roots,
                                output_dir);
  if (!status.ok()) {
    LOG(ERROR) << status.message();
    return -1;
  }
  return 0;
}

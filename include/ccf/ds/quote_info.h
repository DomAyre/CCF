// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.

#pragma once
#include "ccf/pal/new_attestation.h"

#include <vector>

namespace ccf
{
  /// Describes a quote (attestation) from trusted hardware
  struct QuoteInfo
  {
    /// Quote format
    pal::attestation::Format format = pal::attestation::Format::oe_sgx_v1;
    /// Enclave quote
    std::vector<uint8_t> quote;
    /// Quote endorsements
    std::vector<uint8_t> endorsements;
  };

  DECLARE_JSON_TYPE(QuoteInfo);
  DECLARE_JSON_REQUIRED_FIELDS(QuoteInfo, format, quote, endorsements);
}

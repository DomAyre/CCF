// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.
#pragma once

#include "ccf/ds/json.h"

namespace ccf::pal::attestation
{
  enum class Format
  {
    oe_sgx_v1 = 0,
    insecure_virtual = 1,
    amd_sev_snp_v1 = 2
  };

  DECLARE_JSON_ENUM(
    Format,
    {{Format::oe_sgx_v1, "OE_SGX_v1"},
     {Format::insecure_virtual, "Insecure_Virtual"},
     {Format::amd_sev_snp_v1, "AMD_SEV_SNP_v1"}});

  // ReportData make_report_data(crypto::Sha256Hash& node_public_key_hash);

  // Attestation generate(ReportData& report_data);

  // bool verify(Attestation& attestation);

  // ReportData get_report_data(Attestation& attestation);

  // std::optional<MREnclave> get_mrenclave(Attestation& attestation);

  // std::optional<Measurement> get_measurement(Attestation& attestation);

  // std::optional<HostData> get_host_data(Attestation& attestation);

  // std::optional<SecurityPolicy> get_policy(Attestation& attestation);

}
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.
#pragma once

#if !defined(INSIDE_ENCLAVE) || defined(VIRTUAL_ENCLAVE)
#include <array>

namespace ccf::pal
{
  static constexpr size_t attestation_report_data_size = 32;
  using attestation_report_data =
    std::array<uint8_t, attestation_report_data_size>;
  static constexpr size_t attestation_measurement_size = 48;
  using attestation_measurement =
    std::array<uint8_t, attestation_measurement_size>;

  namespace snp
  {
    // From https://developer.amd.com/sev/
    constexpr auto amd_milan_root_signing_public_key = R"(-----BEGIN PUBLIC KEY-----
MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEA0Ld52RJOdeiJlqK2JdsV
mD7FktuotWwX1fNgW41XY9Xz1HEhSUmhLz9Cu9DHRlvgJSNxbeYYsnJfvyjx1MfU
0V5tkKiU1EesNFta1kTA0szNisdYc9isqk7mXT5+KfGRbfc4V/9zRIcE8jlHN61S
1ju8X93+6dxDUrG2SzxqJ4BhqyYmUDruPXJSX4vUc01P7j98MpqOS95rORdGHeI5
2Naz5m2B+O+vjsC060d37jY9LFeuOP4Meri8qgfi2S5kKqg/aF6aPtuAZQVR7u3K
FYXP59XmJgtcog05gmI0T/OitLhuzVvpZcLph0odh/1IPXqx3+MnjD97A7fXpqGd
/y8KxX7jksTEzAOgbKAeam3lm+3yKIcTYMlsRMXPcjNbIvmsBykD//xSniusuHBk
gnlENEWx1UcbQQrs+gVDkuVPhsnzIRNgYvM48Y+7LGiJYnrmE8xcrexekBxrva2V
9TJQqnN3Q53kt5viQi3+gCfmkwC0F0tirIZbLkXPrPwzZ0M9eNxhIySb2npJfgnq
z55I0u33wh4r0ZNQeTGfw03MBUtyuzGesGkcw+loqMaq1qR4tjGbPYxCvpCq7+Og
pCCoMNit2uLo9M18fHz10lOMT8nWAUvRZFzteXCm+7PHdYPlmQwUw3LvenJ/ILXo
QPHfbkH0CyPfhl1jWhJFZasCAwEAAQ==
-----END PUBLIC KEY-----
)";

    // Based on the SEV-SNP ABI Spec document at
    // https://www.amd.com/system/files/TechDocs/56860.pdf

    // Table 3
    struct TcbVersion {
      uint8_t boot_loader;
      uint8_t tee;
      uint8_t reserved[4];
      uint8_t snp;
      uint8_t microcode;
    };
    static_assert(sizeof(TcbVersion) == sizeof(uint64_t), "Can't cast TcbVersion to uint64_t");

    struct Signature {
      uint8_t r[72];
      uint8_t s[72];
      uint8_t reserved[512-144];
    };

    // Table. 105
    enum class SignatureAlgorithm : uint32_t
    {
      invalid = 0,
      ecdsa_p384_sha384 = 1
    };

    // Table 21
    struct Attestation {
      uint32_t            version;                                    /* 0x000 */
      uint32_t            guest_svn;                                  /* 0x004 */
      uint64_t            policy;                                     /* 0x008 */
      uint8_t             family_id[16];                              /* 0x010 */
      uint8_t             image_id[16];                               /* 0x020 */
      uint32_t            vmpl;                                       /* 0x030 */
      SignatureAlgorithm  signature_algo;                             /* 0x034 */
      struct TcbVersion   platform_version;                           /* 0x038 */
      uint64_t            platform_info;                              /* 0x040 */
      uint32_t            flags;                                      /* 0x048 */
      uint32_t            reserved0;                                  /* 0x04C */
      uint8_t             report_data[64];                            /* 0x050 */
      uint8_t             measurement[attestation_measurement_size];  /* 0x090 */
      uint8_t             host_data[32];                              /* 0x0C0 */
      uint8_t             id_key_digest[48];                          /* 0x0E0 */
      uint8_t             author_key_digest[48];                      /* 0x110 */
      uint8_t             report_id[32];                              /* 0x140 */
      uint8_t             report_id_ma[32];                           /* 0x160 */
      struct TcbVersion   reported_tcb;                               /* 0x180 */
      uint8_t             reserved1[24];                              /* 0x188 */
      uint8_t             chip_id[64];                                /* 0x1A0 */
      struct TcbVersion   committed_tcb;                              /* 0x1E0 */
      uint8_t             current_minor;                              /* 0x1E8 */
      uint8_t             current_build;                              /* 0x1E9 */
      uint8_t             current_major;                              /* 0x1EA */
      uint8_t             reserved2;                                  /* 0x1EB */
      uint8_t             committed_build;                            /* 0x1EC */
      uint8_t             committed_minor;                            /* 0x1ED */
      uint8_t             committed_major;                            /* 0x1EE */
      uint8_t             reserved3;                                  /* 0x1EF */
      struct TcbVersion   launch_tcb;                                 /* 0x1F0 */
      uint8_t             reserved4[168];                             /* 0x1F8 */
      struct Signature    signature;                                  /* 0x2A0 */
    };

    // Table 20
    struct AttestationReq
    {
        uint8_t report_data[attestation_report_data_size];
        uint32_t vmpl;
        uint8_t reserved[28];
    };

    // Table 23
    struct AttestationResp {
      uint32_t status;
      uint32_t report_size;
      uint8_t  reserved[0x20-0x8];
      struct Attestation report;
      uint8_t padding[64];
      // padding to the size of SEV_SNP_REPORT_RSP_BUF_SZ (i.e., 1280 bytes)
    };

    struct GuestRequest
    {
      uint8_t req_msg_type;
      uint8_t rsp_msg_type;
      uint8_t msg_version;
      uint16_t request_len;
      uint64_t request_uaddr;
      uint16_t response_len;
      uint64_t response_uaddr;
      uint32_t error; /* firmware error code on failure (see psp-sev.h) */
    };

    // Table 99
    enum MsgType
    {
      MSG_TYPE_INVALID = 0,
      MSG_CPUID_REQ,
      MSG_CPUID_RSP,
      MSG_KEY_REQ,
      MSG_KEY_RSP,
      MSG_REPORT_REQ,
      MSG_REPORT_RSP,
      MSG_EXPORT_REQ,
      MSG_EXPORT_RSP,
      MSG_IMPORT_REQ,
      MSG_IMPORT_RSP,
      MSG_ABSORB_REQ,
      MSG_ABSORB_RSP,
      MSG_VMRK_REQ,
      MSG_VMRK_RSP,
      MSG_TYPE_MAX
    };

    // Changes on 5.19+ kernel
    constexpr auto DEVICE = "/dev/sev";
  }

  #define SEV_GUEST_IOC_TYPE 'S'
  #define SEV_SNP_GUEST_MSG_REPORT \
    _IOWR(SEV_GUEST_IOC_TYPE, 0x1, struct snp::GuestRequest)

}

#endif
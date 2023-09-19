// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.

#include "ccf/js_plugin.h"
#include "ccf/js_snp_attestation_plugin.h"
#include "ccf/pal/attestation.h"
#include "ccf/version.h"
#include "js/wrap.h"

#include <algorithm>
#include <quickjs/quickjs.h>
#include <regex>
#include <vector>

namespace ccf::js
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wc99-extensions"

  static JSValue make_js_tcb_version(JSContext* ctx, pal::snp::TcbVersion tcb)
  {
    auto js_tcb = JS_NewObject(ctx);
    JS_SetPropertyStr(
      ctx, js_tcb, "boot_loader", JS_NewUint32(ctx, tcb.boot_loader));
    JS_SetPropertyStr(ctx, js_tcb, "tee", JS_NewUint32(ctx, tcb.tee));
    JS_SetPropertyStr(ctx, js_tcb, "snp", JS_NewUint32(ctx, tcb.snp));
    JS_SetPropertyStr(
      ctx, js_tcb, "microcode", JS_NewUint32(ctx, tcb.microcode));
    return js_tcb;
  }

  static JSValue JS_NewArrayBuffer2(
    JSContext* ctx, std::span<const uint8_t> data)
  {
    return JS_NewArrayBufferCopy(ctx, data.data(), data.size());
  }

  static JSValue js_verify_snp_attestation(
    JSContext* ctx, JSValueConst, int argc, JSValueConst* argv)
  {
    if (argc != 2 && argc != 3)
      return JS_ThrowTypeError(
        ctx, "Passed %d arguments, but expected 2 or 3", argc);
    js::Context& jsctx = *(js::Context*)JS_GetContextOpaque(ctx);

    size_t evidence_size;
    uint8_t* evidence = JS_GetArrayBuffer(ctx, &evidence_size, argv[0]);
    if (!evidence)
    {
      js::js_dump_error(ctx);
      return JS_EXCEPTION;
    }

    size_t endorsements_size;
    uint8_t* endorsements = JS_GetArrayBuffer(ctx, &endorsements_size, argv[1]);
    if (!endorsements)
    {
      js::js_dump_error(ctx);
      return JS_EXCEPTION;
    }

    QuoteInfo quote_info = {};
    quote_info.format = QuoteFormat::amd_sev_snp_v1;
    quote_info.quote = std::vector<uint8_t>(evidence, evidence + evidence_size);
    quote_info.endorsements =
      std::vector<uint8_t>(endorsements, endorsements + endorsements_size);
    if (argc == 3)
    {
      quote_info.endorsed_tcb = jsctx.to_str(argv[2]);
    }

    pal::PlatformAttestationMeasurement measurement = {};
    pal::PlatformAttestationReportData report_data = {};

    try
    {
      pal::verify_snp_attestation_report(quote_info, measurement, report_data);
    }
    catch (const std::exception& e)
    {
      auto e_ = JS_ThrowRangeError(ctx, "%s", e.what());
      js::js_dump_error(ctx);
      return e_;
    }

    auto attestation =
      *reinterpret_cast<const pal::snp::Attestation*>(quote_info.quote.data());

    auto r = JS_NewObject(ctx);

    JS_SetPropertyStr(
      ctx, r, "version", JS_NewUint32(ctx, attestation.version));
    JS_SetPropertyStr(
      ctx, r, "guest_svn", JS_NewUint32(ctx, attestation.guest_svn));
    auto policy = JS_NewObject(ctx);
    JS_SetPropertyStr(
      ctx,
      policy,
      "abi_minor",
      JS_NewUint32(ctx, attestation.policy.abi_minor));
    JS_SetPropertyStr(
      ctx,
      policy,
      "abi_major",
      JS_NewUint32(ctx, attestation.policy.abi_major));
    JS_SetPropertyStr(
      ctx, policy, "smt", JS_NewUint32(ctx, attestation.policy.smt));
    JS_SetPropertyStr(
      ctx,
      policy,
      "migrate_ma",
      JS_NewUint32(ctx, attestation.policy.migrate_ma));
    JS_SetPropertyStr(
      ctx, policy, "debug", JS_NewUint32(ctx, attestation.policy.debug));
    JS_SetPropertyStr(
      ctx,
      policy,
      "single_socket",
      JS_NewUint32(ctx, attestation.policy.single_socket));
    JS_SetProperty(ctx, r, JS_NewAtom(ctx, "policy"), policy);

    JS_SetPropertyStr(
      ctx, r, "family_id", JS_NewArrayBuffer2(ctx, attestation.family_id));
    JS_SetPropertyStr(
      ctx, r, "image_id", JS_NewArrayBuffer2(ctx, attestation.image_id));
    JS_SetPropertyStr(ctx, r, "vmpl", JS_NewUint32(ctx, attestation.vmpl));
    JS_SetPropertyStr(
      ctx,
      r,
      "signature_algo",
      JS_NewUint32(ctx, static_cast<uint32_t>(attestation.signature_algo)));
    JS_SetProperty(
      ctx,
      r,
      JS_NewAtom(ctx, "platform_version"),
      make_js_tcb_version(ctx, attestation.platform_version));

    auto platform_info = JS_NewObject(ctx);
    JS_SetPropertyStr(
      ctx,
      platform_info,
      "smt_en",
      JS_NewUint32(ctx, attestation.platform_info.smt_en));
    JS_SetPropertyStr(
      ctx,
      platform_info,
      "tsme_en",
      JS_NewUint32(ctx, attestation.platform_info.tsme_en));
    JS_SetProperty(ctx, r, JS_NewAtom(ctx, "platform_info"), platform_info);

    auto flags = JS_NewObject(ctx);
    JS_SetPropertyStr(
      ctx,
      flags,
      "author_key_en",
      JS_NewUint32(ctx, attestation.flags.author_key_en));
    JS_SetPropertyStr(
      ctx,
      flags,
      "mask_chip_key",
      JS_NewUint32(ctx, attestation.flags.mask_chip_key));
    JS_SetPropertyStr(
      ctx,
      flags,
      "signing_key",
      JS_NewUint32(ctx, attestation.flags.signing_key));
    JS_SetProperty(ctx, r, JS_NewAtom(ctx, "flags"), flags);

    JS_SetPropertyStr(
      ctx, r, "report_data", JS_NewArrayBuffer2(ctx, attestation.report_data));
    JS_SetPropertyStr(
      ctx, r, "measurement", JS_NewArrayBuffer2(ctx, attestation.measurement));

    JS_SetPropertyStr(
      ctx, r, "host_data", JS_NewArrayBuffer2(ctx, attestation.host_data));
    JS_SetPropertyStr(
      ctx,
      r,
      "id_key_digest",
      JS_NewArrayBuffer2(ctx, attestation.id_key_digest));
    JS_SetPropertyStr(
      ctx,
      r,
      "author_key_digest",
      JS_NewArrayBuffer2(ctx, attestation.author_key_digest));
    JS_SetPropertyStr(
      ctx, r, "report_id", JS_NewArrayBuffer2(ctx, attestation.report_id));
    JS_SetPropertyStr(
      ctx,
      r,
      "report_id_ma",
      JS_NewArrayBuffer2(ctx, attestation.report_id_ma));
    JS_SetProperty(
      ctx,
      r,
      JS_NewAtom(ctx, "reported_tcb"),
      make_js_tcb_version(ctx, attestation.reported_tcb));
    JS_SetPropertyStr(
      ctx, r, "chip_id", JS_NewArrayBuffer2(ctx, attestation.chip_id));
    JS_SetProperty(
      ctx,
      r,
      JS_NewAtom(ctx, "committed_tcb"),
      make_js_tcb_version(ctx, attestation.committed_tcb));
    JS_SetPropertyStr(
      ctx, r, "current_minor", JS_NewUint32(ctx, attestation.current_minor));
    JS_SetPropertyStr(
      ctx, r, "current_build", JS_NewUint32(ctx, attestation.current_build));
    JS_SetPropertyStr(
      ctx, r, "current_major", JS_NewUint32(ctx, attestation.current_major));
    JS_SetPropertyStr(
      ctx,
      r,
      "committed_build",
      JS_NewUint32(ctx, attestation.committed_build));
    JS_SetPropertyStr(
      ctx,
      r,
      "committed_minor",
      JS_NewUint32(ctx, attestation.committed_minor));
    JS_SetPropertyStr(
      ctx,
      r,
      "committed_major",
      JS_NewUint32(ctx, attestation.committed_major));
    JS_SetProperty(
      ctx,
      r,
      JS_NewAtom(ctx, "launch_tcb"),
      make_js_tcb_version(ctx, attestation.launch_tcb));

    auto signature = JS_NewObject(ctx);
    JS_SetProperty(
      ctx,
      signature,
      JS_NewAtom(ctx, "r"),
      JS_NewArrayBuffer2(ctx, attestation.signature.r));
    JS_SetProperty(
      ctx,
      signature,
      JS_NewAtom(ctx, "s"),
      JS_NewArrayBuffer2(ctx, attestation.signature.s));
    JS_SetProperty(ctx, r, JS_NewAtom(ctx, "signature"), signature);

    return r;
  }

#pragma clang diagnostic pop

  static JSValue create_snp_attestation_obj(JSContext* ctx)
  {
    auto snp_attestation = JS_NewObject(ctx);

    JS_SetPropertyStr(
      ctx,
      snp_attestation,
      "verifySnpAttestation",
      JS_NewCFunction(
        ctx, js_verify_snp_attestation, "verifySnpAttestation", 3));

    return snp_attestation;
  }

  static void populate_global_snp_attestation(Context& ctx)
  {
    auto global_obj = ctx.get_global_obj();
    global_obj.set("snp_attestation", create_snp_attestation_obj(ctx));
  }

  FFIPlugin snp_attestation_plugin = {
    .name = "SNP Attestation",
    .ccf_version = ccf::ccf_version,
    .extend = populate_global_snp_attestation};
}
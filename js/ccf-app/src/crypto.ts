// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the Apache 2.0 License.

/**
 * The `crypto` module provides access to common cryptographic
 * algorithms.
 *
 * Example:
 * ```
 * import * as ccfcrypto from '@microsoft/ccf-app/crypto.js';
 *
 * const key = ccfcrypto.generateAesKey(128);
 * ```
 *
 * @module
 */

import { ccf } from "./global.js";

/**
 * @inheritDoc CCF.generateAesKey
 */
export const generateAesKey = ccf.crypto.generateAesKey;

/**
 * @inheritDoc CCF.generateRsaKeyPair
 */
export const generateRsaKeyPair = ccf.crypto.generateRsaKeyPair;

/**
 * @inheritDoc CCF.generateEcdsaKeyPair
 */
export const generateEcdsaKeyPair = ccf.crypto.generateEcdsaKeyPair;

/**
 * @inheritDoc CCF.wrapKey
 */
export const wrapKey = ccf.crypto.wrapKey;

/**
 * @inheritDoc CCFCrypto.verifySignature
 */
export const verifySignature = ccf.crypto.verifySignature;

/**
 * @inheritDoc CCF.digest
 */
export const digest = ccf.crypto.digest;

/**
 * @inheritDoc CCF.isValidX509CertBundle
 */
export const isValidX509CertBundle = ccf.crypto.isValidX509CertBundle;

/**
 * @inheritDoc CCF.isValidX509CertChain
 */
export const isValidX509CertChain = ccf.crypto.isValidX509CertChain;

export {
  WrapAlgoParams,
  AesKwpParams,
  RsaOaepParams,
  RsaOaepAesKwpParams,
  CryptoKeyPair,
  DigestAlgorithm,
  SigningAlgorithm,
  RsaPkcsParams,
  EcdsaParams,
} from "./global";

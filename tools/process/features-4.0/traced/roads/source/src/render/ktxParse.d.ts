// The code of src/render/ktxParse.d.ts that is the feature's (the rest of the file is shared).

// src/render/ktxParse.d.ts:3-12 interface KTX2Sample: the feature's own
  export interface KTX2Sample {
    bitOffset: number;
    /** The bit length minus one. */
    bitLength: number;
    /** The channel id in the low four bits, its qualifiers (`KHR_DF_SAMPLE_DATATYPE_*`) in the high four. */
    channelType: number;
    samplePosition: [number, number, number, number];
    sampleLower: number;
    sampleUpper: number;
  }

// src/render/ktxParse.d.ts:13-24 interface KTX2DataFormatDescriptorBasicFormat: the feature's own
  export interface KTX2DataFormatDescriptorBasicFormat {
    vendorId: number;
    descriptorType: number;
    versionNumber: number;
    colorModel: number;
    colorPrimaries: number;
    transferFunction: number;
    flags: number;
    texelBlockDimension: [number, number, number, number];
    bytesPlane: [number, number, number, number, number, number, number, number];
    samples: KTX2Sample[];
  }

// src/render/ktxParse.d.ts:25-28 interface KTX2Level: the feature's own
  export interface KTX2Level {
    levelData: Uint8Array;
    uncompressedByteLength: number;
  }

// src/render/ktxParse.d.ts:29-44 interface KTX2Container: the feature's own
  export interface KTX2Container {
    vkFormat: number;
    typeSize: number;
    pixelWidth: number;
    pixelHeight: number;
    pixelDepth: number;
    layerCount: number;
    faceCount: number;
    levelCount: number;
    supercompressionScheme: number;
    /** Level 0, the largest, first. */
    levels: KTX2Level[];
    dataFormatDescriptor: KTX2DataFormatDescriptorBasicFormat[];
    keyValue: Record<string, string | Uint8Array>;
    globalData: unknown;
  }

// src/render/ktxParse.d.ts:46-46 function read: the feature's own
  export function read(data: Uint8Array): KTX2Container;

// src/render/ktxParse.d.ts:48-48 variable VK_FORMAT_UNDEFINED: the feature's own
  export const VK_FORMAT_UNDEFINED: number;

// src/render/ktxParse.d.ts:49-49 variable VK_FORMAT_R8G8B8A8_UNORM: the feature's own
  export const VK_FORMAT_R8G8B8A8_UNORM: number;

// src/render/ktxParse.d.ts:50-50 variable VK_FORMAT_R8G8B8A8_SRGB: the feature's own
  export const VK_FORMAT_R8G8B8A8_SRGB: number;

// src/render/ktxParse.d.ts:51-51 variable KHR_SUPERCOMPRESSION_NONE: the feature's own
  export const KHR_SUPERCOMPRESSION_NONE: number;

// src/render/ktxParse.d.ts:52-52 variable KHR_SUPERCOMPRESSION_ZSTD: the feature's own
  export const KHR_SUPERCOMPRESSION_ZSTD: number;

// src/render/ktxParse.d.ts:53-53 variable KHR_DF_MODEL_RGBSDA: the feature's own
  export const KHR_DF_MODEL_RGBSDA: number;

// src/render/ktxParse.d.ts:54-54 variable KHR_DF_PRIMARIES_BT709: the feature's own
  export const KHR_DF_PRIMARIES_BT709: number;

// src/render/ktxParse.d.ts:55-55 variable KHR_DF_TRANSFER_LINEAR: the feature's own
  export const KHR_DF_TRANSFER_LINEAR: number;

// src/render/ktxParse.d.ts:56-56 variable KHR_DF_TRANSFER_SRGB: the feature's own
  export const KHR_DF_TRANSFER_SRGB: number;

// src/render/ktxParse.d.ts:57-57 variable KHR_DF_CHANNEL_RGBSDA_RED: the feature's own
  export const KHR_DF_CHANNEL_RGBSDA_RED: number;

// src/render/ktxParse.d.ts:58-58 variable KHR_DF_CHANNEL_RGBSDA_GREEN: the feature's own
  export const KHR_DF_CHANNEL_RGBSDA_GREEN: number;

// src/render/ktxParse.d.ts:59-59 variable KHR_DF_CHANNEL_RGBSDA_BLUE: the feature's own
  export const KHR_DF_CHANNEL_RGBSDA_BLUE: number;

// src/render/ktxParse.d.ts:60-60 variable KHR_DF_CHANNEL_RGBSDA_ALPHA: the feature's own
  export const KHR_DF_CHANNEL_RGBSDA_ALPHA: number;

// src/render/ktxParse.d.ts:61-61 variable KHR_DF_SAMPLE_DATATYPE_LINEAR: the feature's own
  export const KHR_DF_SAMPLE_DATATYPE_LINEAR: number;

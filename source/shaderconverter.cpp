#include <shaderconverter.hpp>

#include <iostream>
#include <sstream>

std::string errorlist = "";

namespace sc {
void Init() {
  glslang::InitializeProcess();
  errorlist = "";
}

void Finalize() { glslang::FinalizeProcess(); }

bool GLSLtoSPV(const ShaderStage shader_type, const char *pshader,
               std::vector<unsigned int> &spirv) {
  EShLanguage stage = static_cast<EShLanguage>(shader_type);
  glslang::TShader shader(stage);
  glslang::TProgram program;
  const char *shaderStrings[1];
  TBuiltInResource Resources = {};
  InitResources(Resources);

  // Enable SPIR-V and Vulkan rules when parsing GLSL
  EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

  shaderStrings[0] = pshader;
  shader.setStrings(shaderStrings, 1);
  shader.setAutoMapBindings(true);
  shader.setAutoMapLocations(true);

  if (!shader.parse(&Resources, 100, false, messages)) {
    std::cout << shader.getInfoLog() << std::endl;
    std::cout << shader.getInfoDebugLog() << std::endl;
    errorlist += shader.getInfoLog();
    errorlist += "\n";
    errorlist += shader.getInfoDebugLog();
    return false; // something didn't work
  }

  program.addShader(&shader);
  //
  // Program-level processing...
  //

  if (!program.link(messages)) {
    std::cout << shader.getInfoLog() << std::endl;
    std::cout << shader.getInfoDebugLog() << std::endl;
    errorlist += shader.getInfoLog();
    errorlist += "\n";
    errorlist += shader.getInfoDebugLog();
    return false;
  }

  glslang::GlslangToSpv(*program.getIntermediate(stage), spirv);
  return true;
}

bool SPVtoGLSL(std::vector<unsigned int> spirvSource, std::string &output,
               int version, bool es) {
  spirv_cross::CompilerGLSL glsl(spirvSource);
  spirv_cross::CompilerGLSL::Options scoptions;
  scoptions.version = version;
  scoptions.es = es;
  glsl.set_common_options(scoptions);
  output = glsl.compile();
  return true;
}

std::string ConvertGLSL(ShaderFormat out_fmt, ShaderStage s_stg,
                        std::string code, int version, bool es) {
  std::string out_str = "Error";
  std::vector<unsigned int> spirv_code;
  sc::Init();
  bool err = sc::GLSLtoSPV(s_stg, code.c_str(), spirv_code);
  sc::Finalize();
  if (!err)
    return "Error Could Not Compile Your Shader";
  if (out_fmt == ShaderFormat::GLSL) {
    spirv_cross::CompilerGLSL glsl(spirv_code);
    spirv_cross::CompilerGLSL::Options scoptions;
    scoptions.version = version;
    scoptions.es = es;
    glsl.set_common_options(scoptions);
    return glsl.compile();
  } else if (out_fmt == ShaderFormat::HLSL) {
    spirv_cross::CompilerHLSL hlsl(spirv_code);
    spirv_cross::CompilerHLSL::Options hlslOptions;
    hlslOptions.shader_model = 500;
    hlsl.set_hlsl_options(hlslOptions);
    return hlsl.compile();
  } else if (out_fmt == ShaderFormat::MSL) {
    spirv_cross::CompilerMSL msl(spirv_code);
    return msl.compile();
  } else if (out_fmt == ShaderFormat::SPIRV) {
    std::stringstream result;
    std::copy(spirv_code.begin(), spirv_code.end(),
              std::ostream_iterator<uint32_t>(result, " "));
    return result.str().c_str();
  }

  return out_str;
}

void InitResources(TBuiltInResource &Resources) {
  Resources.maxLights = 32;
  Resources.maxClipPlanes = 6;
  Resources.maxTextureUnits = 32;
  Resources.maxTextureCoords = 32;
  Resources.maxVertexAttribs = 64;
  Resources.maxVertexUniformComponents = 4096;
  Resources.maxVaryingFloats = 64;
  Resources.maxVertexTextureImageUnits = 32;
  Resources.maxCombinedTextureImageUnits = 80;
  Resources.maxTextureImageUnits = 32;
  Resources.maxFragmentUniformComponents = 4096;
  Resources.maxDrawBuffers = 32;
  Resources.maxVertexUniformVectors = 128;
  Resources.maxVaryingVectors = 8;
  Resources.maxFragmentUniformVectors = 16;
  Resources.maxVertexOutputVectors = 16;
  Resources.maxFragmentInputVectors = 15;
  Resources.minProgramTexelOffset = -8;
  Resources.maxProgramTexelOffset = 7;
  Resources.maxClipDistances = 8;
  Resources.maxComputeWorkGroupCountX = 65535;
  Resources.maxComputeWorkGroupCountY = 65535;
  Resources.maxComputeWorkGroupCountZ = 65535;
  Resources.maxComputeWorkGroupSizeX = 1024;
  Resources.maxComputeWorkGroupSizeY = 1024;
  Resources.maxComputeWorkGroupSizeZ = 64;
  Resources.maxComputeUniformComponents = 1024;
  Resources.maxComputeTextureImageUnits = 16;
  Resources.maxComputeImageUniforms = 8;
  Resources.maxComputeAtomicCounters = 8;
  Resources.maxComputeAtomicCounterBuffers = 1;
  Resources.maxVaryingComponents = 60;
  Resources.maxVertexOutputComponents = 64;
  Resources.maxGeometryInputComponents = 64;
  Resources.maxGeometryOutputComponents = 128;
  Resources.maxFragmentInputComponents = 128;
  Resources.maxImageUnits = 8;
  Resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
  Resources.maxCombinedShaderOutputResources = 8;
  Resources.maxImageSamples = 0;
  Resources.maxVertexImageUniforms = 0;
  Resources.maxTessControlImageUniforms = 0;
  Resources.maxTessEvaluationImageUniforms = 0;
  Resources.maxGeometryImageUniforms = 0;
  Resources.maxFragmentImageUniforms = 8;
  Resources.maxCombinedImageUniforms = 8;
  Resources.maxGeometryTextureImageUnits = 16;
  Resources.maxGeometryOutputVertices = 256;
  Resources.maxGeometryTotalOutputComponents = 1024;
  Resources.maxGeometryUniformComponents = 1024;
  Resources.maxGeometryVaryingComponents = 64;
  Resources.maxTessControlInputComponents = 128;
  Resources.maxTessControlOutputComponents = 128;
  Resources.maxTessControlTextureImageUnits = 16;
  Resources.maxTessControlUniformComponents = 1024;
  Resources.maxTessControlTotalOutputComponents = 4096;
  Resources.maxTessEvaluationInputComponents = 128;
  Resources.maxTessEvaluationOutputComponents = 128;
  Resources.maxTessEvaluationTextureImageUnits = 16;
  Resources.maxTessEvaluationUniformComponents = 1024;
  Resources.maxTessPatchComponents = 120;
  Resources.maxPatchVertices = 32;
  Resources.maxTessGenLevel = 64;
  Resources.maxViewports = 16;
  Resources.maxVertexAtomicCounters = 0;
  Resources.maxTessControlAtomicCounters = 0;
  Resources.maxTessEvaluationAtomicCounters = 0;
  Resources.maxGeometryAtomicCounters = 0;
  Resources.maxFragmentAtomicCounters = 8;
  Resources.maxCombinedAtomicCounters = 8;
  Resources.maxAtomicCounterBindings = 1;
  Resources.maxVertexAtomicCounterBuffers = 0;
  Resources.maxTessControlAtomicCounterBuffers = 0;
  Resources.maxTessEvaluationAtomicCounterBuffers = 0;
  Resources.maxGeometryAtomicCounterBuffers = 0;
  Resources.maxFragmentAtomicCounterBuffers = 1;
  Resources.maxCombinedAtomicCounterBuffers = 1;
  Resources.maxAtomicCounterBufferSize = 16384;
  Resources.maxTransformFeedbackBuffers = 4;
  Resources.maxTransformFeedbackInterleavedComponents = 64;
  Resources.maxCullDistances = 8;
  Resources.maxCombinedClipAndCullDistances = 8;
  Resources.maxSamples = 4;
  Resources.maxMeshOutputVerticesNV = 256;
  Resources.maxMeshOutputPrimitivesNV = 512;
  Resources.maxMeshWorkGroupSizeX_NV = 32;
  Resources.maxMeshWorkGroupSizeY_NV = 1;
  Resources.maxMeshWorkGroupSizeZ_NV = 1;
  Resources.maxTaskWorkGroupSizeX_NV = 32;
  Resources.maxTaskWorkGroupSizeY_NV = 1;
  Resources.maxTaskWorkGroupSizeZ_NV = 1;
  Resources.maxMeshViewCountNV = 4;
  Resources.limits.nonInductiveForLoops = 1;
  Resources.limits.whileLoops = 1;
  Resources.limits.doWhileLoops = 1;
  Resources.limits.generalUniformIndexing = 1;
  Resources.limits.generalAttributeMatrixVectorIndexing = 1;
  Resources.limits.generalVaryingIndexing = 1;
  Resources.limits.generalSamplerIndexing = 1;
  Resources.limits.generalVariableIndexing = 1;
  Resources.limits.generalConstantMatrixVectorIndexing = 1;
}
std::string GetErrorList() { return errorlist; }
} // namespace sc
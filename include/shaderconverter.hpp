#pragma once
#include <SPIRV/GlslangToSpv.h>

#include "spirv.hpp"
#include "spirv_glsl.hpp"
#include "spirv_hlsl.hpp"
#include "spirv_msl.hpp"

namespace sc {
enum ShaderFormat {
  GLSL,
  HLSL,
  MSL,
  SPIRV,
  ShaderFormatMax,
};

enum ShaderStage {
  Vertex,
  TessControl,
  TessEvaluation,
  Geometry,
  Fragment,
  Compute,
  ShaderStageMax,
};

void InitResources(TBuiltInResource &Resources);
void Init();
void Finalize();
bool GLSLtoSPV(const ShaderStage shader_type, const char *pshader,
               std::vector<unsigned int> &spirv);
bool SPVtoGLSL(std::vector<unsigned int> spirvSource, std::string &output,
               int version, bool es = false);
std::string ConvertGLSL(ShaderFormat out_fmt, ShaderStage s_stg,
                        std::string code, int version, bool es = false);
std::string GetErrorList();
}; // namespace sc
#include <NImGui/NImGui.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include <shaderconverter.hpp>

#include <map>
#include <vector>

#if defined(_MSC_VER)
#include <Windows.h>
#undef LoadImage
static void FreeConsoleW(){
HWND hwnd = GetConsoleWindow(); 
ShowWindow(hwnd, 0);}
#else
static void FreeConsoleW(){}
#endif

#include "ImGuiTextEditor.hpp"

static const char *const vertTest = R"text(  
#version 450
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
  
out vec4 vertexColor; // specify a color output to the fragment shader

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    vertexColor = vec4(0.5, 0.0, 0.0, 1.0); // set the output variable to a dark-red color
}
)text";

std::vector<int> glslvl = {
    {110}, {120}, {130}, {140}, {150}, {330}, {400},
    {410}, {420}, {430}, {440}, {450}, {460},
};

std::vector<int> glslesvl = {
    {110},
    /*{300},
    {310},
    {320},*/
};

bool ValidateVersion(int ver, bool es) {
  if (es) {
    for (int res : glslesvl) {
      if (ver == res)
        return true;
    }
    return false;

  } else {
    for (int res : glslvl) {
      if (ver == res)
        return true;
    }
    return false;
  }
  return false;
}

bool rnn = true;
bool selnone = false;

// MenuBarStuff
bool exit_ = false;
bool errconsole = false;

void HandleMenuBar() {
  selnone = false;
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      exit_ = ImGui::MenuItem("Exit", "Strg + Q", &selnone);
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("DebugConsole", "", &errconsole);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}

TextEditor imtext;
TextEditor otext;

const char *shader_lang_[] = {"GLSL", "GLSL ES", "SPIRV", "HLSL", "MSL"};
std::string current_item_shader_lang = shader_lang_[0];
std::string glslver = "100";
const char *shader_stage_[] = {"Vertex", "Fragment"};
std::string current_shader_stage_ = shader_stage_[0];

std::map<std::string, sc::ShaderFormat> fmt_lib = {
    {"GLSL", sc::GLSL}, {"GLSL ES", sc::GLSL}, {"SPIRV", sc::SPIRV},
    {"HLSL", sc::HLSL}, {"MSL", sc::MSL},
};

int main() {
  FreeConsoleW();
  NImGui::App app("Shader Studio", NImGui::Vec2i(700, 700),
                  NImGui::MAXIMIZED | NImGui::NORESIZE | NImGui::BORDERLESS);
  app.SetIcon("res/icon.png");
  /*std::cout << "Input Shader:" << std::endl;
  std::cout << vertTest << std::endl;
  std::cout << "HLSL Shader:" << std::endl;
  std::cout << sc::ConvertGLSL(sc::HLSL, sc::Vertex, vertTest, 110)
            << std::endl;
  std::cout << "MSL Shader:" << std::endl;
  std::cout << sc::ConvertGLSL(sc::MSL, sc::Vertex, vertTest, 110) << std::endl;
  std::cout << "SPIRV Shader:" << std::endl;
  std::cout << sc::ConvertGLSL(sc::SPIRV, sc::Vertex, vertTest, 110)
            << std::endl;
  std::cout << "GLSL ES Shader:" << std::endl;
  std::cout << sc::ConvertGLSL(sc::GLSL, sc::Vertex, vertTest, 110, true)
            << std::endl;
  std::cout << "GLSL Shader:" << std::endl;
  std::cout << sc::ConvertGLSL(sc::GLSL, sc::Vertex, vertTest, 450)
            << std::endl;*/
  ImGui::Spectrum::LoadFont(22);
  ImGui::Spectrum::StyleColorsSpectrum();
  auto lang = TextEditor::LanguageDefinition::GLSL();
  imtext.SetLanguageDefinition(lang);
  otext.SetReadOnly(true);
  while (app.IsRunning() && rnn) {
    if(app.IsKeyDown(NImGui::KeyCode::LeftControl) && app.IsKeyDown(NImGui::KeyCode::Q))
      rnn = false;
    auto cpos = imtext.GetCursorPosition();
    if (imtext.IsTextChanged() &&
        ValidateVersion(std::atoi(glslver.c_str()),
                        (current_item_shader_lang == "GLSL ES") ? true
                                                                : false)) {
      sc::ShaderStage stg__ = sc::ShaderStage::Vertex;
      if(current_shader_stage_ == shader_stage_[0])
        stg__ = sc::ShaderStage::Vertex;
      if(current_shader_stage_ == shader_stage_[1])
        stg__ = sc::ShaderStage::Vertex;
      std::string res = sc::ConvertGLSL(
          fmt_lib.at(current_item_shader_lang), sc::Vertex, imtext.GetText(),
          std::atoi(glslver.c_str()),
          (current_item_shader_lang == "GLSL ES") ? true : false);
      otext.SetText(res);
    }
    ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::Begin("Shader Studio", &rnn,
                 ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar);
    ImGui::SetWindowSize(ImVec2(app.GetWindowSize().x, app.GetWindowSize().y));
    HandleMenuBar();
    ImGui::Text("Editor: GLSL");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::BeginCombo("##Stage", current_shader_stage_.c_str(),
                          ImGuiComboFlags_HeightSmall)) {
      for (int n = 0; n < IM_ARRAYSIZE(shader_stage_); n++) {
        bool is_selected =
            (current_shader_stage_ ==
             shader_stage_[n]); // You can store your selection however you want,
                               // outside or inside your objects
        if (ImGui::Selectable(shader_stage_[n], is_selected))
          current_shader_stage_ = shader_stage_[n];
        if (is_selected)
          ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                        // opening the combo (scrolling + for
                                        // keyboard navigation support)
      }
      ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo() returns
                         // true!
    }
    ImGui::SameLine(0, (ImGui::GetWindowSize().x / 2 -
                        ImGui::CalcTextSize("Editor: GLSL").x));
    ImGui::Text("Output: ");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    if (ImGui::BeginCombo("##Language", current_item_shader_lang.c_str(),
                          ImGuiComboFlags_HeightSmall)) {
      for (int n = 0; n < IM_ARRAYSIZE(shader_lang_); n++) {
        bool is_selected =
            (current_item_shader_lang ==
             shader_lang_[n]); // You can store your selection however you want,
                               // outside or inside your objects
        if (ImGui::Selectable(shader_lang_[n], is_selected))
          current_item_shader_lang = shader_lang_[n];
        if (is_selected)
          ImGui::SetItemDefaultFocus(); // You may set the initial focus when
                                        // opening the combo (scrolling + for
                                        // keyboard navigation support)
      }
      ImGui::EndCombo(); // only call ImGui::EndCombo() if BeginCombo() returns
                         // true!
    }
    if (current_item_shader_lang == "GLSL" ||
        current_item_shader_lang == "GLSL ES") {
      ImGui::SameLine();
      ImGui::SetNextItemWidth(100);
      ImGui::PushStyleColor(
          ImGuiCol_Text,
          (!ValidateVersion(std::atoi(glslver.c_str()),
                            (current_item_shader_lang == "GLSL ES") ? true
                                                                    : false))
              ? IM_COL32(255, 0, 0, 255)
              : IM_COL32(0, 255, 0, 255));
      ImGui::InputText("##Version", &glslver);
      ImGui::PopStyleColor();
    }
    ImGui::BeginChild(
        "editor",
        ImVec2((ImGui::GetWindowSize().x / 2 - 20),
               (ImGui::GetWindowSize().y - (errconsole ? 300 : 120))),
        true);
    imtext.Render("TextEditor");

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild(
        "result",
        ImVec2((ImGui::GetWindowSize().x / 2 - 20),
               (ImGui::GetWindowSize().y - (errconsole ? 300 : 120))),
        true);
    otext.Render("Output: ");

    ImGui::EndChild();
    if (errconsole) {
      ImGui::Text("Console: ");
      ImGui::BeginChild("errors", ImVec2(-1, -1), true);
      ImGui::Text(sc::GetErrorList().c_str());
      ImGui::EndChild();
    }
    ImGui::End();
    app.SwapBuffers();
    if (exit_)
      break;
  }
  return 0;
}
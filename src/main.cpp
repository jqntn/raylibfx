#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <raylib.h>
#include <resources.h>
#include <shaders.h>
#include <string>

extern "C"
{
  __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
  __declspec(dllexport) unsigned long AmdPowerXpressRequestHighPerformance =
    0x00000001;
}

namespace fs = std::filesystem;

static Model
LoadModelFromMemory(const char* fileType,
                    const unsigned char* fileData,
                    int dataSize)
{
  fs::path file_path =
    fs::temp_directory_path() / ("out" + std::string(fileType));

  std::ofstream out(file_path, std::ios::binary);
  out.write(reinterpret_cast<const char*>(fileData), dataSize);
  out.close();

  Model model = LoadModel(file_path.string().c_str());

  fs::remove(file_path);

  return model;
}

int
main()
{
  std::string window_title = "raylibfx";
  uint32_t window_width = 1280;
  uint32_t window_height = 720;
  uint32_t scale_factor = 1;

  SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
  InitWindow(window_width * scale_factor,
             window_height * scale_factor,
             window_title.c_str());

  Image image =
    LoadImageFromMemory(".png", pokeemerald_png, pokeemerald_png_len);
  Texture2D texture = LoadTextureFromImage(image);
  UnloadImage(image);

  Model model = LoadModelFromMemory(".glb", helmet_glb, helmet_glb_len);

  Shader shader = LoadShaderFromMemory(
    NULL,
    std::string(reinterpret_cast<char*>(blur_frag), blur_frag_len).c_str());

  RenderTexture2D target = LoadRenderTexture(window_width * scale_factor,
                                             window_height * scale_factor);

  Vector3 position = Vector3{ 0.0f, 1.0f, 0.0f };
  Camera camera = {
    .position = Vector3{ 2.0f, 2.0f, 2.0f },
    .target = position,
    .up = Vector3{ 0.0f, 1.0f, 0.0f },
    .fovy = 50.0f,
    .projection = CAMERA_PERSPECTIVE,
  };

  while (!WindowShouldClose()) {
    UpdateCamera(&camera, CAMERA_ORBITAL);

    BeginTextureMode(target);
    ClearBackground(RAYWHITE);

    BeginMode3D(camera);
    DrawModel(model, position, 1.0f, WHITE);
    DrawGrid(10, 1.0f);
    EndMode3D();

    EndTextureMode();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginShaderMode(shader);
    DrawTextureRec(target.texture,
                   Rectangle{ 0.0f,
                              0.0f,
                              (float)target.texture.width,
                              (float)-target.texture.height },
                   Vector2{},
                   WHITE);
    EndShaderMode();

    DrawTextureEx(texture, Vector2{}, 0.0f, (float)scale_factor, WHITE);

    EndDrawing();
  }

  UnloadTexture(texture);
  UnloadModel(model);
  UnloadShader(shader);
  UnloadRenderTexture(target);

  CloseWindow();
}
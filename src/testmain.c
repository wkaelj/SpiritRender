// test main function

#include <spirit_header.h>
#include <unistd.h>

// types
#include "render/spirit_context.h"

#include "render/spirit_device.h"
#include "render/spirit_material.h"
#include "render/spirit_mesh.h"

// utils
#include "glsl-loader/glsl_loader.h"
#include "render/spirit_types.h"
#include "render/spirit_window.h"
#include "utils/platform.h"
#include "utils/spirit_file.h"
#include "utils/spirit_vector.h"

void mainlooptest(void);

// run all the tests
void Test(void);

int main(int argc, char **argv) {

  spPlatformSetExecutableFolder(argv[0]);

  if (argc >= 2 && strcmp("-h", argv[1]) == 0) {
    printf("Usage:\n\t"
           "--test - run the tests\n\t"
           "--delete-shader-cache - delete the shader cache\n");
  }

  // tests
  if (argc >= 2 && strcmp("--test", argv[1]) == 0) {
    Test();
    return 0;
  }

  if (argc >= 2 && strcmp("--delete-shader-cache", argv[1]) == 0) {
    spPlatformDeleteFolder(GLSL_LOADER_CACHE_FOLDER);
    return 0;
  }

  mainlooptest();

  return 0;
}

void mainlooptest(void) {

  SpiritContextCreateInfo contextInfo = {};
  contextInfo.enableValidation = true;
  contextInfo.windowName = "Hi Square :D";
  contextInfo.windowSize = (SpiritResolution){1400, 1400};
  contextInfo.windowFullscreen = false;

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
  struct FunctionTimerData startup = start_timer("startup");
#endif
  SpiritContext context;
  time_function_with_return(spCreateContext(&contextInfo), context);

  if (context == NULL)
    return;

  SpiritMaterialCreateInfo materialInfo = {
      .name = "std",
      .fragmentShader = "shaders/simple_shader.frag",
      .vertexShader = "shaders/simple_shader.vert"};

  SpiritMaterial material;
  time_function_with_return(spCreateMaterial(context, &materialInfo), material);
  SpiritMaterial material2 = spCreateMaterial(context, &materialInfo);

  if (material == NULL)
    return;

  spContextAddMaterial(context, material);
  spContextAddMaterial(context, material2);

  vec3 meshVerts[] = {
      {-0.5, -0.5f, 0.0f},
      {-0.5f, 0.5f, 0.0f},
      {0.5f, -0.5f, 0.0f},
  };
  SpiritMeshCreateInfo meshInfo = {.vertCount = 3, .verts = meshVerts};
  SpiritMesh mesh = spCreateMesh(context, &meshInfo);

  vec3 meshVerts2[] = {
      {0.5f, -0.5f, 0.0f},
      {-0.5f, 0.5f, 0.0f},
      {0.5f, 0.5f, 0.0f},
  };
  SpiritMeshCreateInfo meshInfo2 = {.vertCount = 3, .verts = meshVerts2};
  SpiritMesh mesh2 = spCreateMesh(context, &meshInfo2);

  SpiritMeshManager meshManager;
  time_function_with_return(spCreateMeshManager(context, NULL), meshManager);

  const SpiritMeshReference meshRef = spMeshManagerAddMesh(meshManager, mesh);
  const SpiritMeshReference meshRef2 = spMeshManagerAddMesh(meshManager, mesh2);

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
  end_timer(startup);
#endif

  u64 frameIndex = 0; // check the current frame being rendered
  float rotation = 0.f;
  while (spContextPollEvents(context) != SPIRIT_WINDOW_CLOSED) {

#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
    struct FunctionTimerData frametime = start_timer("frametime");
#endif

    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(matrix, rotation * GLM_PI, SPIRIT_AXIS_FORWARD);

    SpiritPushConstant uniform;
    glm_mat4_dup(matrix, uniform.transform);
    glm_vec3_copy((vec3){37.f / 255.f, 193.f / 55.f, 13.f / 255.f},
                  uniform.color);

    time_function(spMaterialAddMesh(material, meshRef, uniform));
    glm_vec3_copy((vec3){109.f / 255.f, 5.f / 255.f, 117.f / 255.f},
                  uniform.color);
    time_function(spMaterialAddMesh(material, meshRef2, uniform));

    glm_translate_x(uniform.transform, .5f);
    glm_vec3_copy((vec3){255.f / 37.f, 255.f / 193.f, 255.f / 13.f},
                  uniform.color);
    time_function(spMaterialAddMesh(material2, meshRef, uniform));
    glm_vec3_copy((vec3){255.f / 109.f, 255.f / 5.f, 255.f / 117.f},
                  uniform.color);
    time_function(spMaterialAddMesh(material2, meshRef2, uniform));

    SpiritResult result = SPIRIT_SUCCESS;
    time_function_with_return(spContextSubmitFrame(context), result);

    if (result) {
      log_error("Error on frame %lu", frameIndex);
    }
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
    end_timer(frametime);
#endif

    ++frameIndex;
    rotation += 0.0001f;
    if (rotation >= 2.0f)
      rotation = 0.f;
  }

  spDeviceWaitIdle(context->device);

  spReleaseMesh(meshRef);
  spReleaseMesh(meshRef2);

  spDestroyMeshManager(context, meshManager);
  spDestroyMaterial(context, material);
  spDestroyMaterial(context, material2);

  spDestroyContext(context);
}

//
// Tests
//

void runTest(bool test) {
  static u32 testIntex;
  if (test) {
    log_info("Test %u passed.", testIntex);
  } else {
    log_error("Test %u failed.", testIntex);
  }

  testIntex++;
}

// tests

bool TestVector(const int *values, const size_t valueCount) {

  // VECTOR
  VECTOR(int) vec;
  VECTOR_INIT(&vec, valueCount / 2);

  for (u32 i = 0; i < valueCount; i++) {
    VECTOR_PUSH_BACK(&vec, values[i]);
  }

  for (u32 i = 0; i < valueCount; i++) {
    if (vec.at[i] != values[i]) {
      log_fatal("Push failed");
      VECTOR_DELETE(&vec);
      return false;
    }
  }

  VECTOR_INSERT(&vec, 7, valueCount / 4);
  if (vec.at[valueCount / 4] != 7) {
    log_fatal("Intertion failed");
    VECTOR_DELETE(&vec);
    return false;
  }

  VECTOR_POP(&vec, valueCount / 4);
  if (VECTOR_AT(&vec, valueCount / 4) != values[valueCount / 4]) {
    log_fatal("Pop failed");
    VECTOR_DELETE(&vec);
    return false;
  }

  VECTOR_DELETE(&vec);

  // DVECTOR
  DVECTOR dvec;
  DVECTOR_INIT(&dvec, sizeof(int), valueCount / 2);
  const unsigned testIndex = 2;
  ((int *)dvec.data)[testIndex] = 5;
  db_assert(dvec.elementSize == sizeof(int));

  db_assert(DVECTOR_AT_REF(&dvec, testIndex) == &((int *)dvec.data)[testIndex])
      db_assert(*(int *)DVECTOR_AT_REF(&dvec, testIndex) == 5);

  db_assert(dvec.size == 0) for (u32 i = 0; i < valueCount; i++) {
    db_assert(i == dvec.size);
    DVECTOR_PUSH_BACK(&dvec, &values[i]);
    db_assert(DVECTOR_AT(&dvec, i, int) == values[i]);
  }

  for (u32 i = 0; i < valueCount; i++) {
    if (DVECTOR_AT(&dvec, i, int) != values[i]) {
      log_fatal("Push failed");
      DVECTOR_DELETE(&dvec);
      return false;
    }
  }

  const int t = 7;
  DVECTOR_INSERT(&dvec, &t, valueCount / 4);
  if (DVECTOR_AT(&dvec, valueCount / 4, int) != t) {
    log_fatal("Intertion failed");
    DVECTOR_DELETE(&dvec);
    return false;
  }

  DVECTOR_POP(&dvec, valueCount / 4);
  if (DVECTOR_AT(&dvec, valueCount / 4, int) != values[valueCount / 4]) {
    log_fatal("Pop failed");
    DVECTOR_DELETE(&dvec);
    return false;
  }

  DVECTOR_DELETE(&dvec);

  return true;
}

bool TestPlatformLocalizeFilename(const char *input, const char *expected) {

  u32 max = 0;
  time_function(spPlatformLocalizeFileName(NULL, input, &max));
  char output[max];
  time_function(spPlatformLocalizeFileName(output, input, &max));
  if (strcmp(output, expected)) {
    log_error("Expected %s, got %s", expected, output);
    return false;
  }
  return true;
}

bool TestStringTruncate(const char *input, const char slicer, bool inclusive,
                        const char *expected) {

  u32 len = strlen(input) + 1;
  char dest[len];

  time_function(spStringTruncate(dest, &len, input, slicer, inclusive));

  if (strcmp(dest, expected)) {
    log_error("Expected %s, got %s", expected, dest);
    return false;
  }
  return true;
}

bool TestStringStrip(const char *input, const char slicer,
                     const char *expected) {

  u32 len = strlen(input) + 1;
  char dest[len];

  time_function(spStringStrip(dest, &len, input, slicer));

  if (strcmp(dest, expected)) {
    log_error("Expected %s, got %s", expected, dest);
    return false;
  }
  return true;
}

/**
 * @brief Test the built in file utilities. The function will create a file,
 * write text to it, read the file and verify the contents. It then deletes the
 * file. NOTE: this test will fail if the file is inside a folder that doesn't
 * exist.
 *
 * @param textFileName
 * @param testFileContents
 * @return true
 * @return false
 */
bool TestFileUtilities(const char *restrict testFileName,
                       const char *restrict testFileContents) {

  SpiritResult success = 0;

  u64 testFileContentsLength = strlen(testFileContents) + 1;
  char buf[testFileContentsLength]; // declare buf for goto

  time_function_with_return(spWriteFileBinary(testFileName, testFileContents,
                                              testFileContentsLength - 1),
                            success);
  if (success)
    return false;

  time_function_with_return(
      spReadFileText(buf, testFileName, testFileContentsLength), success);
  if (success)
    goto failure;

  int cmpResult;
  time_function_with_return(
      strncmp(buf, testFileContents, testFileContentsLength), cmpResult);
  if (cmpResult != 0) {
    log_error("input string and read string do not match");
    goto failure;
  }

  memset(buf, 0, testFileContentsLength);
  time_function_with_return(
      spReadFileBinary(buf, testFileName, testFileContentsLength), success);
  if (success)
    goto failure;
  cmpResult = strncmp(buf, testFileContents, testFileContentsLength);
  if (cmpResult != 0) {
    return false;
  }

  time_function_with_return(spPlatformDeleteFile(testFileName), success);
  if (success)
    return false;

  return true;
  ;

failure:
  spPlatformDeleteFile(testFileName);
  return false;
}

bool TestGLSLLoader(const char *restrict shaderPath) {
  // TODO

  SpiritShader testShaderSource; // loaded from source
  time_function_with_return(
      spLoadSourceShader(shaderPath, SPIRIT_SHADER_TYPE_FRAGMENT),
      testShaderSource);

  if (testShaderSource.shaderSize == 0) {
    return false;
  }

  SpiritShader testShaderBinary; // loaded from binary
  time_function_with_return(
      spLoadSourceShader(shaderPath, SPIRIT_SHADER_TYPE_FRAGMENT),
      testShaderBinary);

  if (testShaderBinary.shaderSize == 0) {
    spDestroyShader(testShaderSource);
    return false;
  }

  bool equal = true;

  if (testShaderSource.shaderSize == testShaderBinary.shaderSize &&
      testShaderSource.type == testShaderBinary.type)
    ;
  else {
    log_warning("Shaders were not equal");
    equal = false;
  }

  if (equal && memcmp(testShaderSource.shader, testShaderBinary.shader,
                      testShaderSource.shaderSize) != 0) {
    equal = false;
  }

  // formulate test shader binary name (hacked)
  u32 bufLen = 256;
  char stripBuf[256];
  spStringStrip(stripBuf, &bufLen, shaderPath, '/');

  char buf[256];
  snprintf(buf, bufLen, "%s%s.spv", GLSL_LOADER_CACHE_FOLDER, stripBuf);

  spPlatformDeleteFile(buf);

  time_function(spDestroyShader(testShaderSource));
  time_function(spDestroyShader(testShaderBinary));

  return equal;
}

void Test(void) {
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
  init_timer();
#endif
  for (size_t i = 0; i < 1; i++) {
    // runTest(TestGLSLLoader("tests/test_shader.frag"));
    runTest(
        TestPlatformLocalizeFilename("file/test.png", "./bin/file/test.png"));
    runTest(
        TestPlatformLocalizeFilename("file\\test.mov", "./bin/file/test.mov"));
    runTest(TestStringTruncate("filename.txt", '.', false, "filename"));
    runTest(TestStringStrip("filename.txt", '.', "txt"));
    runTest(
        TestFileUtilities("testfile.txt", "Testing test file\nNewline test"));
    const int arr[] = {5, 6, 4, 5, 2, 192381, 1028329};
    runTest(TestVector(arr, array_length(arr)));
  }
#ifndef FUNCTION_TIMER_NO_DIAGNOSTIC
  terminate_timer();
#endif
}

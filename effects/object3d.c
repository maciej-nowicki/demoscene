#include <math.h>

#include "std/debug.h"
#include "std/memory.h"
#include "std/resource.h"

#include "engine/ms3d.h"
#include "engine/object.h"
#include "engine/scene.h"
#include "tools/frame.h"
#include "tools/loopevent.h"

#include "system/c2p.h"
#include "system/display.h"
#include "system/input.h"
#include "system/vblank.h"

const int WIDTH = 320;
const int HEIGHT = 256;
const int DEPTH = 8;

/*
 * Set up resources.
 */
void AddInitialResources() {
  ResAdd("Scene", NewScene());
  ResAdd("Mesh", NewMeshFromFile("data/wecan_logo.robj"));
  ResAdd("Canvas", NewPixBuf(PIXBUF_CLUT, WIDTH, HEIGHT));
  ResAdd("ColorMap", NewPixBufFromFile("data/wecan_logo_cmap.8"));
  ResAdd("Palette", NewPaletteFromFile("data/wecan_logo_cmap.pal"));

  {
    MeshT *mesh = R_("Mesh");

    CalculateSurfaceNormals(mesh);
    NormalizeMeshSize(mesh);
    MeshApplyPalette(mesh, R_("Palette"));
  }

  SceneAddObject(R_("Scene"), NewSceneObject("Object", R_("Mesh")));

  RenderFlatShading = false;
}

/*
 * Set up display function.
 */
bool SetupDisplay() {
  return InitDisplay(WIDTH, HEIGHT, DEPTH);
}

/*
 * Set up effect function.
 */
void SetupEffect() {
  PixBufClear(R_("Canvas"));
  LoadPalette(R_("Palette"));
}

/*
 * Tear down effect function.
 */
void TearDownEffect() {
}

/*
 * Effect rendering functions.
 */

void RenderMesh(int frameNumber_) {
  PixBufT *canvas = R_("Canvas");
  SceneT *scene = R_("Scene");
  float frameNumber = frameNumber_;
  float s = sin(frameNumber * 3.14159265f / 90.0f) + 1.0f;

  {
    MatrixStack3D *ms = GetObjectTranslation(scene, "Object");

    StackReset(ms);
    PushScaling3D(ms, 0.75f + 0.5f * s, 0.75f + 0.5f * s, 0.75f + 0.5f * s);
    PushRotation3D(ms, 0, (float)(-frameNumber * 2), frameNumber);
    PushTranslation3D(ms, 0.0f, 0.0f, -2.0f);
  }

  PixBufClear(canvas);
  RenderScene(scene, canvas);

  c2p1x1_8_c5_bm(canvas->data, GetCurrentBitMap(), WIDTH, HEIGHT, 0, 0);
}

/*
 * Main loop.
 */
void MainLoop() {
  LoopEventT event = LOOP_CONTINUE;

  SetVBlankCounter(0);

  do {
    static bool paused = FALSE;
    static int oldFrameNumber = 0;
    int frameNumber = GetVBlankCounter();

    if (event == LOOP_PAUSE)
      paused = !paused;

    if (paused) {
      SetVBlankCounter(oldFrameNumber);
      frameNumber = oldFrameNumber;
    } else {
      oldFrameNumber = frameNumber;
    }

    if (event == LOOP_TRIGGER)
      RenderWireFrame = !RenderWireFrame;

    RenderMesh(frameNumber);
    RenderFrameNumber(frameNumber);
    RenderFramesPerSecond(frameNumber);

    DisplaySwap();
  } while ((event = ReadLoopEvent()) != LOOP_EXIT);
}

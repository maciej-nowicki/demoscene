//#define NDEBUG

#include <string.h>

#include "std/debug.h"
#include "std/memory.h"
#include "std/resource.h"

extern ResourceT ResourceList[];

bool ResourcesAlloc() {
  ResourceT *res;

  for (res = ResourceList; res->Name; res++) {
    if (!res->AllocFunc)
      continue;

    if (!(res->Ptr = res->AllocFunc())) {
      LOG("Failed to Allocate resource '%s'.\n", res->Name);
      return FALSE;
    }

    LOG("Allocated resource '%s' at %p.\n", res->Name, res->Ptr);
  }

  return TRUE;
}

bool ResourcesInit() {
  ResourceT *res;

  for (res = ResourceList; res->Name; res++) {
    if (!res->InitFunc)
      continue;

    LOG("Initiating resource '%s'.\n", res->Name);

    if (!res->InitFunc(res->Ptr))
      return FALSE;
  }

  return TRUE;
}

void ResourcesFree() {
  ResourceT *res;

  for (res = ResourceList; res->Name; res++) {
    LOG("Freeing resource '%s' at %p.\n", res->Name, res->Ptr);

    if (res->FreeFunc)
      res->FreeFunc(res->Ptr);
    else if (res->AllocFunc)
      DELETE(res->Ptr);
  }
}

void *GetResource(const char *name) {
  struct Resource *res;

  for (res = ResourceList; res->Name; res++) {
    if (strcmp(res->Name, name) == 0) {
      LOG("Fetched resource '%s' at %p.\n", res->Name, res->Ptr);
      return res->Ptr;
    }
  }

  LOG("Resource '%s' not found.\n", name);
  
  return NULL;
}

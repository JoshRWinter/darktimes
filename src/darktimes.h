#ifndef DARKTIMES_H
#define DARKTIMES_H

#include <win.h>
#include "press.h"

#include "entity/entity.h"
#include "render/renderer.h"
#include "world.h"

#define dput(string, ...) press::fwriteln(stderr, string, ##__VA_ARGS__)

#endif

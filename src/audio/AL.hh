#pragma once

//
// This file includes OpenAL:
//
#ifdef __APPLE__
#  include <OpenAL/al.h>
#  include <OpenAL/alc.h>
#else
// Linux & Win
#  include <AL/al.h>
#  include <AL/alc.h>
#endif

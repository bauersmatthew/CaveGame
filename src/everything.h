
#pragma once

// be wary of using code reloading with this, it's shared between platform and game

#include <stdint.h>

#define DEBUG

#ifdef DEBUG
	#include <assert.h>
	#define INVALID_PATH assert(false);
	#define BOUNDS_CHECK
#else
	#define assert(t) 
	#define INVALID_PATH
#endif

typedef uint8_t 	u8;
typedef int8_t 		i8;
typedef uint16_t 	u16;
typedef int16_t 	i16;
typedef uint32_t 	u32;
typedef int32_t 	i32;
typedef uint64_t 	u64;
typedef int64_t 	i64;

typedef float  f32;
typedef double f64;

#include "str/strings.h"
#include "str/context_strings.h"

#include "platform_api.h"

struct code_context {
	string file;
	string function;
	i32 line = 0;
};

inline code_context make_context(string file, string function, i32 line);

#define CONTEXT make_context(string_literal(__FILE__), string_literal(__func__), __LINE__)

#include "alloc.h"
#include "math.h"

#include "ds/vector.h"
#include "ds/stack.h"
#include "ds/array.h"
#include "ds/queue.h"
#include "ds/map.h"
#include "ds/threadpool.h"

#include "log.h"
#include "game.h"

static game_state* global_state = NULL;

#include "str/strings.cpp"
#include "str/context_strings.cpp"

#include "alloc.cpp"
#include "log.cpp"

#include "ds/vector.cpp"
#include "ds/stack.cpp"
#include "ds/array.cpp"
#include "ds/queue.cpp"
#include "ds/map.cpp"
#include "ds/threadpool.cpp"

#include "everything.cpp"

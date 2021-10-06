#pragma once

#include <hls_stream.h>

#define CHUNK_SIZE 16

struct char16 {
	char pixels[CHUNK_SIZE];
};

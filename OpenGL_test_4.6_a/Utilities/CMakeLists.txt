set (sources
	"Misc.cpp"
	"Exception.cpp"
)

set (headers
	"Misc.h"
	"Exception.h"
	"UninitializedAllocator.h"
)
add_library(Utilities ${sources} ${headers})


#ifndef TAPESTRY
#define TAPESTRY
#include <A>

#define tapestry_schema(A,B,...) \
    i_prop(A,B, intern, string, install) \
    i_prop(A,B, intern, string, src)

#define tapestry_intern(A,B,...) tapestry_schema(A,B,__VA_ARGS__)

declare_class(tapestry)
#endif

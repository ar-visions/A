#ifndef TAPESTRY
#define TAPESTRY
#include <A>

#define tapestry_schema(X,Y,...) \
    i_prop(X,Y, intern, string, install) \
    i_prop(X,Y, intern, string, src)
#define tapestry_intern(X,Y,...) tapestry_schema(X,Y,__VA_ARGS__)
declare_class(tapestry)


#define dep_schema(X,Y,...) \
    i_prop(X,Y, public, string, name) \
    i_prop(X,Y, public, string, uri) \
    i_prop(X,Y, public, string, commit) \
    i_prop(X,Y, public, array, config, of, string) \
    i_override(X,Y, cast, string)
#define dep_intern(X,Y,...) dep_schema(X,Y,__VA_ARGS__)
declare_class(dep)


#define build_schema(X,Y,...) \
    i_ctr(X,Y,  public, path) \
    i_prop(X,Y, public, array, import, of, dep) \
    i_override(X,Y, method, init)
#define build_intern(X,Y,...) build_schema(X,Y,__VA_ARGS__)
declare_class(build)

#define line_schema(X,Y,...) \
    i_prop(X,Y, public, i32, indent) \
    i_prop(X,Y, public, array, tokens, of, string) \
    i_override(X,Y, method, init)
#define line_intern(X,Y,...) line_schema(X,Y,__VA_ARGS__)
declare_class(line)

#endif

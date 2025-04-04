#include <A-public>
#include <tapestry.h>
#include <A-init>

static path   src;
static path   install;
static path   cwd;

import 

int main(int argc, cstr argv[]) {
    A_start();
    path  default_src     = form  (path, "%s", getenv("SRC")     ? getenv("SRC")     : "");
    path  default_install = form  (path, "%s", getenv("INSTALL") ? getenv("INSTALL") : ".");
    map   args            = A_args(argc, argv, "install", default_install, "src", default_src, null);

    src     = get(args, string("src"));
    install = get(args, string("install"));
    cwd     = path_cwd(2048);
    print("src = %o, and install = %o, from path = %o", src, install, cwd);

    import im = import("import");
    
    // tapestry can be given any install base, with a default of . [self] 
    // useful to be relative in the file-system as we may promote stable versioning and hierarchy

    // this will make; we must simply install it, unless we can route 'make' into this.  clever if possible.
    // turn make args... into tapestry args... ; how hard can it be
    // so we write tapestry to build apps, and it should be 444 lines for everything
    return 0;
}
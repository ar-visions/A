# A-type runtime
A-type is a C-based object system designed for efficient reflection and robust interface clarity, balancing the flexibility of dynamic languages with the precision of systems programming. Use familiar OOP patterns in C with less boilerplate. Reflective API and meta types allow for sophisticated controls in compact code.  The result of that is scalable performance.  It's javascript meeting C and python in self expanding fashion.

<a href="https://github.com/ar-visions/A/actions/workflows/build.yml">
  <img src="https://github.com/ar-visions/A/actions/workflows/build.yml/badge.svg" alt="A-type build" width="444">
</a>

```c
#include <A>

int main(int argc, char **argv) {
    A_start();
    cstr        src = getenv("SRC");
    cstr     import = getenv("SILVER_IMPORT");
    map        args = A_args(argc, argv,
        "module",  str(""),
        "install", import ? form(path, "%s", import) : 
                            form(path, "%s/silver-import", src ? src : "."), null);
    string mkey     = str("module");
    string name     = get(args, str("module"));
    path   n        = new(path, chars, name->chars);
    path   source   = call(n, absolute);
    silver mod      = new(silver,
        source,  source,
        install, get(args, str("install")),
        name,    stem(source));
}

define_class (tokens)
define_mod   (silver, ether)
```

We have named arguments in our new() macro and that means 1 ctr/init to code not arbitrary amounts.  It's a far better and more productive pattern, and even more lean than Swift.  This is a post-init call, where we have already set the properties (where holds happen on non-primitives).  A-type standard destructor will also auto-drop member delegates that are object-based.  It just means you don't have to do much memory management at all.

The flattened macro function interface table is one you keep adding to, and they work across all of the objects that support those method names.  This, so you don't have to use the generic 'call' macro.  The large number of defines is actually fine and lets you use variables that clash with the names.  

Orbiter
an IDE being built with silver (was C++)
[https://github.com/ar-visions/orbiter.git]

Hyperspace
spatial dev kit, ai module & training scripts
[https://github.com/ar-visions/hyperspace.git]

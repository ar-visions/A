# A-type runtime
reflective types emitted directly into global initializers.
no lookups during load, and no meta emitted for intern types
serializing arguments to methods and expressing them for prototypes.

primitives use a protocol
for naming that allows them to be defined as prototypes with their types occupying another symbol of *_f (function table) and *_type (data name)
more compact than string output and doesnt require further processing.

a lazy loading regime is there for insuring ordered initialization.

A-type has the exact same schema as silver objects in terms of features
(proto, class, mod), intern and public.  as such we will be ABI compatible (there is just the issue of multiple context args in the silver spec)

public arguments are introspectable, and intern are only accessible inside the module.  we effectively partition the interns at the end of the struct, so that it may be design-compatible with users.


Orbiter
an IDE being built with silver (was C++)
[https://github.com/ar-visions/orbiter.git]

Hyperspace
spatial dev kit, ai module & training scripts
[https://github.com/ar-visions/hyperspace.git]

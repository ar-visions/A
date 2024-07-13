# A-type runtime
introspective types emitted directly into global initializers that do not require lookups.
serializing arguments to methods and expressing them for prototypes.

primitives use a protocol
for naming that allows them to be defined as prototypes with their types occupying another symbol of *_f (function table) and *_type (data name)
more compact than string output and doesnt require further processing.

a lazy loading regime is there for insuring ordered initialization.

A-type has the exact same schema as silver objects in terms of features
(proto, class, mod), intern and public

public arguments are introspectable as well as constructable with names.
there can be a 'property' idea that remains introspectable but not constructable by name.
for 1.0.0 we want it simple.


projects this applies to:

silver 0.44 implements the import keyword alone.  so with this we have ability to, build our C apps with A-type amongst all externals

this would need to select in your .c, how cute...


Orbiter
simple agent atm (not updated in months will update very very shortly)

[https://github.com/ar-visions/orbiter.git]

Hyperspace
spatial dev kit, ai module & training scripts at the moment no data.
the use-case here is go pro or xavier based dev.  it has cross compilation ability in the ion::dx build system

[https://github.com/ar-visions/hyperspace.git]

{ stdenv
, libxkbcommon
, meson
, ninja
, pkgconfig 
, yacc
}:

libxkbcommon.overrideAttrs({...}: {
  nativeBuildInputs = [ meson ninja pkgconfig yacc ];
  buildInputs = [ ];

  mesonFlags = [
    "-Denable-wayland=false"
    "-Denable-x11=false"
    "-Denable-docs=false"

    # This is because we're forcing uses of this build
    # to define config and locale root; for stage-1 use.
    # In stage-2, use the regular xkbcommon lib.
    "-Dxkb-config-root=/dev/null/etc/X11/xkb"
    "-Dx-locale-root=/dev/null/share/X11/locale"
  ];

  outputs = [ "out" "dev" ];

  # Ensures we don't get any stray dependencies.
  allowedReferences = [
    "out"
    "dev"
    stdenv.cc.libc_lib
  ];
})

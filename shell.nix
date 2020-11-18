(import <nixpkgs> {}).callPackage (
{ mkShell, callPackage, libevdev, xlibs }:
mkShell {
  buildInputs = [
    libevdev
    (callPackage ./xkbcommon.nix {})
  ];

  shellHook = ''
    export XKB_LOG_LEVEL=DEBUG
    export XKB_CONFIG_ROOT=${xlibs.xkeyboardconfig}/share/X11/xkb
    export XLOCALEDIR=${xlibs.libX11.out}/share/X11/locale
    run() {
      (
      PS4=" $ ";
      set -x
      $CC -g -o test test.c -lxkbcommon && ./test
      )
    }
  '';
}
) {}

(import <nixpkgs> {}).callPackage (
{ stdenv, callPackage, libxkbcommon }:

stdenv.mkDerivation {
  name = "xkbcommon-test";

  src = builtins.fetchGit ./.;

  buildInputs = [
    #(callPackage ./xkbcommon.nix {})
    libxkbcommon
  ];

  buildPhase = ''
    $CC -g -o test test.c -lxkbcommon
  '';

  installPhase = ''
    mv test $out
  '';
}
) {}

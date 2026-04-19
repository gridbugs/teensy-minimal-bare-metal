{
  pkgs ? import <nixpkgs> { },
}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    teensy-loader-cli
    gcc-arm-embedded
    picocom
    ctags
    python3
  ];
}

{
  description = "DLXEmu";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils = {
      url = "github:numtide/flake-utils";
    };
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        devShell = pkgs.mkShell {
          nativeBuildInputs = with pkgs; [
            pkg-config
            gcc14
          ];

          buildInputs = with pkgs; [
            # Tooling
            clang_18
            clang-tools_18
            cmake
            ninja
            python3

            # Dependencies
            libffi
            libGL
            libxkbcommon
            python3Packages.jinja2
            wayland
            xorg.libX11
            xorg.libXcursor
            xorg.libXi
            xorg.libXinerama
            xorg.libXrandr
          ];
        };
      }
    );
}

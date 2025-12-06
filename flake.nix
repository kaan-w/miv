{
  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
    flake-parts.url = "github:hercules-ci/flake-parts";
  };

  outputs = { flake-parts, ... }@inputs:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin" ];

      perSystem = { self', pkgs, lib, ... }: {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "miv";
          version = "0.1.1";
          src = lib.cleanSource ./.;

          nativeBuildInputs = with pkgs; [
            cmake
          ];

          buildInputs = with pkgs; [
            sdl3
            sdl3-image
          ];

          installPhase = ''
            mkdir -p $out/bin
            cp miv $out/bin/
          '';
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            sdl3
            sdl3-image
            self'.packages.default
          ];
        };
      };
    };
}
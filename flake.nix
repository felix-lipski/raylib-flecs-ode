{
  inputs = {
    nixpkgs.url = github:NixOS/nixpkgs;
    flake-utils.url = github:numtide/flake-utils;
    flecs = { url = github:SanderMertens/flecs; flake = false; };
  };

  outputs = { nixpkgs, self, flake-utils, ... }@inputs:
    flake-utils.lib.eachDefaultSystem
      (system:
      let 
        pkgs = import nixpkgs { config = { allowUnfree = false; }; inherit system; };
        buildInputs = with pkgs; [ raylib libGL xorg.libX11 gcc ]; 
        flecs = pkgs.pkgs.stdenv.mkDerivation rec {
          pname        = "flecs";
          version      = "2.4.8";
          src = ".";
          dontUnpack = true;
          inherit buildInputs;
          buildPhase = ''
            cp ${inputs.flecs}/flecs.c flecs.c
            cp ${inputs.flecs}/flecs.h flecs.h
            gcc -o libflecs.so -shared -I . flecs.c
            ls -la >> log
          '';
          installPhase = ''
            mkdir -p $out/lib $out/include
            cp libflecs.so $out/lib/libflecs.so
            cp flecs.h $out/include/flecs.h
            cp log $out/log
          '';
        };
      in {
        devShell = pkgs.mkShell { 
          # buildInputs = buildInputs ++ [ flecs ];
          buildInputs = buildInputs;

          shellHook = ''
            echo ${flecs}
          '';
        };
      }
      );
}

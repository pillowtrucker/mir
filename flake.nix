{
  description = "A template for Nix based C++ project setup.";

  inputs = {
    # Pointing to the current stable release of nixpkgs. You can
    # customize this to point to an older version or unstable if you
    # like everything shining.
    #
    # E.g.
    #
    # nixpkgs.url = "github:NixOS/nixpkgs/unstable";

    nixpkgs.url = "github:NixOS/nixpkgs/master";

    utils.url = "github:numtide/flake-utils";

  };

  outputs = { self, nixpkgs, ... }@inputs:
    inputs.utils.lib.eachSystem [
      # Add the system/architecture you would like to support here. Note that not
      # all packages in the official nixpkgs support all platforms.
      "x86_64-linux"
      "i686-linux"
      "aarch64-linux"
      "x86_64-darwin"
    ] (system:
      let
        pkgs = import nixpkgs {
          inherit system;

          # Add overlays here if you need to override the nixpkgs
          # official packages.
          overlays = [
            (final: prev: { llvmPackages = prev.llvmPackages_18; })

          ];

          # Uncomment this if you need unfree software (e.g. cuda) for
          # your project.
          #
          config.allowUnfree = true;
        };
      in {
        #        devShells.default = pkgs.llvmPackages_18.libcxxStdenv.mkDerivation
        devShells.default = pkgs.llvmPackages_18.stdenv.mkDerivation rec {
          # Update the name to something that suites your project.
          name = "wonkyhonky_tooting_shell";
          stdenv = pkgs.llvmPackages_18.stdenv;
          packages = with pkgs; [
            # Development Tools
            #            llvmPackages_18.clang
            (clang-tools.override { llvmPackages = llvmPackages_18; })
            llvmPackages_18.bintools
            git
            #            llvmPackages_18.libraries.libcxx
            #            llvmPackages_18.libcxx
            #            llvmPackages_18.compiler-rt
            cmake
            cmakeCurses
            ninja

            boost
            egl-wayland
            freetype
            glib
            glm
            glog
            libdrm
            libepoxy
            libevdev
            libglvnd
            libinput
            libuuid
            xorg.libxcb
            libxkbcommon
            (libxmlxx.overrideAttrs {
              version = "2.42.3";
              configureFlags = [ ];
            })
            yaml-cpp
            lttng-ust
            mesa
            nettle
            udev
            wayland
            xorg.libX11
            xorg.libXcursor
            xorg.xorgproto
            xwayland
            glib # gdbus-codegen
            lttng-ust # lttng-gen-tp
            pkg-config
            (python3.withPackages
              (ps: with ps; [ pillow ] ++ [ pygobject3 python-dbusmock ]))
            validatePkgConfig
            dbus
            gtest
            umockdev
            wlcs
          ];
          nativeBuildInputs = packages;
          # Setting up the environment variables you need during
          # development.
          #      shellHook = let
          #        icon = "f121";
          #      in ''
          #        export PS1="$(echo -e '\u${icon}') {\[$(tput sgr0)\]\[\033[38;5;228m\]\w\[$(tput sgr0)\]\[\033[38;5;15m\]} (${name}) \\$ \[$(tput sgr0)\]"
          #      '';
        };

        packages.default = pkgs.callPackage ./default.nix {
          #          stdenv = pkgs.llvmPackages_18.libcxxStdenv;
        };
      });
}

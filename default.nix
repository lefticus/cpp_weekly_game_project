let
 pkgs = import <nixos> {};
 unstable = import <nixos-unstable> {};

 inherit (unstable) stdenv;

 build_settings = unstable.gcc10Stdenv.mkDerivation {
    name = "build_settings";

    nativeBuildInputs = with pkgs; [
	# Tools
	pkgconfig unstable.cmake unstable.ninja
	binutils gnumake libtool automake autoconf gnum4
    ];

    propagatedBuildInputs = [
	# Libraries
	pkgs.libGL.dev pkgs.systemd.dev
    ];

    hardeningDisable = [ "format" ];
  };
in build_settings

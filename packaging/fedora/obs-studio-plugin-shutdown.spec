Name: obs-studio-plugin-shutdown
Version: @VERSION@
Release: @RELEASE@%{?dist}
Summary: Shutdown API plugin for OBS Studio
License: GPLv2+

Source0: %{name}-%{version}.tar.bz2
BuildRequires: cmake, gcc, gcc-c++
BuildRequires: obs-studio-devel
BuildRequires: qt6-qtbase-devel qt6-qtbase-private-devel

%description
This is a simple plugin for OBS Studio to shutdown OBS Studio requested by websocket.
Note: The 'vendorName' is modified in this release. Please see script below.
  %{_docdir}/obs-studio-plugin-shutdown/shutdown.py

%prep
%autosetup -p1
sed -i -e 's/project(obs-/project(/g' CMakeLists.txt
sed -i -e 's/.obs-shutdown-plugin.,$/'\''shutdown-plugin'\'', # Removed "obs-" /' example/shutdown.py

%build
%{cmake} -DLINUX_RPATH=OFF -DQT_VERSION=6 -DINSTALL_LICENSE_FILES:BOOL=OFF \
  -DVERSION_DESCRIPTION_OVERRIDE:STRING='%{version}-%{release}'
%{cmake_build}

%install
%{cmake_install}

%files
%{_libdir}/obs-plugins/*.so
%{_datadir}/obs/obs-plugins/*/
%license LICENSE
%doc example/shutdown.py

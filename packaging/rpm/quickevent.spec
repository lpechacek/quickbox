#
# spec file for package quickevent
#
# Copyright (c) 2019 SUSE LINUX GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via https://bugs.opensuse.org/
#


Name:           quickevent
Version:        2.2.4
Release:        0
Summary:        Orienteering event management program
License:        GPL-2.0-only
Group:          Productivity/Other
URL:            https://github.com/fvacek/quickbox
Source:         https://github.com/fvacek/quickbox/archive/QE-v2.2.4-beta.tar.gz
BuildRequires:  libqt5-linguist
BuildRequires:  pkgconfig
BuildRequires:  rsync
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5Gui)
BuildRequires:  pkgconfig(Qt5Multimedia)
BuildRequires:  pkgconfig(Qt5Network)
BuildRequires:  pkgconfig(Qt5PrintSupport)
BuildRequires:  pkgconfig(Qt5Qml)
BuildRequires:  pkgconfig(Qt5Script)
BuildRequires:  pkgconfig(Qt5SerialPort)
BuildRequires:  pkgconfig(Qt5Sql)
BuildRequires:  pkgconfig(Qt5Svg)
BuildRequires:  pkgconfig(Qt5Widgets)
BuildRequires:  pkgconfig(Qt5Xml)
#Patch0:         suse-build.patch
BuildRequires:  pkgconfig(fuse)
Recommends:     libQt5Sql5-postgresql
Recommends:     libQt5Sql5-sqlite

%description
Orienteering event management program with close relationship with ORIS
database.  Suitable for organizing single and multi-day orienteering events.

%prep
%setup -q -n quickbox-QE-v2.2.4-beta
#%patch0 -p1

%build
%qmake5
%make_jobs

%install
%qmake5_install
install -d %{buildroot}/%{_datadir}/%{name}/translations
devutils/lctools/lctool.sh --qt-bin-dir %_libqt5_bindir -s . release %{buildroot}/%{_datadir}/%{name}/translations

%post
%postun

%files
%{_libdir}/quickevent/*
%{_bindir}/*
%{_datadir}/quickevent/*
%license LICENSE
%doc README.md

%changelog

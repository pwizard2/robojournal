Summary:            Free journal software for everyone
Name:               robojournal
Version:            0.4.1
Release:            1%{?dist}
License:            GPLv3
Group:              Applications/Productivity
Source:             http://sourceforge.net/projects/robojournal/files/Source/%{name}-%{version}.tar.gz
URL:                http://sourceforge.net/projects/robojournal

BuildRequires:      qt, qt-assistant, qt-mysql, qt-devel, qt-webkit, qt-webkit-devel, patch
Requires:           qt, qt-assistant, qt-mysql, qt-webkit

%description

RoboJournal is a cross-platform journal/diary tool written in Qt/C++.
Right now, RoboJournal only supports MySQL but support for SQLite 
(and possibly Postgres) will be added in future releases. RoboJournal 
runs on Windows and Linux.

%prep

%setup -q -n %{name}-%{version}

%build

# 5/21/13: Patch project file to prevent documentation from being built (it should be built
# only while making the robojournal-doc package).
patch robojournal.pro < pkg/package-config.patch
qmake-qt4 CONFIG+=package robojournal.pro

# Apply standard Fedora patch so the app compiles properly
patch Makefile < fedora_build.patch

make 
strip robojournal

%install

#install files manually because "make install" doesn't work with rpmbuild in this case.

# create directory tree in buildroot:
mkdir -p %{buildroot}%{_bindir}/
mkdir -p %{buildroot}%{_datadir}/
mkdir -p %{buildroot}%{_datadir}/applications/
mkdir -p %{buildroot}%{_datadir}/icons/
mkdir -p %{buildroot}%{_datadir}/menu/
mkdir -p %{buildroot}%{_datadir}/pixmaps/
mkdir -p %{buildroot}%{_mandir}/man7

# install the files where they need to go
cp -p robojournal %{buildroot}%{_bindir}/
cp -p robojournal64.png %{buildroot}%{_datadir}/icons/
cp -p %{_builddir}/%{buildsubdir}/menus/robojournal.desktop %{buildroot}%{_datadir}/applications/
cp -p %{_builddir}/%{buildsubdir}/menus/robojournal %{buildroot}%{_datadir}/menu/
cp -p %{_builddir}/%{buildsubdir}/menus/robojournal.xpm %{buildroot}%{_datadir}/pixmaps/
cp -p %{_builddir}/%{buildsubdir}/robojournal.7.gz %{buildroot}%{_mandir}/man7/

%post
mandb -p

%clean

make distclean

%files

%{_bindir}/robojournal
%{_datadir}/applications/robojournal.desktop
%{_datadir}/icons/robojournal64.png
%{_datadir}/pixmaps/robojournal.xpm
%{_datadir}/menu/robojournal
%{_mandir}/man7/robojournal.7.gz


%changelog
* Thu Apr 25 2013 Will Kraft <pwizard@gmail.com> 0.4.1-1
- Initial release.
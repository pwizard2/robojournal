Summary:            Keep a journal/diary of day-to-day events in your life
Name:               robojournal
Version:            0.4.2
Release:            1%{?dist}
License:            GPLv3
Group:              Applications/Productivity
Source:             http://sourceforge.net/projects/robojournal/files/Source/%{name}-%{version}.tar.gz
URL:                http://sourceforge.net/projects/robojournal

BuildRequires:      qt-mysql, qt-devel, qt-webkit-devel, desktop-file-utils
Requires:           qt, qt-mysql, qt-webkit

%description

RoboJournal is a cross-platform journal/diary tool written in Qt/C++.
Right now, RoboJournal only supports MySQL but support for SQLite 
(and possibly Postgres) will be added in future releases. RoboJournal 
runs on Windows and Linux.

%prep

%setup -q -n %{name}-%{version}

# Update 6/17/13: Patch the project file to prevent the documentation from being built. That should only
# happen when building the robojournal-doc package.
patch robojournal.pro < pkg/package-config.patch

# UPDATE 6/17/13: validate the desktop file.
desktop-file-validate %{_builddir}/%{name}-%{version}/menus/robojournal.desktop

%build

# 6/1/13: Removed Fedora build patch since it is no longer needed.

qmake-qt4 CONFIG+=package robojournal.pro
make %{?_smp_mflags}

%install

# install files manually because the rpm build process fails when "make install" is used. 

# create directory tree in buildroot:
mkdir -p %{buildroot}%{_bindir}/
mkdir -p %{buildroot}%{_datadir}/
mkdir -p %{buildroot}%{_datadir}/applications/
mkdir -p %{buildroot}%{_datadir}/icons/
mkdir -p %{buildroot}%{_mandir}/man7

# install the files where they need to go
cp -p robojournal %{buildroot}%{_bindir}/
cp -p robojournal64.png %{buildroot}%{_datadir}/icons/

# UPDATE 6/17/13: Use desktop-file-install to process .desktop file in accordance with Fedora requirements.
desktop-file-install                                    \
--dir=${RPM_BUILD_ROOT}%{_datadir}/applications         \
%{_builddir}/%{buildsubdir}/menus/robojournal.desktop

cp -p %{_builddir}/%{buildsubdir}/robojournal.7* %{buildroot}%{_mandir}/man7/

%files

%{_bindir}/robojournal
%{_datadir}/applications/robojournal.desktop
%{_datadir}/icons/robojournal64.png
%{_mandir}/man7/robojournal.7*

%changelog

* Fri Jul 5 2013 Will Kraft <pwizard@gmail.com> 0.4.2-1
- Initial release.


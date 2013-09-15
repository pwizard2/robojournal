Summary:            Keep a journal/diary of day-to-day events in your life
Name:               robojournal
Version:            0.4.2
Release:            1%{?dist}
License:            GPLv3
Group:              Applications/Productivity
Source:             http://sourceforge.net/projects/robojournal/files/Source/%{name}-%{version}.tar.gz
URL:                http://sourceforge.net/projects/robojournal

BuildRequires:      qt-mysql%{?_isa} >= 1:4.7.4-1, qt-devel%{?_isa} >= 1:4.7.4-1, qtwebkit-devel%{?_isa} >= 2.2.2-8, desktop-file-utils, perl
Requires:           qt%{?_isa} >= 1:4.7.4-1, qt-mysql%{?_isa} >= 1:4.7.4-1, qtwebkit%{?_isa} >= 2.2.2-8

%description

RoboJournal is a cross-platform journal/diary tool written in Qt/C++.
Right now, RoboJournal only supports MySQL but support for SQLite 
(and possibly Postgres) will be added in future releases. RoboJournal 
runs on Windows and Linux.

%prep

%setup -q -n %{name}-%{version}

# UPDATE 6/17/13: validate the desktop file.
desktop-file-validate %{_builddir}/%{name}-%{version}/menus/robojournal.desktop

# Patch robojournal.pro to remove install instructions for Debian menu items we obviously don't need on Fedora.
# This is necessary b/c rpmbuild fails if we leave them in the makefile --Will Kraft (9/2/13).
patch %{_builddir}/%{name}-%{version}/robojournal.pro < %{_builddir}/%{name}-%{version}/pkg/fedora-rpmbuild.patch

%build

qmake-qt4 CONFIG+=package robojournal.pro
make %{?_smp_mflags}

%install
  make INSTALL_ROOT=%{buildroot} install

# UPDATE 6/17/13: Use desktop-file-install to process .desktop file in accordance with Fedora requirements.
desktop-file-install                                    \
--dir=${RPM_BUILD_ROOT}%{_datadir}/applications         \
%{_builddir}/%{buildsubdir}/menus/robojournal.desktop

# UPDATE 9/2/13: install documentation by hand. 
mkdir ${RPM_BUILD_ROOT}%{_datadir}/doc
mkdir ${RPM_BUILD_ROOT}%{_datadir}/doc/robojournal
cp %{_builddir}/%{buildsubdir}/doc/robojournal.qhc ${RPM_BUILD_ROOT}%{_datadir}/doc/robojournal/robojournal.qhc
cp %{_builddir}/%{buildsubdir}/doc/robojournal.qch ${RPM_BUILD_ROOT}%{_datadir}/doc/robojournal/robojournal.qch

%files

%{_bindir}/robojournal
%{_datadir}/applications/robojournal.desktop
%{_datadir}/icons/robojournal64.png
%{_mandir}/man7/robojournal.7*

%changelog

* Sat Sep 14 2013 Will Kraft <pwizard@gmail.com> 0.4.2-1
- Initial release.

######################################################################################################################
# RoboJournal Documentation Package SPEC (9/2/13)

%package doc
BuildArch: noarch
Summary: Documentation files for RoboJournal
Requires: qt-assistant, robojournal

%description doc
Documentation (compiled help file and collection file) for RoboJournal %{version}.

%files doc
%{_defaultdocdir}/robojournal/robojournal.qhc
%{_defaultdocdir}/robojournal/robojournal.qch
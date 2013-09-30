Summary:            Keep a journal/diary of day-to-day events in your life
Name:               robojournal
Version:            0.5
Release:            1%{?dist}
License:            GPLv3
Group:              Applications/Productivity
Source:             http://sourceforge.net/projects/robojournal/files/Source/%{name}-%{version}.tar.gz
URL:                http://sourceforge.net/projects/robojournal

BuildRequires:      qt-mysql, qt-devel, qtwebkit-devel, desktop-file-utils, perl

%description

RoboJournal is a cross-platform journal/diary tool written in Qt/C++.
Right now, RoboJournal only supports MySQL but support for SQLite 
(and possibly Postgres) will be added in future releases. RoboJournal 
runs on Windows and Linux.

######################################################################################################################
# RoboJournal Documentation Package SPEC (9/2/13)

%package doc
BuildArch: noarch
Summary: Documentation files for RoboJournal
Requires: qt-assistant, robojournal

%description doc
Documentation (compiled help file and collection file) for RoboJournal %{version}.

%files doc
%{_docdir}/robojournal/robojournal.qhc
%{_docdir}/robojournal/robojournal.qch

%dir %{_docdir}/robojournal/

######################################################################################################################

%prep

%setup -q 

%build

# Using the QMake macro requires Fedora >= 18 (all Qt packages on f18 *must* be up-to-date). If you have to package on an
# older system, un-comment line #49 and comment out line #50.
#qmake-qt4 CONFIG+=package robojournal.pro 
%{qmake_qt4} CONFIG+=package robojournal.pro

make %{?_smp_mflags}

%install

# Install the app to the rpm build directory.
make INSTALL_ROOT=$RPM_BUILD_ROOT install

# Install desktop file.
desktop-file-install                                    \
--dir=${RPM_BUILD_ROOT}%{_datadir}/applications         \
menus/robojournal.desktop

# Install documentation by hand (9/2/13).
mkdir $RPM_BUILD_ROOT%{_docdir}
mkdir $RPM_BUILD_ROOT%{_docdir}/robojournal
cp doc/robojournal.qhc $RPM_BUILD_ROOT%{_docdir}/robojournal/robojournal.qhc
cp doc/robojournal.qch $RPM_BUILD_ROOT%{_docdir}/robojournal/robojournal.qch

# Delete Debian menu entry and xpm icon file because they are completely unnecessary on Fedora. This is an easier,
# more reliable solution than using a patch to keep these items from being installed in the first place. (9/30/13).
rm -f ${RPM_BUILD_ROOT}%{_datadir}/menu/robojournal
rm -f ${RPM_BUILD_ROOT}%{_datadir}/pixmaps/robojournal.xpm

%files

%{_bindir}/robojournal
%{_datadir}/applications/robojournal.desktop
%{_datadir}/icons/robojournal64.png
%{_mandir}/man7/robojournal.7*


%changelog

* Mon Sep 30 2013 Will Kraft <pwizard@gmail.com> 0.4.2-1
- Initial release.
Summary:            Free journal software for everyone
Name:               robojournal
Version:            0.4.1
Release:            1%{?dist}
License:            GPLv3
Group:              Applications/Productivity
Source:             http://sourceforge.net/projects/robojournal/files/Source/%{name}-%{version}.tar.gz
URL:                http://sourceforge.net/projects/robojournal
BuildRequires:      qt, qt-assistant, qt-mysql, qt-devel, qt-webkit, qt-webkit-devel, patch
Requires:           qt, qt-assistant, qt-mysql, qt-devel, qt-webkit, qt-webkit-devel

%description

RoboJournal is a cross-platform journal/diary tool written in Qt/C++.
Right now, RoboJournal only supports MySQL but support for SQLite 
(and possibly Postgres) will be added in future releases. RoboJournal 
runs on Windows and Linux.

%prep

%setup -q -n %{name}-%{version}

%build

qmake-qt4 CONFIG+=package robojournal.pro
patch Makefile < fedora_build.patch
make 
qcollectiongenerator doc/robojournal.qhcp -o doc/robojournal.qhc

%install

rm -rf %{buildroot}
make DESTDIR=%{buildroot} install

%clean

make clean

%files

%{_bindir}/robojournal
%{_datadir}/applications/robojournal.destop
%{_datadir}/icons/robojournal64.png
%{_datadir}/pixmaps/robojournal.xpm
%{_datadir}/menu/robojournal
%{_defaultdocdir}/%{name}/robojournal.qch
%{_defaultdocdir}/%{name}/robojournal.qhc

%changelog
* Thu Apr 25 2013 Will Kraft <pwizard@gmail.com>.
- Version 0.4.1
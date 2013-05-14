Name:		robojournal-doc		
Version:	0.4.1
Release:	1%{?dist}
Summary:	Documentation Files for RoboJournal %{version}

Group:		Applications/Productivity
License:	GPLv3+	
URL:		http://sourceforge.net/projects/robojournal
Source0:	http://sourceforge.net/projects/robojournal/files/Source/robojournal-%{version}.tar.gz

BuildArch:	noarch
BuildRequires:	qt-devel
Requires:	qt-assistant, robojournal	

%description

Documentation (compiled help file and collection file) for RoboJournal %{version}. 

%prep
tar -xvf %{_sourcedir}/robojournal-%{version}.tar.gz 

%build

qcollectiongenerator %{_builddir}/robojournal-%{version}/doc/robojournal.qhcp -o %{_builddir}/robojournal-%{version}/doc/robojournal.qhc


%install

mkdir -p %{buildroot}%{_defaultdocdir}/
mkdir -p %{buildroot}%{_defaultdocdir}/robojournal
cp -p %{_builddir}/robojournal-%{version}/doc/robojournal.qhc %{buildroot}%{_defaultdocdir}/robojournal
cp -p %{_builddir}/robojournal-%{version}/doc/robojournal.qch %{buildroot}%{_defaultdocdir}/robojournal

%files

%{_defaultdocdir}/robojournal/robojournal.qhc
%{_defaultdocdir}/robojournal/robojournal.qch


%changelog
* Tue May 14 2013 Will Kraft <pwizard@gmail.com>.
- Version 0.4.1

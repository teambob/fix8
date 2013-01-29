Summary: Foobar
Name: fix8
Version: 0.6.6
Release: 0
License: GPL
Group: Productivity/Networking
Source0: http://somewhere/Foobar/%{name}-%{version}.tar.gz
URL: http://somewhere
BuildRoot: %{_tmppath}/%{name}-root
Prefix: %{_prefix}

%description
Foobar does something

#%prep
%setup

#%build
%configure
make

#%install
%makeinstall

#%clean
#rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)

#%doc AUTHORS BUGS COPYING INSTALL NEWS README

%changelog


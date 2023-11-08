.rc.lo:
			$(LIBTOOL) --tag=RC --mode=compile $(WX_RESCOMP) $< -o $@

pkgdata_DATA = \
			ProjectExporter.zip

CLEANFILES = \
			$(pkgdata_DATA)

AM_CPPFLAGS = \
			$(WX_CXXFLAGS) \
			-I$(top_srcdir)/src/include \
			-I$(top_srcdir)/src/include/tinyxml \
			-I$(top_srcdir)/src/sdk/wxscintilla/include

if CODEBLOCKS_NT
AM_CXXFLAGS = \
			-DBUILDING_PLUGIN \
			-DCC_NO_COLLAPSE_ITEM \
			-DLOGGING \
			-DDONT_SHOW_SERVER_CONSOLE
			-m64 \
			-pipe \
			-fmessage-length=0 \
			-fexceptions \
			-fno-keep-inline-dllexport \
			-DHAVE_W32API_H \
			-D__WXMSW__ \
			-DWXUSINGDLL \
			-DcbDEBUG \
			-DwxUSE_UNICODE \
			-D_WIN64
endif

if CODEBLOCKS_NT_BUILD_INSTALLER
pluginlibdir = $(pkgdatadir)/plugins
else
pluginlibdir = $(pkglibdir)/plugins
endif

pluginlib_LTLIBRARIES = \
			libProjectExporter.la

libProjectExporter_la_LDFLAGS = \
			@MODULE_SHARED_LDFLAGS@ \
			-no-undefined \
			-avoid-version

if CODEBLOCKS_NT
libProjectExporter_la_LDFLAGS += \
			-enable-auto-image-base \
			-add-stdcall-alias \
			-enable-auto-import \
			-no-undefined
endif

libProjectExporter_la_LIBADD = \
			$(top_srcdir)/src/sdk/libcodeblocks.la \
			$(WX_LIBS)

if !CODEBLOCKS_NT
libProjectExporter_la_LIBADD += \
			$(CB_TINYXML_LIBS)
endif

libProjectExporter_la_SOURCES = \
			Exporter_AutoTools/AutotoolsExporter.cpp \
			Exporter_BakeFile/BakefileExporter.cpp \
			Exporter_CMake/CMakeListsExporter.cpp \
			Exporter_PreMake4/Premake4Dlg.cpp \
			Exporter_PreMake4/Premake4Exporter.cpp \
			Exporter_PreMake5/pm_base.cpp \
			Exporter_PreMake5/pm_config.cpp \
			Exporter_PreMake5/pm_config_cb.cpp \
			Exporter_PreMake5/pm_defaults.cpp \
			Exporter_PreMake5/pm_file.cpp \
			Exporter_PreMake5/pm_file_cb.cpp \
			Exporter_PreMake5/pm_project.cpp \
			Exporter_PreMake5/pm_project_cb.cpp \
			Exporter_PreMake5/pm_regex.cpp \
			Exporter_PreMake5/pm_settings.cpp \
			Exporter_PreMake5/pm_utils.cpp \
			Exporter_PreMake5/pm_workspace.cpp \
			Exporter_PreMake5/pm_workspace_cb.cpp \
			Exporter_PreMake5/PM5SettingsDialog.cpp \
			Exporter_PreMake5/premake5cb.cpp \
			ExporterBase.cpp \
			ProjectExporter.cpp

noinst_HEADERS = \
			Exporter_AutoTools/AutotoolsExporter.h \
			Exporter_BakeFile/BakefileExporter.h \
			Exporter_CMake/CMakeListsExporter.h \
			Exporter_PreMake4/Premake4Dlg.h \
			Exporter_PreMake4/Premake4Exporter.h \
			Exporter_PreMake5/pm_base.h \
			Exporter_PreMake5/pm_config.h \
			Exporter_PreMake5/pm_config_cb.h \
			Exporter_PreMake5/pm_defaults.h \
			Exporter_PreMake5/pm_file.h \
			Exporter_PreMake5/pm_file_cb.h \
			Exporter_PreMake5/pm_project.h \
			Exporter_PreMake5/pm_project_cb.h \
			Exporter_PreMake5/pm_regex.h \
			Exporter_PreMake5/pm_settings.h \
			Exporter_PreMake5/pm_utils.h \
			Exporter_PreMake5/pm_workspace.h \
			Exporter_PreMake5/pm_workspace_cb.h \
			Exporter_PreMake5/PM5SettingsDialog.h \
			Exporter_PreMake5/premake5cb.h \
			ExporterBase.h \
			ProjectExporter.h

resources_forZIP = \
			manifest.xml \
			Exporter_PreMake4/Premake4Dlg.xrc

EXTRA_DIST = \
			$(resources_forZIP)

ProjectExporter.zip: $(resources_forZIP)
			PWD=`pwd` cd $(srcdir) && zip -jq9 $(PWD)/ProjectExporter.zip $(resources_forZIP) > /dev/null

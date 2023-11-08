#include <sdk.h> // Code::Blocks SDK
#include <wx/textfile.h>
#include "AutotoolsExporter.h"

AutotoolsExporter::AutotoolsExporter()
{
    //ctor
    m_project = Manager::Get()->GetProjectManager()->GetActiveProject();
}

AutotoolsExporter::~AutotoolsExporter()
{
    //dtor
}

void AutotoolsExporter::RunExport()
{
    CreateConfig();
    CreateMake();
    Manager::Get()->GetLogManager()->Log(m_configure);
    Manager::Get()->GetLogManager()->Log(m_makefile);
    /*wxArrayString includesArray = IncludeFilesArray();
    for(unsigned int i = 0; i < includesArray.GetCount(); i++)
    {
        Manager::Get()->GetLogManager()->Log(wxT("<") + includesArray[i] + wxT(">"));
    }*/
    //Manager::Get()->GetFileManager()->Save((m_project->GetBasePath() + wxT("configure.ac")), m_configure, wxFONTENCODING_SYSTEM, true);
    //Manager::Get()->GetFileManager()->Save((m_project->GetBasePath() + wxT("makefile.am")), m_makefile, wxFONTENCODING_SYSTEM, true);
}

void AutotoolsExporter::CreateConfig()
{
    m_configure = wxT("AC_PREREQ([2.64])\n"
                      "AC_INIT([") + m_project->GetTitle() + wxT("], [1.0])\n"
                      "AM_INIT_AUTOMAKE\n\n");
    wxString bufferString;
    bool useC = false;
    bool useCxx = false;
    for(FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile* pf = *i;
        wxString ext = pf->file.GetExt().Lower();
        if(ext == wxT("c"))
        {
            useC = true;
            bufferString = pf->relativeFilename;
            if(ext == m_project->GetTitle())
            {
                break;
            }
        }
        else if(ext == wxT("cpp") ||
                ext == wxT("cc")  ||
                ext == wxT("cxx")  )
        {
            useCxx = true;
            bufferString = pf->relativeFilename;
            if(pf->file.GetName() == m_project->GetTitle())
            {
                break;
            }
        }
    }
    m_configure += wxT("AC_CONFIG_SRCDIR([") + ConvertSlash(bufferString) + wxT("])\n");
    bufferString = wxFileName((*m_project->GetFilesList().begin())->relativeFilename).GetPath(wxPATH_GET_SEPARATOR, wxPATH_UNIX);
    for(FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile* pf = *i;
        if(FileTypeOf(pf->relativeFilename) == ftHeader)
        {
            wxString headerPath = wxFileName(pf->relativeFilename).GetPath(wxPATH_GET_SEPARATOR, wxPATH_UNIX);
            if(headerPath.IsSameAs(wxT("include/"), false))
            {
                bufferString = headerPath;
                break;
            }
            if(headerPath.Length() < bufferString.Length())
            {
                bufferString = headerPath;
            }
            if(bufferString.IsEmpty())
            {
                break;
            }
        }
    }
    m_configure   +=   wxT("AC_CONFIG_HEADERS([") + bufferString + wxT("config.h])\n"
                           "AC_CONFIG_FILES([Makefile])\n\n");
    if(!useC)
    {
        m_configure += wxT("AC_LANG([C++])\n\n");
    }
    wxTextFile file(m_project->GetCommonTopLevelPath() + wxT("/configure.scan"));
    wxArrayString (macros)[5];
    if(file.Open())
    {
        int fileSection = -1;
        for(unsigned int i = 0; i < file.GetLineCount(); i++)
        {
            bufferString = file.GetLine(i);
            if(bufferString    ==   wxT("# Checks for programs."))
            {
                fileSection = 0;
            }
            else if(bufferString == wxT("# Checks for libraries."))
            {
                fileSection = 1;
            }
            else if(bufferString == wxT("# Checks for header files."))
            {
                fileSection = 2;
            }
            else if(bufferString == wxT("# Checks for typedefs, structures, and compiler characteristics."))
            {
                fileSection = 3;
            }
            else if(bufferString == wxT("# Checks for library functions."))
            {
                fileSection = 4;
            }
            else if(bufferString.IsEmpty())
            {
                if(fileSection == 4)
                {
                    break;
                }
            }
            else if(fileSection != -1)
            {
                macros[fileSection].Add(bufferString);
            }
        }
    }
    else
    {
        wxString s = wxT("AC_PROG_C");if(useC){s+=wxT("C\n");if(useCxx){s+=
                     wxT("AC_PROG_CXX\n\n");}else{s+=wxT("\n");}}else{s+=wxT("XX\n\n");};
        m_configure += s +
                     wxT("# Warning: \"configure.scan\" not found; this file is required\n"
                         "#          to catch a greater number of necessary macros.\n"
                         "#          Please run \"autoscan\" to generate this file.\n\n");
    }
    IdentifyLibs();/*********************************************/
    for(int i = 0; i < 5; i++)
    {
        bufferString = GetStringFromArray(macros[i], wxT("\n"), false);
        if(!bufferString.IsEmpty())
        {
            m_configure += bufferString + wxT("\n\n");
        }
    }
    m_configure  +=  wxT("AC_OUTPUT\n\n"
                         "echo \\\n"
                        "\"-------------------------------------------------\n"
                         "    ${PACKAGE_NAME}   Version ${PACKAGE_VERSION}\n\n"
                         " Prefix:   ${prefix}\n");{wxString s=wxT(
                         " Compiler: ${C");if(useC){s+=wxT("C} ${C");if(useCxx){s+=wxT("FLAGS} ${CPPFLAGS}\n"
                         "           ${CXX} ${CXX");}}else{s+=wxT("XX} ${CXX");}s+=wxT("FLAGS} ${CPPFLAGS}\n");m_configure+=s+wxT(
                         "-------------------------------------------------\n"
                         "Configuration is complete; enter 'make' to build\"");}
}

void AutotoolsExporter::CreateMake()
{
    m_makefile.Clear();
    for(int i=0; i < m_project->GetBuildTargetsCount(); i++)
    {
        if(!m_makefile.IsEmpty())
        {
            m_makefile += wxT("\n\n");
        }
        wxString name = RemLib(wxFileName(m_project->GetBuildTarget(i)->GetOutputFilename()).GetName());
        switch(m_project->GetBuildTarget(i)->GetTargetType())
        {
            case ttExecutable:
            case ttConsoleOnly:
            {
                m_makefile += wxT("bin_PROGRAMS = ") + name + wxT("\n") +
                                   name + wxT("_SOURCES =");
                for(FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
                {
                    ProjectFile* pf = *j;
                    wxString cfn(pf->relativeFilename);
                    if(cfn.Right(4) == wxT(".cpp") || cfn.Right(2) == wxT(".c") || cfn.Right(3) == wxT(".cc") || cfn.Right(4) == wxT(".cxx"))
                    {
                        if(pf->compile)
                        {
                            m_makefile += wxT(" ") + ConvertSlash(cfn);
                        }
                    }
                }
                /*wxArrayString tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
                if(!tmpArray.IsEmpty())
                {
                    m_makefile += wxT("_LDADD =")
                    for(unsigned int j=0; j < tmpArray.GetCount(); j++)
                    {
                        ConvertSlash(tmpArray[j]);
                    }
                }*/
            }
            default:;
        }
    }
}

wxArrayString AutotoolsExporter::IdentifyLibs()
{
    wxArrayString output;
    bool useWx = false;
    int wxVer = 28;
    bool useBoost[41];
    for(int i = 0; i < 41; i++)
    {
        useBoost[i] = false;
    }
    wxString boostVer = wxT("1.48.0");
    wxArrayString tmpArray = IncludeFilesArray();
    for(unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner(wxT("^wx/.*"));
        if(scanner.Matches(tmpArray[i]))
        {
            useWx = true; continue;
        }
        scanner.Compile(wxT("^boost/.*hpp"));
        if(scanner.Matches(tmpArray[i]))
        {
            useBoost[0] = true;
            if(tmpArray[i] == wxT("boost/array.hpp"))
            {
                useBoost[1] = true; continue;
            }
            scanner.Compile(wxT("^boost/asio.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[2] = true; continue;
            }
            scanner.Compile(wxT("^boost/bind.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[3] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/cast.hpp") ||      //Conversion
               tmpArray[i] == wxT("boost/lexical_cast.hpp"))
            {
                useBoost[4] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/foreach.hpp"))
            {
                useBoost[5] = true; continue;
            }
            scanner.Compile(wxT("^boost/format.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[6] = true; continue;
            }
            scanner.Compile(wxT("^boost/function[^a]*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[7] = true; continue;
            }
            scanner.Compile(wxT("^boost/functional.*hash.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[8] = true; continue;
            }
            scanner.Compile(wxT("^boost/lambda/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[9] = true; continue;
            }
            scanner.Compile(wxT("^boost/math.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[10] = true; continue;
            }
            scanner.Compile(wxT("^boost/multi_array.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[11] = true; continue;
            }
            scanner.Compile(wxT("^boost/numeric/conversion.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[12] = true; continue;
            }
            scanner.Compile(wxT("^boost/optional.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[13] = true; continue;
            }
            scanner.Compile(wxT("^boost/preprocessor.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[14] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/ref.hpp"))
            {
                useBoost[15] = true; continue;
            }
            scanner.Compile(wxT("^boost/smart_ptr.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[16] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/static_assert.hpp"))
            {
                useBoost[17] = true; continue;
            }
            scanner.Compile(wxT("^boost/algorithm/.*hpp")); //StringAlgo
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[18] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/tokenizer.hpp"))
            {
                useBoost[19] = true; continue;
            }
            scanner.Compile(wxT("^boost/logic/tribool.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[20] = true; continue;
            }
            scanner.Compile(wxT("^boost/tuple/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[21] = true; continue;
            }
            scanner.Compile(wxT("^boost/type_traits.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[22] = true; continue;
            }
            scanner.Compile(wxT("^boost/unordered.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[23] = true; continue;
            }
            scanner.Compile(wxT("^boost/utility.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[24] = true; continue;
            }
            scanner.Compile(wxT("^boost/uuid/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[25] = true; continue;
            }
            scanner.Compile(wxT("^boost/variant.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[26] = true; continue;
            }
            scanner.Compile(wxT("^boost/xpressive/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[27] = true; continue;
            }
            //Requires linking
            scanner.Compile(wxT("^boost/date_time.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[28] = true; continue;
            }
            scanner.Compile(wxT("^boost/filesystem.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[29] = true; continue;
            }
            scanner.Compile(wxT("^boost/graph/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[30] = true; continue;
            }
            scanner.Compile(wxT("^boost/iostreams/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[31] = true; continue;
            }
            scanner.Compile(wxT("^boost/program_options.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[32] = true; continue;
            }
            scanner.Compile(wxT("^boost/python.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[33] = true; continue;
            }
            if(tmpArray[i] == wxT("boost/regex.hpp"))
            {
                useBoost[34] = true; continue;
            }
            scanner.Compile(wxT("^boost/serialization/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[35] = true; continue;
            }
            scanner.Compile(wxT("^boost/signal.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[36] = true; continue;
            }
            scanner.Compile(wxT("^boost/system/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[37] = true; continue;
            }
            scanner.Compile(wxT("^boost/test/.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[38] = true; continue;
            }
            scanner.Compile(wxT("^boost/thread.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[39] = true; continue;
            }
            scanner.Compile(wxT("^boost/wave.*hpp"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[40] = true; continue;
            }
            continue;
        }
    }
    if(useBoost[0])
    {
        tmpArray.Clear();
        tmpArray = m_project->GetIncludeDirs();
        for(int i = 0; i < m_project->GetBuildTargetsCount(); i++)
        {
            AppendArray(m_project->GetBuildTarget(i)->GetIncludeDirs(), tmpArray);
        }
        for(unsigned int i = 0; i < tmpArray.GetCount(); i++)
        {
            wxString verFile = tmpArray[i] + wxT("/boost/version.hpp");
            Manager::Get()->GetMacrosManager()->ReplaceMacros(verFile);
            wxTextFile file(verFile);
            if(file.Open())
            {
                wxRegEx scanner(wxT("^#define BOOST_LIB_VERSION \"([[:digit:]]+)_([[:digit:]]+)\""));
                for(unsigned int j = 0; j < file.GetLineCount(); j++)
                {
                    if(scanner.Matches(file.GetLine(j)))
                    {
                        double verNumMaj = 0;
                        double verNumMin = 0;
                        if(scanner.GetMatch(file.GetLine(j), 1).ToDouble(&verNumMaj) && scanner.GetMatch(file.GetLine(j), 2).ToDouble(&verNumMin))
                        {
                            boostVer = wxString::Format(wxT("%d.%d.0"), (int)verNumMaj, (int)verNumMin);
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    tmpArray.Clear();
    tmpArray = m_project->GetLinkLibs();
    for(int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        AppendArray(m_project->GetBuildTarget(i)->GetLinkLibs(), tmpArray);
    }
    for(unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner(wxT(".*wx[[:alpha:]]+([[:digit:]]+).*"));
        if(scanner.Matches(tmpArray[i]))
        {
            useWx = true;
            double verNum = 0;
            if(scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNum) && verNum >= 10)
            {
                wxVer = (int)verNum;
            }
            continue;
        }
        scanner.Compile(wxT(".*boost_.*([[:digit:]]+)_([[:digit:]]+).*"));
        if(scanner.Matches(tmpArray[i]))
        {
            useBoost[0] = true;
            double verNumMaj = 0;
            double verNumMin = 0;
            if(scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNumMaj) && scanner.GetMatch(tmpArray[i], 2).ToDouble(&verNumMin))
            {
                boostVer = wxString::Format(wxT("%d.%d.0"), (int)verNumMaj, (int)verNumMin);
            }
            scanner.Compile(wxT(".*boost_date_time.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[28] = true; continue;
            }
            scanner.Compile(wxT(".*boost_filesystem.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[29] = true; continue;
            }
            scanner.Compile(wxT(".*boost_graph.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[30] = true; continue;
            }
            scanner.Compile(wxT(".*boost_iostreams.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[31] = true; continue;
            }
            scanner.Compile(wxT(".*boost_program_options.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[32] = true; continue;
            }
            scanner.Compile(wxT(".*boost_python.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[33] = true; continue;
            }
            scanner.Compile(wxT(".*boost_regex.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[34] = true; continue;
            }
            scanner.Compile(wxT(".*boost_serialization.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[35] = true; continue;
            }
            scanner.Compile(wxT(".*boost_signals.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[36] = true; continue;
            }
            scanner.Compile(wxT(".*boost_system.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[37] = true; continue;
            }
            scanner.Compile(wxT(".*boost_unit_test_framework.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[38] = true; continue;
            }
            scanner.Compile(wxT(".*boost_thread.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[39] = true; continue;
            }
            scanner.Compile(wxT(".*boost_wave.*"));
            if(scanner.Matches(tmpArray[i]))
            {
                useBoost[40] = true; continue;
            }
            continue;
        }
    }
    tmpArray.Clear();
    tmpArray = m_project->GetCompilerOptions();
    AppendArray(m_project->GetLinkerOptions(), tmpArray);
    for(int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        AppendArray(m_project->GetBuildTarget(i)->GetCompilerOptions(), tmpArray);
        AppendArray(m_project->GetBuildTarget(i)->GetLinkerOptions(), tmpArray);
    }
    for(unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner(wxT(".*wx-config.*--version=([[:digit:][:punct:]]+).*|.*wx-config.*"));
        if(scanner.Matches(tmpArray[i]))
        {
            useWx = true;
            double verNum = 0;
            if(scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNum) && verNum >= 1)
            {
                wxVer = (int)(verNum * 10);
            }
            continue;
        }
    }
    if(useWx)
    {
        Manager::Get()->GetLogManager()->Log(wxString::Format(
             wxT("AM_OPTIONS_WXCONFIG\n"
                 "AM_PATH_WXCONFIG(%d.%d.0, wxWin=1)\n"
                 "    if test \"$wxWin\" != 1; then\n"
                 "        AC_MSG_ERROR([\n"
                 "            wxWidgets must be installed on your system.\n\n"
                 "            Please check that wx-config is in path, the directory\n"
                 "            where wxWidgets libraries are installed (returned by\n"
                 "            'wx-config --libs' or 'wx-config --static --libs' command)\n"
                 "            is in LD_LIBRARY_PATH or equivalent variable and\n"
                 "            wxWindows version is %d.%d.0 or above.\n"
                 "        ])\n"
                 "    fi"), wxVer / 10, wxVer % 10, wxVer / 10, wxVer % 10)
                                             );
    }
    if(useBoost[0])
    {
        wxString bufferString =
             wxT("BOOST_REQUIRE([") + boostVer + wxT("])\n");{wxString s;
                                               if(useBoost[1]){s+=wxT(
                 "BOOST_ARRAY\n");}            if(useBoost[2]){s+=wxT(
                 "BOOST_ASIO\n");}             if(useBoost[3]){s+=wxT(
                 "BOOST_BIND\n");}             if(useBoost[4]){s+=wxT(
                 "BOOST_CONVERSION\n");}       if(useBoost[5]){s+=wxT(
                 "BOOST_FOREACH\n");}          if(useBoost[6]){s+=wxT(
                 "BOOST_FORMAT\n");}           if(useBoost[7]){s+=wxT(
                 "BOOST_FUNCTION\n");}         if(useBoost[8]){s+=wxT(
                 "BOOST_HASH\n");}             if(useBoost[9]){s+=wxT(
                 "BOOST_LAMBDA\n");}           if(useBoost[10]){s+=wxT(
                 "BOOST_MATH\n");}             if(useBoost[11]){s+=wxT(
                 "BOOST_MULTIARRAY\n");}       if(useBoost[12]){s+=wxT(
                 "BOOST_NUMERICCONVERSION\n");}if(useBoost[13]){s+=wxT(
                 "BOOST_OPTIONAL\n");}         if(useBoost[14]){s+=wxT(
                 "BOOST_PREPROCESSOR\n");}     if(useBoost[15]){s+=wxT(
                 "BOOST_REF\n");}              if(useBoost[16]){s+=wxT(
                 "BOOST_SMARTPTR\n");}         if(useBoost[17]){s+=wxT(
                 "BOOST_STATICASSERT\n");}     if(useBoost[18]){s+=wxT(
                 "BOOST_STRINGALGO\n");}       if(useBoost[19]){s+=wxT(
                 "BOOST_TOKENIZER\n");}        if(useBoost[20]){s+=wxT(
                 "BOOST_TRIBOOL\n");}          if(useBoost[21]){s+=wxT(
                 "BOOST_TUPLE\n");}            if(useBoost[22]){s+=wxT(
                 "BOOST_TYPETRAITS\n");}       if(useBoost[23]){s+=wxT(
                 "BOOST_UNORDERD\n");}         if(useBoost[24]){s+=wxT(
                 "BOOST_UTILITY\n");}          if(useBoost[25]){s+=wxT(
                 "BOOST_UUID\n");}             if(useBoost[26]){s+=wxT(
                 "BOOST_VARIANT\n");}          if(useBoost[27]){s+=wxT(
                 "BOOST_XPRESSIVE\n");}        //Requires linking
                                               if(useBoost[28]){s+=wxT(
                 "BOOST_DATE_TIME\n");}        if(useBoost[29]){s+=wxT(
                 "BOOST_FILESYSTEM\n");}       if(useBoost[30]){s+=wxT(
                 "BOOST_GRAPH\n");}            if(useBoost[31]){s+=wxT(
                 "BOOST_IOSTREAMS\n");}        if(useBoost[32]){s+=wxT(
                 "BOOST_PROGRAM_OPTIONS\n");}  if(useBoost[33]){s+=wxT(
                 "BOOST_PYTHON\n");}           if(useBoost[34]){s+=wxT(
                 "BOOST_REGEX\n");}            if(useBoost[35]){s+=wxT(
                 "BOOST_SERIALIZATION\n");}    if(useBoost[36]){s+=wxT(
                 "BOOST_SIGNALS\n");}          if(useBoost[37]){s+=wxT(
                 "BOOST_SYSTEM\n");}           if(useBoost[38]){s+=wxT(
                 "BOOST_TEST\n");}             if(useBoost[39]){s+=wxT(
                 "BOOST_THREADS\n");}          if(useBoost[40]){s+=wxT(
                 "BOOST_WAVE\n");}bufferString+=s;}
        Manager::Get()->GetLogManager()->Log(bufferString);
    }
    //wxArrayString tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
    return output;
}

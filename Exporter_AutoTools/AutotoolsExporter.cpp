// System include files
#include <wx/textfile.h>

// CB include files (not DAP)
#include <sdk.h> // Code::Blocks SDK
#include "manager.h"
#include "logmanager.h"
#include "macrosmanager.h"

// ProjectExporter include files
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
    Manager::Get()->GetLogManager()->DebugLog(m_configure);
    Manager::Get()->GetLogManager()->DebugLog(m_makefile);
    /*wxArrayString includesArray = IncludeFilesArray();
    for(unsigned int i = 0; i < includesArray.GetCount(); i++)
    {
        Manager::Get()->GetLogManager()->Log("<" + includesArray[i] + ">");
    }*/
    //Manager::Get()->GetFileManager()->Save((m_project->GetBasePath() + "configure.ac"), m_configure, wxFONTENCODING_SYSTEM, true);
    //Manager::Get()->GetFileManager()->Save((m_project->GetBasePath() + "makefile.am"), m_makefile, wxFONTENCODING_SYSTEM, true);
}

void AutotoolsExporter::CreateConfig()
{
    m_configure = "AC_PREREQ([2.64])\n"
                  "AC_INIT([" + m_project->GetTitle() + "], [1.0])\n"
                  "AM_INIT_AUTOMAKE\n\n";
    wxString bufferString;
    bool useC = false;
    bool useCxx = false;

    for (FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile * pf = *i;
        wxString ext = pf->file.GetExt().Lower();

        if (ext == "c")
        {
            useC = true;
            bufferString = pf->relativeFilename;

            if (ext == m_project->GetTitle())
            {
                break;
            }
        }
        else
            if (ext == "cpp" ||
                    ext == "cc"  ||
                    ext == "cxx")
            {
                useCxx = true;
                bufferString = pf->relativeFilename;

                if (pf->file.GetName() == m_project->GetTitle())
                {
                    break;
                }
            }
    }

    m_configure += "AC_CONFIG_SRCDIR([" + ConvertSlash(bufferString) + "])\n";
    bufferString = wxFileName((*m_project->GetFilesList().begin())->relativeFilename).GetPath(wxPATH_GET_SEPARATOR, wxPATH_UNIX);

    for (FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile * pf = *i;

        if (FileTypeOf(pf->relativeFilename) == ftHeader)
        {
            wxString headerPath = wxFileName(pf->relativeFilename).GetPath(wxPATH_GET_SEPARATOR, wxPATH_UNIX);

            if (headerPath.IsSameAs("include/", false))
            {
                bufferString = headerPath;
                break;
            }

            if (headerPath.Length() < bufferString.Length())
            {
                bufferString = headerPath;
            }

            if (bufferString.IsEmpty())
            {
                break;
            }
        }
    }

    m_configure   +=   "AC_CONFIG_HEADERS([" + bufferString + "config.h])\n"
                       "AC_CONFIG_FILES([Makefile])\n\n";

    if (!useC)
    {
        m_configure += "AC_LANG([C++])\n\n";
    }

    wxTextFile file(m_project->GetCommonTopLevelPath() + "/configure.scan");
    wxArrayString(macros)[5];

    if (file.Open())
    {
        int fileSection = -1;

        for (unsigned int i = 0; i < file.GetLineCount(); i++)
        {
            bufferString = file.GetLine(i);

            if (bufferString    ==   "# Checks for programs.")
            {
                fileSection = 0;
            }
            else
                if (bufferString == "# Checks for libraries.")
                {
                    fileSection = 1;
                }
                else
                    if (bufferString == "# Checks for header files.")
                    {
                        fileSection = 2;
                    }
                    else
                        if (bufferString == "# Checks for typedefs, structures, and compiler characteristics.")
                        {
                            fileSection = 3;
                        }
                        else
                            if (bufferString == "# Checks for library functions.")
                            {
                                fileSection = 4;
                            }
                            else
                                if (bufferString.IsEmpty())
                                {
                                    if (fileSection == 4)
                                    {
                                        break;
                                    }
                                }
                                else
                                    if (fileSection != -1)
                                    {
                                        macros[fileSection].Add(bufferString);
                                    }
        }
    }
    else
    {
        wxString s = "AC_PROG_C";

        if (useC)
        {
            s += "C\n";

            if (useCxx)
            {
                s +=
                    "AC_PROG_CXX\n\n";
            }
            else
            {
                s += "\n";
            }
        }
        else
        {
            s += "XX\n\n";
        };

        m_configure += s +
                       "# Warning: \"configure.scan\" not found; this file is required\n"
                       "#          to catch a greater number of necessary macros.\n"
                       "#          Please run \"autoscan\" to generate this file.\n\n";
    }

    IdentifyLibs();/*********************************************/

    for (int i = 0; i < 5; i++)
    {
        bufferString = GetStringFromArray(macros[i], "\n", false);

        if (!bufferString.IsEmpty())
        {
            m_configure += bufferString + "\n\n";
        }
    }

    m_configure  +=  "AC_OUTPUT\n\n"
                     "echo \\\n"
                     "\"-------------------------------------------------\n"
                     "    ${PACKAGE_NAME}   Version ${PACKAGE_VERSION}\n\n"
                     " Prefix:   ${prefix}\n";
    {
        wxString s =
            " Compiler: ${C";

        if (useC)
        {
            s += "C} ${C";

            if (useCxx)
            {
                s += "FLAGS} ${CPPFLAGS}\n"
                     "           ${CXX} ${CXX";
            }
        }
        else
        {
            s += "XX} ${CXX";
        }

        s += "FLAGS} ${CPPFLAGS}\n";
        m_configure += s +
                       "-------------------------------------------------\n"
                       "Configuration is complete; enter 'make' to build\"";
    }
}

void AutotoolsExporter::CreateMake()
{
    m_makefile.Clear();

    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        if (!m_makefile.IsEmpty())
        {
            m_makefile += "\n\n";
        }

        wxString name = RemLib(wxFileName(m_project->GetBuildTarget(i)->GetOutputFilename()).GetName());

        switch (m_project->GetBuildTarget(i)->GetTargetType())
        {
            case ttExecutable:
            case ttConsoleOnly:
            {
                m_makefile += "bin_PROGRAMS = " + name + "\n" +
                              name + "_SOURCES =";

                for (FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
                {
                    ProjectFile * pf = *j;
                    wxString cfn(pf->relativeFilename);

                    if (cfn.Right(4) == ".cpp" || cfn.Right(2) == ".c" || cfn.Right(3) == ".cc" || cfn.Right(4) == ".cxx")
                    {
                        if (pf->compile)
                        {
                            m_makefile += " " + ConvertSlash(cfn);
                        }
                    }
                }

                /*wxArrayString tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
                if(!tmpArray.IsEmpty())
                {
                    m_makefile += "_LDADD ="
                    for(unsigned int j=0; j < tmpArray.GetCount(); j++)
                    {
                        ConvertSlash(tmpArray[j]);
                    }
                }*/
            }

            default:
                ;
        }
    }
}

wxArrayString AutotoolsExporter::IdentifyLibs()
{
    wxArrayString output;
    bool useWx = false;
    int wxVer = 28;
    bool useBoost[41];

    for (int i = 0; i < 41; i++)
    {
        useBoost[i] = false;
    }

    wxString boostVer = "1.48.0";
    wxArrayString tmpArray = IncludeFilesArray();

    for (unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner("^wx/.*");

        if (scanner.Matches(tmpArray[i]))
        {
            useWx = true;
            continue;
        }

        scanner.Compile("^boost/.*hpp");

        if (scanner.Matches(tmpArray[i]))
        {
            useBoost[0] = true;

            if (tmpArray[i] == "boost/array.hpp")
            {
                useBoost[1] = true;
                continue;
            }

            scanner.Compile("^boost/asio.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[2] = true;
                continue;
            }

            scanner.Compile("^boost/bind.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[3] = true;
                continue;
            }

            if (tmpArray[i] == "boost/cast.hpp" ||     //Conversion
                    tmpArray[i] == "boost/lexical_cast.hpp")
            {
                useBoost[4] = true;
                continue;
            }

            if (tmpArray[i] == "boost/foreach.hpp")
            {
                useBoost[5] = true;
                continue;
            }

            scanner.Compile("^boost/format.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[6] = true;
                continue;
            }

            scanner.Compile("^boost/function[^a]*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[7] = true;
                continue;
            }

            scanner.Compile("^boost/functional.*hash.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[8] = true;
                continue;
            }

            scanner.Compile("^boost/lambda/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[9] = true;
                continue;
            }

            scanner.Compile("^boost/math.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[10] = true;
                continue;
            }

            scanner.Compile("^boost/multi_array.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[11] = true;
                continue;
            }

            scanner.Compile("^boost/numeric/conversion.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[12] = true;
                continue;
            }

            scanner.Compile("^boost/optional.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[13] = true;
                continue;
            }

            scanner.Compile("^boost/preprocessor.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[14] = true;
                continue;
            }

            if (tmpArray[i] == "boost/ref.hpp")
            {
                useBoost[15] = true;
                continue;
            }

            scanner.Compile("^boost/smart_ptr.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[16] = true;
                continue;
            }

            if (tmpArray[i] == "boost/static_assert.hpp")
            {
                useBoost[17] = true;
                continue;
            }

            scanner.Compile("^boost/algorithm/.*hpp"); //StringAlgo

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[18] = true;
                continue;
            }

            if (tmpArray[i] == "boost/tokenizer.hpp")
            {
                useBoost[19] = true;
                continue;
            }

            scanner.Compile("^boost/logic/tribool.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[20] = true;
                continue;
            }

            scanner.Compile("^boost/tuple/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[21] = true;
                continue;
            }

            scanner.Compile("^boost/type_traits.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[22] = true;
                continue;
            }

            scanner.Compile("^boost/unordered.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[23] = true;
                continue;
            }

            scanner.Compile("^boost/utility.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[24] = true;
                continue;
            }

            scanner.Compile("^boost/uuid/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[25] = true;
                continue;
            }

            scanner.Compile("^boost/variant.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[26] = true;
                continue;
            }

            scanner.Compile("^boost/xpressive/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[27] = true;
                continue;
            }

            //Requires linking
            scanner.Compile("^boost/date_time.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[28] = true;
                continue;
            }

            scanner.Compile("^boost/filesystem.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[29] = true;
                continue;
            }

            scanner.Compile("^boost/graph/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[30] = true;
                continue;
            }

            scanner.Compile("^boost/iostreams/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[31] = true;
                continue;
            }

            scanner.Compile("^boost/program_options.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[32] = true;
                continue;
            }

            scanner.Compile("^boost/python.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[33] = true;
                continue;
            }

            if (tmpArray[i] == "boost/regex.hpp")
            {
                useBoost[34] = true;
                continue;
            }

            scanner.Compile("^boost/serialization/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[35] = true;
                continue;
            }

            scanner.Compile("^boost/signal.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[36] = true;
                continue;
            }

            scanner.Compile("^boost/system/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[37] = true;
                continue;
            }

            scanner.Compile("^boost/test/.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[38] = true;
                continue;
            }

            scanner.Compile("^boost/thread.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[39] = true;
                continue;
            }

            scanner.Compile("^boost/wave.*hpp");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[40] = true;
                continue;
            }

            continue;
        }
    }

    if (useBoost[0])
    {
        tmpArray.Clear();
        tmpArray = m_project->GetIncludeDirs();

        for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
        {
            AppendArray(m_project->GetBuildTarget(i)->GetIncludeDirs(), tmpArray);
        }

        for (unsigned int i = 0; i < tmpArray.GetCount(); i++)
        {
            wxString verFile = tmpArray[i] + "/boost/version.hpp";
            Manager::Get()->GetMacrosManager()->ReplaceMacros(verFile);
            wxTextFile file(verFile);

            if (file.Open())
            {
                wxRegEx scanner("^#define BOOST_LIB_VERSION \"([[:digit:]]+)_([[:digit:]]+)\"");

                for (unsigned int j = 0; j < file.GetLineCount(); j++)
                {
                    if (scanner.Matches(file.GetLine(j)))
                    {
                        double verNumMaj = 0;
                        double verNumMin = 0;

                        if (scanner.GetMatch(file.GetLine(j), 1).ToDouble(&verNumMaj) && scanner.GetMatch(file.GetLine(j), 2).ToDouble(&verNumMin))
                        {
                            boostVer = wxString::Format("%d.%d.0", (int)verNumMaj, (int)verNumMin);
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

    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        AppendArray(m_project->GetBuildTarget(i)->GetLinkLibs(), tmpArray);
    }

    for (unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner(".*wx[[:alpha:]]+([[:digit:]]+).*");

        if (scanner.Matches(tmpArray[i]))
        {
            useWx = true;
            double verNum = 0;

            if (scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNum) && verNum >= 10)
            {
                wxVer = (int)verNum;
            }

            continue;
        }

        scanner.Compile(".*boost_.*([[:digit:]]+)_([[:digit:]]+).*");

        if (scanner.Matches(tmpArray[i]))
        {
            useBoost[0] = true;
            double verNumMaj = 0;
            double verNumMin = 0;

            if (scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNumMaj) && scanner.GetMatch(tmpArray[i], 2).ToDouble(&verNumMin))
            {
                boostVer = wxString::Format("%d.%d.0", (int)verNumMaj, (int)verNumMin);
            }

            scanner.Compile(".*boost_date_time.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[28] = true;
                continue;
            }

            scanner.Compile(".*boost_filesystem.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[29] = true;
                continue;
            }

            scanner.Compile(".*boost_graph.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[30] = true;
                continue;
            }

            scanner.Compile(".*boost_iostreams.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[31] = true;
                continue;
            }

            scanner.Compile(".*boost_program_options.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[32] = true;
                continue;
            }

            scanner.Compile(".*boost_python.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[33] = true;
                continue;
            }

            scanner.Compile(".*boost_regex.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[34] = true;
                continue;
            }

            scanner.Compile(".*boost_serialization.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[35] = true;
                continue;
            }

            scanner.Compile(".*boost_signals.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[36] = true;
                continue;
            }

            scanner.Compile(".*boost_system.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[37] = true;
                continue;
            }

            scanner.Compile(".*boost_unit_test_framework.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[38] = true;
                continue;
            }

            scanner.Compile(".*boost_thread.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[39] = true;
                continue;
            }

            scanner.Compile(".*boost_wave.*");

            if (scanner.Matches(tmpArray[i]))
            {
                useBoost[40] = true;
                continue;
            }

            continue;
        }
    }

    tmpArray.Clear();
    tmpArray = m_project->GetCompilerOptions();
    AppendArray(m_project->GetLinkerOptions(), tmpArray);

    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        AppendArray(m_project->GetBuildTarget(i)->GetCompilerOptions(), tmpArray);
        AppendArray(m_project->GetBuildTarget(i)->GetLinkerOptions(), tmpArray);
    }

    for (unsigned int i = 0; i < tmpArray.GetCount(); i++)
    {
        wxRegEx scanner(".*wx-config.*--version=([[:digit:][:punct:]]+).*|.*wx-config.*");

        if (scanner.Matches(tmpArray[i]))
        {
            useWx = true;
            double verNum = 0;

            if (scanner.GetMatch(tmpArray[i], 1).ToDouble(&verNum) && verNum >= 1)
            {
                wxVer = (int)(verNum * 10);
            }

            continue;
        }
    }

    if (useWx)
    {
        Manager::Get()->GetLogManager()->Log(wxString::Format(
                                                 "AM_OPTIONS_WXCONFIG\n"
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
                                                 "    fi", wxVer / 10, wxVer % 10, wxVer / 10, wxVer % 10)
                                            );
    }

    if (useBoost[0])
    {
        wxString bufferString = "BOOST_REQUIRE([" + boostVer + "])\n";
        {
            wxString s;

            if (useBoost[1])
            {
                s += "BOOST_ARRAY\n";
            }

            if (useBoost[2])
            {
                s += "BOOST_ASIO\n";
            }

            if (useBoost[3])
            {
                s += "BOOST_BIND\n";
            }

            if (useBoost[4])
            {
                s += "BOOST_CONVERSION\n";
            }

            if (useBoost[5])
            {
                s += "BOOST_FOREACH\n";
            }

            if (useBoost[6])
            {
                s += "BOOST_FORMAT\n";
            }

            if (useBoost[7])
            {
                s += "BOOST_FUNCTION\n";
            }

            if (useBoost[8])
            {
                s += "BOOST_HASH\n";
            }

            if (useBoost[9])
            {
                s += "BOOST_LAMBDA\n";
            }

            if (useBoost[10])
            {
                s += "BOOST_MATH\n";
            }

            if (useBoost[11])
            {
                s += "BOOST_MULTIARRAY\n";
            }

            if (useBoost[12])
            {
                s += "BOOST_NUMERICCONVERSION\n";
            }

            if (useBoost[13])
            {
                s += "BOOST_OPTIONAL\n";
            }

            if (useBoost[14])
            {
                s += "BOOST_PREPROCESSOR\n";
            }

            if (useBoost[15])
            {
                s += "BOOST_REF\n";
            }

            if (useBoost[16])
            {
                s += "BOOST_SMARTPTR\n";
            }

            if (useBoost[17])
            {
                s += "BOOST_STATICASSERT\n";
            }

            if (useBoost[18])
            {
                s += "BOOST_STRINGALGO\n";
            }

            if (useBoost[19])
            {
                s += "BOOST_TOKENIZER\n";
            }

            if (useBoost[20])
            {
                s += "BOOST_TRIBOOL\n";
            }

            if (useBoost[21])
            {
                s += "BOOST_TUPLE\n";
            }

            if (useBoost[22])
            {
                s += "BOOST_TYPETRAITS\n";
            }

            if (useBoost[23])
            {
                s += "BOOST_UNORDERD\n";
            }

            if (useBoost[24])
            {
                s += "BOOST_UTILITY\n";
            }

            if (useBoost[25])
            {
                s += "BOOST_UUID\n";
            }

            if (useBoost[26])
            {
                s += "BOOST_VARIANT\n";
            }

            if (useBoost[27])
            {
                s += "BOOST_XPRESSIVE\n";
            }

            //Requires linking

            if (useBoost[28])
            {
                s += "BOOST_DATE_TIME\n";
            }

            if (useBoost[29])
            {
                s += "BOOST_FILESYSTEM\n";
            }

            if (useBoost[30])
            {
                s += "BOOST_GRAPH\n";
            }

            if (useBoost[31])
            {
                s += "BOOST_IOSTREAMS\n";
            }

            if (useBoost[32])
            {
                s += "BOOST_PROGRAM_OPTIONS\n";
            }

            if (useBoost[33])
            {
                s += "BOOST_PYTHON\n";
            }

            if (useBoost[34])
            {
                s += "BOOST_REGEX\n";
            }

            if (useBoost[35])
            {
                s += "BOOST_SERIALIZATION\n";
            }

            if (useBoost[36])
            {
                s += "BOOST_SIGNALS\n";
            }

            if (useBoost[37])
            {
                s += "BOOST_SYSTEM\n";
            }

            if (useBoost[38])
            {
                s += "BOOST_TEST\n";
            }

            if (useBoost[39])
            {
                s += "BOOST_THREADS\n";
            }

            if (useBoost[40])
            {
                s += "BOOST_WAVE\n";
            }

            bufferString += s;
        }
        Manager::Get()->GetLogManager()->Log(bufferString);
    }

    //wxArrayString tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
    return output;
}

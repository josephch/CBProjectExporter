// System include files

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "filemanager.h"
#include "manager.h"
#include "macrosmanager.h"

// ProjectExporter include files
#include "Premake4Exporter.h"

Premake4Exporter::Premake4Exporter()
{
    m_project = Manager::Get()->GetProjectManager()->GetActiveProject();
    m_content = "solution \"";
}

Premake4Exporter::~Premake4Exporter()
{
    //dtor
}

void Premake4Exporter::RunExport(bool EvaluateVars, bool UpgrTargs)
{
    //output file
    wxString fn(m_project->GetBasePath());
    fn << "premake4.lua";
    m_content << m_project->GetTitle() << "\"\r\n    configurations { \"";
    //    m_content << m_project->GetBuildTarget(0)->GetTitle();
    //    for(int i=1; i < m_project->GetBuildTargetsCount(); i++)
    //    {
    //        m_content << "\", \"" << m_project->GetBuildTarget(i)->GetTitle();
    //    }

    if (UpgrTargs)
    {
        ExportUpgrade(EvaluateVars);
    }
    else
    {
        ExportStraight(EvaluateVars);
    }

#ifndef __WXMSW__
    m_content.Replace("\r\n", "\n");
#endif //__WXMSW__
    Manager::Get()->GetFileManager()->Save(fn, m_content, wxFONTENCODING_SYSTEM, true, true);
}

wxString Premake4Exporter::EmitFlags(const wxString & compilerID, const wxArrayString & compilerFlags)
{
    wxString flags;

    //Don't bother if nothing needs to be done
    if (compilerFlags.IsEmpty())
    {
        flags.Clear();
    }
    //gcc flag parser
    else
        if (compilerID == "gcc" || compilerID == "cygwin" || compilerID == "arm-elf-gcc" || compilerID == "tricoregcc" || compilerID == "ppcgcc" || compilerID == "msp430gcc")
        {
            if (StringExists(compilerFlags, "-msse"))
            {
                flags << "\", \"EnableSSE";
            }

            if (StringExists(compilerFlags, "-msse2"))
            {
                flags << "\", \"EnableSSE2";
            }

            if (StringExists(compilerFlags, "-Wall"))
            {
                flags << "\", \"ExtraWarnings";
            }

            if (StringExists(compilerFlags, "-Wfatal-errors"))
            {
                flags << "\", \"FatalWarnings";
            }

            if (StringExists(compilerFlags, "-ffast-math"))
            {
                flags << "\", \"FloatFast";
            }

            if (StringExists(compilerFlags, "-ffloat-store"))
            {
                flags << "\", \"FloatStrict";
            }

            if (StringExists(compilerFlags, "-fno-exceptions"))
            {
                flags << "\", \"NoExceptions";
            }

            if (StringExists(compilerFlags, "-fomit-frame-pointer"))
            {
                flags << "\", \"NoFramePointer";
            }

            if (StringExists(compilerFlags, "-fno-rtti"))
            {
                flags << "\", \"NoRTTI";
            }

            if (StringExists(compilerFlags, "-O2"))
            {
                flags << "\", \"Optimize";
            }

            if (StringExists(compilerFlags, "-Os"))
            {
                flags << "\", \"OptimizeSize";
            }

            if (StringExists(compilerFlags, "-O3"))
            {
                flags << "\", \"OptimizeSpeed";
            }

            if (StringExists(compilerFlags, "-g"))
            {
                flags << "\", \"Symbols";
            }

            flags << "\"";
            flags = flags.Mid(3);
        }
        //msvc flag parser
        else
            if (compilerID == "msvc10" || compilerID == "msvc8" || compilerID == "msvctk")
            {
                if (StringExists(compilerFlags, "/arch:SSE"))
                {
                    flags << "\", \"EnableSSE";
                }

                if (StringExists(compilerFlags, "/arch:SSE2"))
                {
                    flags << "\", \"EnableSSE2";
                }

                if (StringExists(compilerFlags, "/Wall"))
                {
                    flags << "\", \"ExtraWarnings";
                }

                if (StringExists(compilerFlags, "/WX"))
                {
                    flags << "\", \"FatalWarnings";
                }

                if (StringExists(compilerFlags, "/fp:fast"))
                {
                    flags << "\", \"FloatFast";
                }

                if (StringExists(compilerFlags, "/fp:precise") || StringExists(compilerFlags, "/fp:strict"))
                {
                    flags << "\", \"FloatStrict";
                }

                if (StringExists(compilerFlags, "/EH-"))
                {
                    flags << "\", \"NoExceptions";
                }

                if (StringExists(compilerFlags, "/Oy"))
                {
                    flags << "\", \"NoFramePointer";
                }

                if (StringExists(compilerFlags, "/GR-"))
                {
                    flags << "\", \"NoRTTI";
                }

                if (StringExists(compilerFlags, "/Ox") || StringExists(compilerFlags, "/Og"))
                {
                    flags << "\", \"Optimize";
                }

                if (StringExists(compilerFlags, "/O1") || StringExists(compilerFlags, "/Os"))
                {
                    flags << "\", \"OptimizeSize";
                }

                if (StringExists(compilerFlags, "/O2") || StringExists(compilerFlags, "/Ot"))
                {
                    flags << "\", \"OptimizeSpeed";
                }

                if (StringExists(compilerFlags, "/Zi"))
                {
                    flags << "\", \"Symbols";
                }

                flags << "\"";
                flags = flags.Mid(3);
            }

    return flags;
}

wxString Premake4Exporter::EmitDefines(const wxArrayString & compilerFlags)
{
    wxString defines;

    //Don't bother if nothing needs to be done
    if (compilerFlags.IsEmpty())
    {
        return defines;
    }

    for (unsigned int i = 0; i < compilerFlags.Count(); i++)
    {
        if (compilerFlags[i].Left(2) == "-D" || compilerFlags[i].Left(2) == "/D")
        {
            defines << "\", \"" << compilerFlags[i].Mid(2);
        }
    }

    //Return if nothing is found
    if (defines.IsEmpty())
    {
        return defines;
    }

    defines << "\"";
    defines = defines.Mid(3);
    return defines;
}

wxString Premake4Exporter::AddEscapes(const wxString & source)
{
    wxString output = source;
    output.Replace("\\", "\\\\");
    output.Replace("\"", "\\\"");
    return output;
}

wxString Premake4Exporter::ReplVars(const wxString & source, bool repl)
{
    if (repl)
    {
        wxString output = Manager::Get()->GetMacrosManager()->ReplaceMacros(source);
        return output;
    }
    else
    {
        return source;
    }
}

void Premake4Exporter::ExportStraight(bool EvaluateVars)
{
    //Write configurations
    m_content << m_project->GetBuildTarget(0)->GetTitle();

    for (int i = 1; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << "\", \"" << m_project->GetBuildTarget(i)->GetTitle();
    }

    //Write project
    m_content << "\" }\r\n\r\n    project \"" << m_project->GetTitle();
    //Acquire file list
    wxString bufferString;
    bool tmpBool = true;

    for (FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile * pf = *i;
        bufferString << "\", \"" << ConvertSlash(pf->relativeFilename);

        if (tmpBool && (bufferString.Right(2) == ".c"))
        {
            tmpBool = false;
        }
    }

    bufferString << "\" }";
    //Determine language (may not be accurate)
    m_content << "\"\r\n        language \"C";

    if (tmpBool)
    {
        m_content << "++";
    }

    m_content << "\"\r\n        files {" << bufferString.Mid(2);
    //Emit flags
    bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetCompilerOptions());

    if (!bufferString.IsEmpty())
    {
        m_content << "\r\n        flags { " << bufferString << " }";
    }

    //Emit defines
    bufferString = EmitDefines(m_project->GetCompilerOptions());

    if (!bufferString.IsEmpty())
    {
        m_content << "\r\n        defines { " << bufferString << " }";
    }

    //Write libraries
    if (!m_project->GetLinkLibs().IsEmpty())
    {
        m_content << "\r\n        links { \"" << m_project->GetLinkLibs()[0];

        for (unsigned int i = 1; i < m_project->GetLinkLibs().GetCount(); i++)
        {
            m_content << "\", \"" << m_project->GetLinkLibs()[i];
        }

        m_content << "\" }";
    }

    //Compiler search directories
    if (!m_project->GetIncludeDirs().IsEmpty())
    {
        m_content << "\r\n        includedirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetIncludeDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Linker search directories
    if (!m_project->GetLibDirs().IsEmpty())
    {
        m_content << "\r\n        libdirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetLibDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Resource compiler search directories
    if (!m_project->GetResourceIncludeDirs().IsEmpty())
    {
        m_content << "\r\n        resincludedirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetResourceIncludeDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Pre-build commands
    if (!m_project->GetCommandsBeforeBuild().IsEmpty())
    {
        m_content << "\r\n        prebuildcommands { \"";
        m_content << AddEscapes(m_project->GetCommandsBeforeBuild()[0]);

        for (unsigned int i = 1; i < m_project->GetCommandsBeforeBuild().GetCount(); i++)
        {
            m_content << "\", \"" << AddEscapes(m_project->GetCommandsBeforeBuild()[i]);
        }

        m_content << "\" }";
    }

    //Post-build commands
    if (!m_project->GetCommandsAfterBuild().IsEmpty())
    {
        m_content << "\r\n        postbuildcommands { \"";
        m_content << AddEscapes(m_project->GetCommandsAfterBuild()[0]);

        for (unsigned int i = 1; i < m_project->GetCommandsAfterBuild().GetCount(); i++)
        {
            m_content << "\", \"" << AddEscapes(m_project->GetCommandsAfterBuild()[i]);
        }

        m_content << "\" }";
    }

    //Write configurations
    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << "\r\n\r\n        configuration \"";
        m_content << m_project->GetBuildTarget(i)->GetTitle();
        //Write target type
        m_content << "\"\r\n            kind \"";

        switch (m_project->GetBuildTarget(i)->GetTargetType())
        {
            case ttExecutable:
                m_content << "WindowedApp";
                break;

            case ttConsoleOnly:
                m_content << "ConsoleApp";
                break;

            case ttStaticLib:
                m_content << "StaticLib";
                break;

            case ttDynamicLib:
                m_content << "SharedLib";
                break;

            default:
                ;
        }

        //Write target name
        wxFileName fn(m_project->GetBuildTarget(i)->GetOutputFilename());
        m_content << "\"\r\n            targetname (\"";

        if (m_project->GetBuildTarget(i)->GetTargetType() == ttStaticLib)
        {
            m_content << RemLib(fn.GetName());
        }
        else
        {
            m_content << fn.GetName();
        }

        m_content << "\")";

        //Write target directory
        if (!fn.GetPath().IsEmpty())
        {
            m_content << "\r\n            targetdir (\"";
            m_content << fn.GetPath(true, wxPATH_UNIX) << "\")";
        }

        //Write object directory
        m_content << "\r\n            objdir (\"";
        m_content << ConvertSlash(m_project->GetBuildTarget(i)->GetObjectOutput());
        m_content << "\")";
        //Emit flags
        bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetBuildTarget(i)->GetCompilerOptions());

        if (!bufferString.IsEmpty())
        {
            m_content << "\r\n            flags { " << bufferString << " }";
        }

        //Emit defines
        bufferString = EmitDefines(m_project->GetBuildTarget(i)->GetCompilerOptions());

        if (!bufferString.IsEmpty())
        {
            m_content << "\r\n            defines { " << bufferString << " }";
        }

        //Write libraries
        if (!m_project->GetBuildTarget(i)->GetLinkLibs().IsEmpty())
        {
            m_content << "\r\n            links { \"" << m_project->GetBuildTarget(i)->GetLinkLibs()[0];

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetLinkLibs().GetCount(); j++)
            {
                m_content << "\", \"" << m_project->GetBuildTarget(i)->GetLinkLibs()[j];
            }

            m_content << "\" }";
        }

        //Compiler search directories
        if (!m_project->GetBuildTarget(i)->GetIncludeDirs().IsEmpty())
        {
            m_content << "\r\n            includedirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetIncludeDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Linker search directories
        if (!m_project->GetBuildTarget(i)->GetLibDirs().IsEmpty())
        {
            m_content << "\r\n            libdirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetLibDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Resource compiler search directories
        if (!m_project->GetBuildTarget(i)->GetResourceIncludeDirs().IsEmpty())
        {
            m_content << "\r\n            resincludedirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetResourceIncludeDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Pre-build commands
        if (!m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().IsEmpty())
        {
            m_content << "\r\n            prebuildcommands { \"";
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[0]);

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().GetCount(); j++)
            {
                m_content << "\", \"" << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[j]);
            }

            m_content << "\" }";
        }

        //Post-build commands
        if (!m_project->GetBuildTarget(i)->GetCommandsAfterBuild().IsEmpty())
        {
            m_content << "\r\n            postbuildcommands { \"";
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[0]);

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetCommandsAfterBuild().GetCount(); j++)
            {
                m_content << "\", \"" << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[j]);
            }

            m_content << "\" }";
        }
    }
}

void Premake4Exporter::ExportUpgrade(bool EvaluateVars)
{
    m_content << "default\" }";
    //Emit flags
    wxString bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetCompilerOptions());

    if (!bufferString.IsEmpty())
    {
        m_content << "\r\n    flags { " << bufferString << " }";
    }

    //Emit defines
    bufferString = EmitDefines(m_project->GetCompilerOptions());

    if (!bufferString.IsEmpty())
    {
        m_content << "\r\n    defines { " << bufferString << " }";
    }

    //Write libraries
    if (!m_project->GetLinkLibs().IsEmpty())
    {
        m_content << "\r\n    links { \"" << m_project->GetLinkLibs()[0];

        for (unsigned int i = 1; i < m_project->GetLinkLibs().GetCount(); i++)
        {
            m_content << "\", \"" << m_project->GetLinkLibs()[i];
        }

        m_content << "\" }";
    }

    //Compiler search directories
    if (!m_project->GetIncludeDirs().IsEmpty())
    {
        m_content << "\r\n    includedirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetIncludeDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Linker search directories
    if (!m_project->GetLibDirs().IsEmpty())
    {
        m_content << "\r\n    libdirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetLibDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Resource compiler search directories
    if (!m_project->GetResourceIncludeDirs().IsEmpty())
    {
        m_content << "\r\n    resincludedirs { \"";
        m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[0], EvaluateVars));

        for (unsigned int i = 1; i < m_project->GetResourceIncludeDirs().GetCount(); i++)
        {
            m_content << "\", \"";
            m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[i], EvaluateVars));
        }

        m_content << "\" }";
    }

    //Pre-build commands
    if (!m_project->GetCommandsBeforeBuild().IsEmpty())
    {
        m_content << "\r\n    prebuildcommands { \"";
        m_content << AddEscapes(m_project->GetCommandsBeforeBuild()[0]);

        for (unsigned int i = 1; i < m_project->GetCommandsBeforeBuild().GetCount(); i++)
        {
            m_content << "\", \"" << AddEscapes(m_project->GetCommandsBeforeBuild()[i]);
        }

        m_content << "\" }";
    }

    //Post-build commands
    if (!m_project->GetCommandsAfterBuild().IsEmpty())
    {
        m_content << "\r\n    postbuildcommands { \"";
        m_content << AddEscapes(m_project->GetCommandsAfterBuild()[0]);

        for (unsigned int i = 1; i < m_project->GetCommandsAfterBuild().GetCount(); i++)
        {
            m_content << "\", \"" << AddEscapes(m_project->GetCommandsAfterBuild()[i]);
        }

        m_content << "\" }";
    }

    //Write projects
    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << "\r\n\r\n    project \"";
        m_content << m_project->GetBuildTarget(i)->GetTitle();
        //Acquire file list
        bufferString.Clear();
        bool tmpBool = true;

        for (FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
        {
            ProjectFile * pf = *j;
            bufferString << "\", \"" << ConvertSlash(pf->relativeFilename);

            if (tmpBool && (bufferString.Right(2) == ".c"))
            {
                tmpBool = false;
            }
        }

        //Determine language (may not be accurate)
        m_content << "\"\r\n        language \"C";

        if (tmpBool)
        {
            m_content << "++";
        }

        m_content << "\"\r\n        files {";
        m_content << ConvertSlash(bufferString.Mid(2));
        //Write target type
        m_content << "\" }\r\n        kind \"";

        switch (m_project->GetBuildTarget(i)->GetTargetType())
        {
            case ttExecutable:
                m_content << "WindowedApp";
                break;

            case ttConsoleOnly:
                m_content << "ConsoleApp";
                break;

            case ttStaticLib:
                m_content << "StaticLib";
                break;

            case ttDynamicLib:
                m_content << "SharedLib";
                break;

            default:
                ;
        }

        //Write target name
        wxFileName fn(m_project->GetBuildTarget(i)->GetOutputFilename());
        m_content << "\"\r\n        targetname (\"";

        if (m_project->GetBuildTarget(i)->GetTargetType() == ttStaticLib)
        {
            m_content << RemLib(fn.GetName());
        }
        else
        {
            m_content << fn.GetName();
        }

        m_content << "\")";

        //Write target directory
        if (!fn.GetPath().IsEmpty())
        {
            m_content << "\r\n        targetdir (\"";
            m_content << fn.GetPath(true, wxPATH_UNIX) << "\")";
        }

        //Write object directory
        m_content << "\r\n        objdir (\"";
        m_content << ConvertSlash(m_project->GetBuildTarget(i)->GetObjectOutput());
        m_content << "\")";
        //Emit flags
        bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetBuildTarget(i)->GetCompilerOptions());

        if (!bufferString.IsEmpty())
        {
            m_content << "\r\n        flags { " << bufferString << " }";
        }

        //Emit defines
        bufferString = EmitDefines(m_project->GetBuildTarget(i)->GetCompilerOptions());

        if (!bufferString.IsEmpty())
        {
            m_content << "\r\n        defines { " << bufferString << " }";
        }

        //Write libraries
        if (!m_project->GetBuildTarget(i)->GetLinkLibs().IsEmpty())
        {
            m_content << "\r\n        links { \"" << m_project->GetBuildTarget(i)->GetLinkLibs()[0];

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetLinkLibs().GetCount(); j++)
            {
                m_content << "\", \"" << m_project->GetBuildTarget(i)->GetLinkLibs()[j];
            }

            m_content << "\" }";
        }

        //Compiler search directories
        if (!m_project->GetBuildTarget(i)->GetIncludeDirs().IsEmpty())
        {
            m_content << "\r\n        includedirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetIncludeDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Linker search directories
        if (!m_project->GetBuildTarget(i)->GetLibDirs().IsEmpty())
        {
            m_content << "\r\n        libdirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetLibDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Resource compiler search directories
        if (!m_project->GetBuildTarget(i)->GetResourceIncludeDirs().IsEmpty())
        {
            m_content << "\r\n        resincludedirs { \"";
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[0], EvaluateVars));

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetResourceIncludeDirs().GetCount(); j++)
            {
                m_content << "\", \"";
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[j], EvaluateVars));
            }

            m_content << "\" }";
        }

        //Pre-build commands
        if (!m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().IsEmpty())
        {
            m_content << "\r\n        prebuildcommands { \"";
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[0]);

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().GetCount(); j++)
            {
                m_content << "\", \"" << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[j]);
            }

            m_content << "\" }";
        }

        //Post-build commands
        if (!m_project->GetBuildTarget(i)->GetCommandsAfterBuild().IsEmpty())
        {
            m_content << "\r\n        postbuildcommands { \"";
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[0]);

            for (unsigned int j = 1; j < m_project->GetBuildTarget(i)->GetCommandsAfterBuild().GetCount(); j++)
            {
                m_content << "\", \"" << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[j]);
            }

            m_content << "\" }";
        }

        m_content << "\r\n            configuration \"default\"";
    }
}

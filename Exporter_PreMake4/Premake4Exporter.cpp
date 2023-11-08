#include <sdk.h> // Code::Blocks SDK
#include "PremakeExporter.h"

PremakeExporter::PremakeExporter()
{
    m_project = Manager::Get()->GetProjectManager()->GetActiveProject();
    m_content = wxT("solution \"");
}

PremakeExporter::~PremakeExporter()
{
    //dtor
}

void PremakeExporter::RunExport(bool EvaluateVars, bool UpgrTargs)
{
    //output file
    wxString fn( m_project->GetBasePath() );
    fn << _T("premake4.lua");
    m_content << m_project->GetTitle() << _T("\"\r\n    configurations { \"");
//    m_content << m_project->GetBuildTarget(0)->GetTitle();
//    for(int i=1; i < m_project->GetBuildTargetsCount(); i++)
//    {
//        m_content << _T("\", \"") << m_project->GetBuildTarget(i)->GetTitle();
//    }

    if(UpgrTargs)
    {
        ExportUpgrade(EvaluateVars);
    }
    else
    {
        ExportStraight(EvaluateVars);
    }

#ifndef __WXMSW__
    m_content.Replace(wxT("\r\n"), wxT("\n"));
#endif //__WXMSW__
    Manager::Get()->GetFileManager()->Save(fn, m_content, wxFONTENCODING_SYSTEM, true);
}

wxString PremakeExporter::EmitFlags(const wxString& compilerID, const wxArrayString& compilerFlags)
{
    wxString flags;
    //Don't bother if nothing needs to be done
    if(compilerFlags.IsEmpty())
    {
        flags.Clear();
    }

    //gcc flag parser
    else if(compilerID==wxT("gcc") || compilerID==wxT("cygwin") || compilerID==wxT("arm-elf-gcc") || compilerID==wxT("tricoregcc") || compilerID==wxT("ppcgcc") || compilerID==wxT("msp430gcc"))
    {
        if(StringExists(compilerFlags, wxT("-msse")))
        {
            flags << _T("\", \"EnableSSE");
        }
        if(StringExists(compilerFlags, wxT("-msse2")))
        {
            flags << _T("\", \"EnableSSE2");
        }
        if(StringExists(compilerFlags, wxT("-Wall")))
        {
            flags << _T("\", \"ExtraWarnings");
        }
        if(StringExists(compilerFlags, wxT("-Wfatal-errors")))
        {
            flags << _T("\", \"FatalWarnings");
        }
        if(StringExists(compilerFlags, wxT("-ffast-math")))
        {
            flags << _T("\", \"FloatFast");
        }
        if(StringExists(compilerFlags, wxT("-ffloat-store")))
        {
            flags << _T("\", \"FloatStrict");
        }
        if(StringExists(compilerFlags, wxT("-fno-exceptions")))
        {
            flags << _T("\", \"NoExceptions");
        }
        if(StringExists(compilerFlags, wxT("-fomit-frame-pointer")))
        {
            flags << _T("\", \"NoFramePointer");
        }
        if(StringExists(compilerFlags, wxT("-fno-rtti")))
        {
            flags << _T("\", \"NoRTTI");
        }
        if(StringExists(compilerFlags, wxT("-O2")))
        {
            flags << _T("\", \"Optimize");
        }
        if(StringExists(compilerFlags, wxT("-Os")))
        {
            flags << _T("\", \"OptimizeSize");
        }
        if(StringExists(compilerFlags, wxT("-O3")))
        {
            flags << _T("\", \"OptimizeSpeed");
        }
        if(StringExists(compilerFlags, wxT("-g")))
        {
            flags << _T("\", \"Symbols");
        }
        flags << _T("\"");
        flags = flags.Mid(3);
    }

    //msvc flag parser
    else if(compilerID==wxT("msvc10") || compilerID==wxT("msvc8") || compilerID==wxT("msvctk"))
    {
        if(StringExists(compilerFlags, wxT("/arch:SSE")))
        {
            flags << _T("\", \"EnableSSE");
        }
        if(StringExists(compilerFlags, wxT("/arch:SSE2")))
        {
            flags << _T("\", \"EnableSSE2");
        }
        if(StringExists(compilerFlags, wxT("/Wall")))
        {
            flags << _T("\", \"ExtraWarnings");
        }
        if(StringExists(compilerFlags, wxT("/WX")))
        {
            flags << _T("\", \"FatalWarnings");
        }
        if(StringExists(compilerFlags, wxT("/fp:fast")))
        {
            flags << _T("\", \"FloatFast");
        }
        if(StringExists(compilerFlags, wxT("/fp:precise")) || StringExists(compilerFlags, wxT("/fp:strict")))
        {
            flags << _T("\", \"FloatStrict");
        }
        if(StringExists(compilerFlags, wxT("/EH-")))
        {
            flags << _T("\", \"NoExceptions");
        }
        if(StringExists(compilerFlags, wxT("/Oy")))
        {
            flags << _T("\", \"NoFramePointer");
        }
        if(StringExists(compilerFlags, wxT("/GR-")))
        {
            flags << _T("\", \"NoRTTI");
        }
        if(StringExists(compilerFlags, wxT("/Ox")) || StringExists(compilerFlags, wxT("/Og")))
        {
            flags << _T("\", \"Optimize");
        }
        if(StringExists(compilerFlags, wxT("/O1")) || StringExists(compilerFlags, wxT("/Os")))
        {
            flags << _T("\", \"OptimizeSize");
        }
        if(StringExists(compilerFlags, wxT("/O2")) || StringExists(compilerFlags, wxT("/Ot")))
        {
            flags << _T("\", \"OptimizeSpeed");
        }
        if(StringExists(compilerFlags, wxT("/Zi")))
        {
            flags << _T("\", \"Symbols");
        }
        flags << _T("\"");
        flags = flags.Mid(3);
    }
    return flags;
}

wxString PremakeExporter::EmitDefines(const wxArrayString& compilerFlags)
{
    wxString defines;
    //Don't bother if nothing needs to be done
    if(compilerFlags.IsEmpty())
        return defines;
    for(unsigned int i=0; i < compilerFlags.Count(); i++)
    {
        if(compilerFlags[i].Left(2)==wxT("-D") || compilerFlags[i].Left(2)==wxT("/D"))
        {
            defines << _T("\", \"") << compilerFlags[i].Mid(2);
        }
    }
    //Return if nothing is found
    if(defines.IsEmpty())
        return defines;
    defines << _T("\"");
    defines = defines.Mid(3);
    return defines;
}

wxString PremakeExporter::AddEscapes(const wxString& source)
{
    wxString output = source;
    output.Replace(wxT("\\"), wxT("\\\\"));
    output.Replace(wxT("\""), wxT("\\\""));
    return output;
}

wxString PremakeExporter::ReplVars(const wxString& source, bool repl)
{
    if(repl)
    {
        wxString output = Manager::Get()->GetMacrosManager()->ReplaceMacros(source);
        return output;
    }
    else
    {
        return source;
    }
}

void PremakeExporter::ExportStraight(bool EvaluateVars)
{
    //Write configurations
    m_content << m_project->GetBuildTarget(0)->GetTitle();
    for(int i=1; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << _T("\", \"") << m_project->GetBuildTarget(i)->GetTitle();
    }

    //Write project
    m_content << _T("\" }\r\n\r\n    project \"") << m_project->GetTitle();



    //Acquire file list
    wxString bufferString;
    bool tmpBool = true;
    for(FilesList::iterator i = m_project->GetFilesList().begin(); i != m_project->GetFilesList().end(); i++)
    {
        ProjectFile* pf = *i;
        bufferString << _T("\", \"") << ConvertSlash(pf->relativeFilename);
        if(tmpBool && (bufferString.Right(2) == wxT(".c")))
        {
            tmpBool = false;
        }
    }

    bufferString << _T("\" }");

    //Determine language (may not be accurate)
    m_content << _T("\"\r\n        language \"C");
    if(tmpBool)
    {
        m_content << _T("++");
    }
    m_content << _T("\"\r\n        files {") << bufferString.Mid(2);

    //Emit flags
    bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetCompilerOptions());
    if(!bufferString.IsEmpty())
    {
        m_content << _T("\r\n        flags { ") << bufferString << _T(" }");
    }

    //Emit defines
    bufferString = EmitDefines(m_project->GetCompilerOptions());
    if(!bufferString.IsEmpty())
    {
        m_content << _T("\r\n        defines { ") << bufferString << _T(" }");
    }

    //Write libraries
    if(!m_project->GetLinkLibs().IsEmpty())
    {
        m_content << _T("\r\n        links { \"") << m_project->GetLinkLibs()[0];
        for(unsigned int i=1; i < m_project->GetLinkLibs().GetCount(); i++)
        {
            m_content << _T("\", \"") << m_project->GetLinkLibs()[i];
        }
        m_content << _T("\" }");
    }

    //Compiler search directories
    if(!m_project->GetIncludeDirs().IsEmpty())
    {
        m_content << _T("\r\n        includedirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetIncludeDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Linker search directories
    if(!m_project->GetLibDirs().IsEmpty())
    {
        m_content << _T("\r\n        libdirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetLibDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Resource compiler search directories
    if(!m_project->GetResourceIncludeDirs().IsEmpty())
    {
        m_content << _T("\r\n        resincludedirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetResourceIncludeDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Pre-build commands
    if(!m_project->GetCommandsBeforeBuild().IsEmpty())
    {
        m_content << _T("\r\n        prebuildcommands { \"");
        m_content << AddEscapes(m_project->GetCommandsBeforeBuild()[0]);
        for(unsigned int i=1; i < m_project->GetCommandsBeforeBuild().GetCount(); i++)
        {
            m_content << _T("\", \"") << AddEscapes(m_project->GetCommandsBeforeBuild()[i]);
        }
        m_content << _T("\" }");
    }

    //Post-build commands
    if(!m_project->GetCommandsAfterBuild().IsEmpty())
    {
        m_content << _T("\r\n        postbuildcommands { \"");
        m_content << AddEscapes(m_project->GetCommandsAfterBuild()[0]);
        for(unsigned int i=1; i < m_project->GetCommandsAfterBuild().GetCount(); i++)
        {
            m_content << _T("\", \"") << AddEscapes(m_project->GetCommandsAfterBuild()[i]);
        }
        m_content << _T("\" }");
    }

    //Write configurations
    for(int i=0; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << _T("\r\n\r\n        configuration \"");
        m_content << m_project->GetBuildTarget(i)->GetTitle();

        //Write target type
        m_content << _T("\"\r\n            kind \"");
        switch(m_project->GetBuildTarget(i)->GetTargetType())
        {
        case ttExecutable:
            m_content << _T("WindowedApp");
            break;
        case ttConsoleOnly:
            m_content << _T("ConsoleApp");
            break;
        case ttStaticLib:
            m_content << _T("StaticLib");
            break;
        case ttDynamicLib:
            m_content << _T("SharedLib");
            break;
        default:
            ;
        }

        //Write target name
        wxFileName fn(m_project->GetBuildTarget(i)->GetOutputFilename());
        m_content << _T("\"\r\n            targetname (\"");
        if(m_project->GetBuildTarget(i)->GetTargetType() == ttStaticLib)
        {
            m_content << RemLib(fn.GetName());
        }
        else
        {
            m_content << fn.GetName();
        }
        m_content << _T("\")");

        //Write target directory
        if(!fn.GetPath().IsEmpty())
        {
            m_content << _T("\r\n            targetdir (\"");
            m_content << fn.GetPath(true, wxPATH_UNIX) << _T("\")");
        }

        //Write object directory
        m_content << _T("\r\n            objdir (\"");
        m_content << ConvertSlash(m_project->GetBuildTarget(i)->GetObjectOutput());
        m_content << _T("\")");

        //Emit flags
        bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetBuildTarget(i)->GetCompilerOptions());
        if(!bufferString.IsEmpty())
        {
            m_content << _T("\r\n            flags { ") << bufferString << _T(" }");
        }

        //Emit defines
        bufferString = EmitDefines(m_project->GetBuildTarget(i)->GetCompilerOptions());
        if(!bufferString.IsEmpty())
        {
            m_content << _T("\r\n            defines { ") << bufferString << _T(" }");
        }

        //Write libraries
        if(!m_project->GetBuildTarget(i)->GetLinkLibs().IsEmpty())
        {
            m_content << _T("\r\n            links { \"") << m_project->GetBuildTarget(i)->GetLinkLibs()[0];
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetLinkLibs().GetCount(); j++)
            {
                m_content << _T("\", \"") << m_project->GetBuildTarget(i)->GetLinkLibs()[j];
            }
            m_content << _T("\" }");
        }

        //Compiler search directories
        if(!m_project->GetBuildTarget(i)->GetIncludeDirs().IsEmpty())
        {
            m_content << _T("\r\n            includedirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetIncludeDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Linker search directories
        if(!m_project->GetBuildTarget(i)->GetLibDirs().IsEmpty())
        {
            m_content << _T("\r\n            libdirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetLibDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Resource compiler search directories
        if(!m_project->GetBuildTarget(i)->GetResourceIncludeDirs().IsEmpty())
        {
            m_content << _T("\r\n            resincludedirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetResourceIncludeDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Pre-build commands
        if(!m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().IsEmpty())
        {
            m_content << _T("\r\n            prebuildcommands { \"");
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[0]);
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().GetCount(); j++)
            {
                m_content << _T("\", \"") << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[j]);
            }
            m_content << _T("\" }");
        }

        //Post-build commands
        if(!m_project->GetBuildTarget(i)->GetCommandsAfterBuild().IsEmpty())
        {
            m_content << _T("\r\n            postbuildcommands { \"");
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[0]);
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetCommandsAfterBuild().GetCount(); j++)
            {
                m_content << _T("\", \"") << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[j]);
            }
            m_content << _T("\" }");
        }
    }
}

void PremakeExporter::ExportUpgrade(bool EvaluateVars)
{
    m_content << _T("default\" }");

    //Emit flags
    wxString bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetCompilerOptions());
    if(!bufferString.IsEmpty())
    {
        m_content << _T("\r\n    flags { ") << bufferString << _T(" }");
    }

    //Emit defines
    bufferString = EmitDefines(m_project->GetCompilerOptions());
    if(!bufferString.IsEmpty())
    {
        m_content << _T("\r\n    defines { ") << bufferString << _T(" }");
    }

    //Write libraries
    if(!m_project->GetLinkLibs().IsEmpty())
    {
        m_content << _T("\r\n    links { \"") << m_project->GetLinkLibs()[0];
        for(unsigned int i=1; i < m_project->GetLinkLibs().GetCount(); i++)
        {
            m_content << _T("\", \"") << m_project->GetLinkLibs()[i];
        }
        m_content << _T("\" }");
    }

    //Compiler search directories
    if(!m_project->GetIncludeDirs().IsEmpty())
    {
        m_content << _T("\r\n    includedirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetIncludeDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetIncludeDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Linker search directories
    if(!m_project->GetLibDirs().IsEmpty())
    {
        m_content << _T("\r\n    libdirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetLibDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetLibDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Resource compiler search directories
    if(!m_project->GetResourceIncludeDirs().IsEmpty())
    {
        m_content << _T("\r\n    resincludedirs { \"");
        m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[0], EvaluateVars));
        for(unsigned int i=1; i < m_project->GetResourceIncludeDirs().GetCount(); i++)
        {
            m_content << _T("\", \"");
            m_content << ConvertSlash(ReplVars(m_project->GetResourceIncludeDirs()[i], EvaluateVars));
        }
        m_content << _T("\" }");
    }

    //Pre-build commands
    if(!m_project->GetCommandsBeforeBuild().IsEmpty())
    {
        m_content << _T("\r\n    prebuildcommands { \"");
        m_content << AddEscapes(m_project->GetCommandsBeforeBuild()[0]);
        for(unsigned int i=1; i < m_project->GetCommandsBeforeBuild().GetCount(); i++)
        {
            m_content << _T("\", \"") << AddEscapes(m_project->GetCommandsBeforeBuild()[i]);
        }
        m_content << _T("\" }");
    }

    //Post-build commands
    if(!m_project->GetCommandsAfterBuild().IsEmpty())
    {
        m_content << _T("\r\n    postbuildcommands { \"");
        m_content << AddEscapes(m_project->GetCommandsAfterBuild()[0]);
        for(unsigned int i=1; i < m_project->GetCommandsAfterBuild().GetCount(); i++)
        {
            m_content << _T("\", \"") << AddEscapes(m_project->GetCommandsAfterBuild()[i]);
        }
        m_content << _T("\" }");
    }

    //Write projects
    for(int i=0; i < m_project->GetBuildTargetsCount(); i++)
    {
        m_content << _T("\r\n\r\n    project \"");
        m_content << m_project->GetBuildTarget(i)->GetTitle();

        //Acquire file list
        bufferString.Clear();
        bool tmpBool = true;
        for(FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
        {
            ProjectFile* pf = *j;
            bufferString << _T("\", \"") << ConvertSlash(pf->relativeFilename);
            if(tmpBool && (bufferString.Right(2) == wxT(".c")))
            {
                tmpBool = false;
            }
        }

        //Determine language (may not be accurate)
        m_content << _T("\"\r\n        language \"C");
        if(tmpBool)
        {
            m_content << _T("++");
        }
        m_content << _T("\"\r\n        files {");

        m_content << ConvertSlash(bufferString.Mid(2));

        //Write target type
        m_content << _T("\" }\r\n        kind \"");
        switch(m_project->GetBuildTarget(i)->GetTargetType())
        {
        case ttExecutable:
            m_content << _T("WindowedApp");
            break;
        case ttConsoleOnly:
            m_content << _T("ConsoleApp");
            break;
        case ttStaticLib:
            m_content << _T("StaticLib");
            break;
        case ttDynamicLib:
            m_content << _T("SharedLib");
            break;
        default:
            ;
        }

        //Write target name
        wxFileName fn(m_project->GetBuildTarget(i)->GetOutputFilename());
        m_content << _T("\"\r\n        targetname (\"");
        if(m_project->GetBuildTarget(i)->GetTargetType() == ttStaticLib)
        {
            m_content << RemLib(fn.GetName());
        }
        else
        {
            m_content << fn.GetName();
        }
        m_content << _T("\")");

        //Write target directory
        if(!fn.GetPath().IsEmpty())
        {
            m_content << _T("\r\n        targetdir (\"");
            m_content << fn.GetPath(true, wxPATH_UNIX) << _T("\")");
        }

        //Write object directory
        m_content << _T("\r\n        objdir (\"");
        m_content << ConvertSlash(m_project->GetBuildTarget(i)->GetObjectOutput());
        m_content << _T("\")");

        //Emit flags
        bufferString = EmitFlags(m_project->GetCompilerID(), m_project->GetBuildTarget(i)->GetCompilerOptions());
        if(!bufferString.IsEmpty())
        {
            m_content << _T("\r\n        flags { ") << bufferString << _T(" }");
        }

        //Emit defines
        bufferString = EmitDefines(m_project->GetBuildTarget(i)->GetCompilerOptions());
        if(!bufferString.IsEmpty())
        {
            m_content << _T("\r\n        defines { ") << bufferString << _T(" }");
        }

        //Write libraries
        if(!m_project->GetBuildTarget(i)->GetLinkLibs().IsEmpty())
        {
            m_content << _T("\r\n        links { \"") << m_project->GetBuildTarget(i)->GetLinkLibs()[0];
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetLinkLibs().GetCount(); j++)
            {
                m_content << _T("\", \"") << m_project->GetBuildTarget(i)->GetLinkLibs()[j];
            }
            m_content << _T("\" }");
        }

        //Compiler search directories
        if(!m_project->GetBuildTarget(i)->GetIncludeDirs().IsEmpty())
        {
            m_content << _T("\r\n        includedirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetIncludeDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetIncludeDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Linker search directories
        if(!m_project->GetBuildTarget(i)->GetLibDirs().IsEmpty())
        {
            m_content << _T("\r\n        libdirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetLibDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetLibDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Resource compiler search directories
        if(!m_project->GetBuildTarget(i)->GetResourceIncludeDirs().IsEmpty())
        {
            m_content << _T("\r\n        resincludedirs { \"");
            m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[0], EvaluateVars));
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetResourceIncludeDirs().GetCount(); j++)
            {
                m_content << _T("\", \"");
                m_content << ConvertSlash(ReplVars(m_project->GetBuildTarget(i)->GetResourceIncludeDirs()[j], EvaluateVars));
            }
            m_content << _T("\" }");
        }

        //Pre-build commands
        if(!m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().IsEmpty())
        {
            m_content << _T("\r\n        prebuildcommands { \"");
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[0]);
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetCommandsBeforeBuild().GetCount(); j++)
            {
                m_content << _T("\", \"") << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsBeforeBuild()[j]);
            }
            m_content << _T("\" }");
        }

        //Post-build commands
        if(!m_project->GetBuildTarget(i)->GetCommandsAfterBuild().IsEmpty())
        {
            m_content << _T("\r\n        postbuildcommands { \"");
            m_content << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[0]);
            for(unsigned int j=1; j < m_project->GetBuildTarget(i)->GetCommandsAfterBuild().GetCount(); j++)
            {
                m_content << _T("\", \"") << AddEscapes(m_project->GetBuildTarget(i)->GetCommandsAfterBuild()[j]);
            }
            m_content << _T("\" }");
        }
        m_content << _T("\r\n            configuration \"default\"");
    }
}

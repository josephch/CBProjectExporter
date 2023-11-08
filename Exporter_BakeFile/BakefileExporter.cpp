#include <sdk.h> // Code::Blocks SDK
#include <tinyxml/tinywxuni.h>
#include "BakefileExporter.h"

BakefileExporter::BakefileExporter()
{
    m_project = Manager::Get()->GetProjectManager()->GetActiveProject();
    //ctor
}

BakefileExporter::~BakefileExporter()
{
    //dtor
}

wxArrayString BakefileExporter::EmitFlags(const wxString& compilerID, const wxArrayString& compilerFlags, wxArrayString& unrecognizedFlags)
{
    wxArrayString flags;
    if(compilerID==wxT("gcc") || compilerID==wxT("cygwin") || compilerID==wxT("arm-elf-gcc") || compilerID==wxT("tricoregcc") || compilerID==wxT("ppcgcc") || compilerID==wxT("msp430gcc"))
    {
        if(StringExists(compilerFlags, wxT("-O2")) || StringExists(compilerFlags, wxT("-O3")))
        {
            flags.Add(wxT("speed"));
            unrecognizedFlags.Remove(wxT("-O2"));
            unrecognizedFlags.Remove(wxT("-O3"));
        }
        else if(StringExists(compilerFlags, wxT("-Os")))
        {
            flags.Add(wxT("size"));
            unrecognizedFlags.Remove(wxT("-Os"));
        }
        else
        {
            flags.Add(wxT("off"));
        }
        if(StringExists(compilerFlags, wxT("-g")))
        {
            flags.Add(wxT("on"));
            unrecognizedFlags.Remove(wxT("-g"));
        }
        else
        {
            flags.Add(wxT("off"));
        }
        if(StringExists(compilerFlags, wxT("-Wall")))
        {
            flags.Add(wxT("max"));
            unrecognizedFlags.Remove(wxT("-Wall"));
        }
        else if(StringExists(compilerFlags, wxT("-w")))
        {
            flags.Add(wxT("no"));
            unrecognizedFlags.Remove(wxT("-w"));
        }
        else
        {
            flags.Add(wxT("default"));
        }
        if(StringExists(compilerFlags, wxT("-fno-rtti")))
        {
            flags.Add(wxT("off"));
            unrecognizedFlags.Remove(wxT("-fno-rtti"));
        }
        else
        {
            flags.Add(wxT("on"));
        }
        if(StringExists(compilerFlags, wxT("-fno-exceptions")))
        {
            flags.Add(wxT("off"));
            unrecognizedFlags.Remove(wxT("-fno-exceptions"));
        }
        else
        {
            flags.Add(wxT("on"));
        }
    }

    else if(compilerID==wxT("msvc10") || compilerID==wxT("msvc8") || compilerID==wxT("msvctk"))
    {
        if(StringExists(compilerFlags, wxT("/Ox")) || StringExists(compilerFlags, wxT("/Og")) || StringExists(compilerFlags, wxT("/O2")) || StringExists(compilerFlags, wxT("/Ot")))
        {
            flags.Add(wxT("speed"));
            unrecognizedFlags.Remove(wxT("/Ox"));
            unrecognizedFlags.Remove(wxT("/Og"));
            unrecognizedFlags.Remove(wxT("/O2"));
            unrecognizedFlags.Remove(wxT("/Ot"));
        }
        else if(StringExists(compilerFlags, wxT("/O1")) || StringExists(compilerFlags, wxT("/Os")))
        {
            flags.Add(wxT("size"));
            unrecognizedFlags.Remove(wxT("/O1"));
            unrecognizedFlags.Remove(wxT("/Os"));
        }
        else
        {
            flags.Add(wxT("off"));
        }
        if(StringExists(compilerFlags, wxT("/Zi")))
        {
            flags.Add(wxT("on"));
            unrecognizedFlags.Remove(wxT("/Zi"));
        }
        else
        {
            flags.Add(wxT("off"));
        }
        if(StringExists(compilerFlags, wxT("/Wall")))
        {
            flags.Add(wxT("max"));
            unrecognizedFlags.Remove(wxT("/Wall"));
        }
        else if(StringExists(compilerFlags, wxT("/w")))
        {
            flags.Add(wxT("no"));
            unrecognizedFlags.Remove(wxT("/w"));
        }
        else
        {
            flags.Add(wxT("default"));
        }
        if(StringExists(compilerFlags, wxT("/GR-")))
        {
            flags.Add(wxT("off"));
            unrecognizedFlags.Remove(wxT("/GR-"));
        }
        else
        {
            flags.Add(wxT("on"));
        }
        if(StringExists(compilerFlags, wxT("/EH-")))
        {
            flags.Add(wxT("off"));
            unrecognizedFlags.Remove(wxT("/EH-"));
        }
        else
        {
            flags.Add(wxT("on"));
        }
    }
    return flags;
}

wxString BakefileExporter::GetOptions(const wxString& source)
{
    wxString output = source.AfterFirst(wxT('$'));
    //Manager::Get()->GetLogManager()->Log( source.AfterFirst(wxT('$')) );
    while(!output.IsEmpty())
    {
        if(output.Left(1) == wxT("("))
        {
            if(!output.BeforeFirst(wxT(')')).IsEmpty() && !StringExists(m_options, wxT("$") + output.BeforeFirst(wxT(')')) + wxT(")")))
            {
                m_options.Add(wxT("$") + output.BeforeFirst(wxT(')')) + wxT(")"));
            }
        }
        else if(!output.BeforeFirst(wxT(' ')).IsEmpty())
        {
            if(!StringExists(m_options, wxT("$(") + output.BeforeFirst(wxT(' ')) + wxT(")")))
            {
                m_options.Add(wxT("$(") + output.BeforeFirst(wxT(' ')) + wxT(")"));
            }
        }
        output = output.AfterFirst(wxT('$'));
    }
    output = source;
    output.Replace(wxT("$(#"), wxT("$("));
    return output;
}

void BakefileExporter::RunExport()
{
    //output file
    wxString fn(m_project->GetFilename());
    fn.RemoveLast(4);
    fn << _T(".bkl");
    TiXmlDocument m_content;

    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* element = new TiXmlElement("makefile");
	TiXmlElement* subElement;
	TiXmlElement* subSubElement;
	TiXmlText* text;
    wxArrayString tmpArray;
    wxString tmpString;
	for(int i=0; i < m_project->GetBuildTargetsCount(); i++)
    {
        switch(m_project->GetBuildTarget(i)->GetTargetType())
        {
        case ttExecutable:
            subElement = new TiXmlElement("exe");
            subSubElement = new TiXmlElement("app-type");
            text = new TiXmlText("gui");
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
            subSubElement = new TiXmlElement("exename");
            break;
        case ttConsoleOnly:
            subElement = new TiXmlElement("exe");
            subSubElement = new TiXmlElement("app-type");
            text = new TiXmlText("console");
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
            subSubElement = new TiXmlElement("exename");
            break;
        case ttStaticLib:
            subElement = new TiXmlElement("lib");
            subSubElement = new TiXmlElement("libname");
            break;
        case ttDynamicLib:
            subSubElement = new TiXmlElement("dllname");
            if(m_project->GetBuildTarget(i)->GetCreateStaticLib() || m_project->GetBuildTarget(i)->GetCreateDefFile())
            {
                subElement = new TiXmlElement("dll");
                wxFileName fn(m_project->GetBuildTarget(i)->GetOutputFilename());
                text = new TiXmlText(RemLib(fn.GetName()).ToAscii());
                subSubElement->LinkEndChild(text);
                subElement->LinkEndChild(subSubElement);
                subSubElement = new TiXmlElement("libname");
            }
            else
            {
                subElement = new TiXmlElement("module");
            }
            break;
        default:
            Manager::Get()->GetLogManager()->LogWarning(wxT("Warning: \"") + m_project->GetBuildTarget(i)->GetTitle() + wxT("\" is of an unrecognized target type; skipping..."));
            continue;
        }

        wxFileName m_tmpFileName(m_project->GetBuildTarget(i)->GetOutputFilename());
        text = new TiXmlText(RemLib(m_tmpFileName.GetName()).ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subElement->SetAttribute("id", ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());
        if(!m_tmpFileName.GetPath().IsEmpty())
        {
            subSubElement = new TiXmlElement("dirname");
            text = new TiXmlText(m_tmpFileName.GetPath(true, wxPATH_UNIX).BeforeLast(wxT('/')).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        tmpString.Clear();
        wxString tmpStringB;
        for(FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
        {
            ProjectFile* pf = *j;
            wxString cfn(pf->relativeFilename);
            //is header
            if(cfn.Right(2) == wxT(".h") || cfn.Right(4) == wxT(".hpp") || cfn.Right(4) == wxT(".hxx") || cfn.Right(3) == wxT(".hh"))
            {
                tmpStringB << _T(" ") << ConvertSlash(cfn);
            }
            else if(cfn.Right(4) == wxT(".cpp") || cfn.Right(2) == wxT(".c") || cfn.Right(3) == wxT(".cc") || cfn.Right(4) == wxT(".cxx"))
            {
                if(pf->compile)
                {
                    tmpString << _T(" ") << ConvertSlash(cfn);
                }
            }
            else if(cfn.Right(3) == wxT(".rc"))
            {
                subSubElement = new TiXmlElement("win32-res");
                text = new TiXmlText(ConvertSlash(cfn).ToAscii());
                subSubElement->LinkEndChild(text);
                subElement->LinkEndChild(subSubElement);
            }
        }
        subSubElement = new TiXmlElement("headers");
        text = new TiXmlText(tmpStringB.Mid(1).ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subSubElement = new TiXmlElement("sources");
        text = new TiXmlText(tmpString.Mid(1).ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);

        tmpArray = AppendOptionsArray(m_project->GetCompilerOptions(), m_project->GetBuildTarget(i)->GetCompilerOptions());
        wxArrayString unparsedFlags;
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            if(!(tmpArray[j].Left(2)==wxT("-D") || tmpArray[j].Left(2)==wxT("/D")) && unparsedFlags.Index(tmpArray[j]) == wxNOT_FOUND)
            {
                unparsedFlags.Add(tmpArray[j]);
            }
        }
        tmpArray = EmitFlags(m_project->GetBuildTarget(i)->GetCompilerID(), tmpArray, unparsedFlags);
        subSubElement = new TiXmlElement("optimize");
        text = new TiXmlText(tmpArray[0].ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subSubElement = new TiXmlElement("debug-info");
        text = new TiXmlText(tmpArray[1].ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subSubElement = new TiXmlElement("warnings");
        text = new TiXmlText(tmpArray[2].ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subSubElement = new TiXmlElement("cxx-rtti");
        text = new TiXmlText(tmpArray[3].ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subSubElement = new TiXmlElement("cxx-exceptions");
        text = new TiXmlText(tmpArray[4].ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);

        tmpArray = AppendOptionsArray(m_project->GetCompilerOptions(), m_project->GetBuildTarget(i)->GetCompilerOptions(), m_project->GetBuildTarget(i)->GetOptionRelation(ortCompilerOptions));
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            if(tmpArray[j].Left(2)==wxT("-D") || tmpArray[j].Left(2)==wxT("/D"))
            {
                subSubElement = new TiXmlElement("define");
                text = new TiXmlText(tmpArray[j].Mid(2).ToAscii());
                subSubElement->LinkEndChild(text);
                subElement->LinkEndChild(subSubElement);
            }
        }

        if(!unparsedFlags.IsEmpty())
        {
            subSubElement = new TiXmlElement("cppflags");
            tmpString.Clear();
            for(unsigned int j=0; j < unparsedFlags.GetCount(); j++)
            {
                tmpString += wxT(" ") + unparsedFlags[j];
            }
            text = new TiXmlText(tmpString.Mid(1).ToAscii());
            subSubElement->LinkEndChild(text);
            tmpArray = AppendOptionsArray(m_project->GetLinkerOptions(), m_project->GetBuildTarget(i)->GetLinkerOptions(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
            tmpString.Clear();
            for(unsigned int j=0; j < tmpArray.GetCount(); j++)
            {
                tmpString += wxT(" ") + tmpArray[j];
            }
            text = new TiXmlText(tmpString.Mid(1).ToAscii());
            TiXmlElement* ifElement = new TiXmlElement("if");
            wxString compilerID = m_project->GetBuildTarget(i)->GetCompilerID();

            if(compilerID==wxT("gcc") || compilerID==wxT("cygwin") || compilerID==wxT("arm-elf-gcc") || compilerID==wxT("tricoregcc") || compilerID==wxT("ppcgcc") || compilerID==wxT("msp430gcc"))
            {
                ifElement->SetAttribute("cond", "FORMAT=='gnu' or FORMAT=='mingw' or FORMAT=='xcode2'");
                ifElement->LinkEndChild(subSubElement);
                if(!tmpString.IsEmpty())
                {
                    subSubElement = new TiXmlElement("ldflags");
                    subSubElement->LinkEndChild(text);
                    ifElement->LinkEndChild(subSubElement);
                }
                subElement->LinkEndChild(ifElement);
            }
            else if(compilerID==wxT("msvc10") || compilerID==wxT("msvc8") || compilerID==wxT("msvctk"))
            {
                ifElement->SetAttribute("cond", "FORMAT=='msvc' or FORMAT=='msvc6prj' or FORMAT=='msevc4prj' or FORMAT=='msvs2003prj' or FORMAT=='msvs2005prj' or FORMAT=='msvs2008prj'");
                ifElement->LinkEndChild(subSubElement);
                {
                    subSubElement = new TiXmlElement("ldflags");
                    subSubElement->LinkEndChild(text);
                    ifElement->LinkEndChild(subSubElement);
                }
                subElement->LinkEndChild(ifElement);
            }
            else
            {
                subElement->LinkEndChild(subSubElement);
                if(!tmpString.IsEmpty())
                {
                    subSubElement = new TiXmlElement("ldflags");
                    subSubElement->LinkEndChild(text);
                    subElement->LinkEndChild(subSubElement);
                }
            }
        }

        //Compiler search directories
        tmpArray = AppendOptionsArray(m_project->GetIncludeDirs(), m_project->GetBuildTarget(i)->GetIncludeDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortIncludeDirs));
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("include");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        //Linker search directories
        tmpArray = AppendOptionsArray(m_project->GetLibDirs(), m_project->GetBuildTarget(i)->GetLibDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLibDirs));
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("lib-path");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("sys-lib");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        tmpArray = AppendOptionsArray(m_project->GetResourceIncludeDirs(), m_project->GetBuildTarget(i)->GetResourceIncludeDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortResDirs));
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("res-include");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        tmpArray = AppendOptionsArray(m_project->GetCommandsAfterBuild(), m_project->GetBuildTarget(i)->GetCommandsAfterBuild());
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("postlink-command");
            text = new TiXmlText(tmpArray[j].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        element->LinkEndChild(subElement);

        tmpArray = m_project->GetBuildTarget(i)->GetCommandsBeforeBuild();
        if(!tmpArray.IsEmpty())
        {
            subElement = new TiXmlElement("action");
            subElement->SetAttribute("id", (ReplSpace(m_project->GetBuildTarget(i)->GetTitle()) + wxT("-pre-build-command")).ToAscii());
            subSubElement = new TiXmlElement("dependency-of");
            text = new TiXmlText(ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
            for(unsigned int j=0; j < tmpArray.GetCount(); j++)
            {
                subSubElement = new TiXmlElement("command");
                text = new TiXmlText(tmpArray[j].ToAscii());
                subSubElement->LinkEndChild(text);
                subElement->LinkEndChild(subSubElement);
            }
            element->LinkEndChild(subElement);
        }
    }

    tmpArray = m_project->GetCommandsBeforeBuild();
    if(!tmpArray.IsEmpty())
    {
        subElement = new TiXmlElement("action");
        subElement->SetAttribute("id", "pre-build-command");
        for(int i=0; i < m_project->GetBuildTargetsCount(); i++)
        {
            subSubElement = new TiXmlElement("dependency-of");
            text = new TiXmlText(ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        for(unsigned int i=0; i < tmpArray.GetCount(); i++)
        {
            subSubElement = new TiXmlElement("command");
            text = new TiXmlText(tmpArray[i].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        element->LinkEndChild(subElement);
    }

    for(unsigned int i=0; i < m_project->GetVirtualBuildTargets().GetCount(); i++)
    {
        tmpString = m_project->GetVirtualBuildTargets()[i];
        subElement = new TiXmlElement("phony");
        subElement->SetAttribute("id", ReplSpace(tmpString).ToAscii());
        tmpArray = m_project->GetVirtualBuildTargetGroup(tmpString);
        for(unsigned int j=0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("depends");
            text = new TiXmlText(ReplSpace(tmpArray[j]).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }
        element->LinkEndChild(subElement);
    }

    /*subElement = new TiXmlElement("set");
    subElement->SetAttribute("var", "BUILDDIR");
    text = new TiXmlText(m_project->GetBuildTarget(0)->GetObjectOutput().ToAscii());
    subElement->LinkEndChild(text);
    element->InsertBeforeChild(element->FirstChild(), *subElement);*/

    tmpArray.Clear();
    for(unsigned int i=0; i < m_options.GetCount(); i++)
    {
        if(m_options[i].Contains(wxT("#")))
        {
            tmpString = m_options[i].BeforeFirst(wxT(')')).Mid(3);
            if(tmpString.Contains(wxT(".")))
            {
                subElement = new TiXmlElement("set");
                subElement->SetAttribute("var", tmpString.ToAscii());
                wxString tmpStringB = Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)m_options[i]);
                tmpStringB.Replace(Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)(m_options[i].BeforeFirst(wxT('.')) + wxT(")"))), wxT(""));
                text = new TiXmlText((wxT("$(") + tmpString.BeforeFirst(wxT('.')) + wxT(")") + tmpStringB).ToAscii());
                subElement->LinkEndChild(text);
                TiXmlNode* iterate = element->FirstChild();
                while(wxString::FromAscii(iterate->Value()) == wxT("option"))
                {
                    iterate = iterate->NextSiblingElement();
                }
                element->InsertBeforeChild(iterate, *subElement);
                if(!StringExists(m_options, m_options[i].BeforeFirst(wxT('.')) + wxT(")")) && !StringExists(tmpArray, m_options[i].BeforeFirst(wxT('.')) + wxT(")")))
                {
                    tmpArray.Add(m_options[i].BeforeFirst(wxT('.')) + wxT(")"));
                    subElement = new TiXmlElement("option");
                    subElement->SetAttribute("name", tmpString.BeforeFirst(wxT('.')).ToAscii());
                    subElement->SetAttribute("category", "path");
                    subSubElement = new TiXmlElement("default-value");
                    text = new TiXmlText(Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)(m_options[i].BeforeFirst(wxT('.')) + wxT(")"))).ToAscii());
                    subSubElement->LinkEndChild(text);
                    subElement->LinkEndChild(subSubElement);
                    element->InsertBeforeChild(element->FirstChild(), *subElement);
                }
                continue;
            }
        }
        else
        {
            tmpString = m_options[i].BeforeFirst(wxT(')')).Mid(2);
        }
        subElement = new TiXmlElement("option");
        subElement->SetAttribute("name", tmpString.ToAscii());
        subElement->SetAttribute("category", "path");
        subSubElement = new TiXmlElement("default-value");
        text = new TiXmlText(Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)m_options[i]).ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        element->InsertBeforeChild(element->FirstChild(), *subElement);
    }
	m_content.LinkEndChild(decl);
	m_content.LinkEndChild(element);

    m_content.SaveFile(fn.ToAscii());
}

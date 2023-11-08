// System include files

// CB include files
#include <sdk.h> // Code::Blocks SDK
#include "logmanager.h"
#include "manager.h"
#include "macrosmanager.h"
#include <tinyxml.h>

// ProjectExporter include files
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

wxArrayString BakefileExporter::EmitFlags(const wxString & compilerID, const wxArrayString & compilerFlags, wxArrayString & unrecognizedFlags)
{
    wxArrayString flags;

    if (
        compilerID == "gcc"          ||
        compilerID == "cygwin"       ||
        compilerID == "arm-elf-gcc"  ||
        compilerID == "tricoregcc"   ||
        compilerID == "ppcgcc"       ||
        compilerID == "msp430gcc"     ||
        compilerID.StartsWith("gcc")
    )
    {
        if (StringExists(compilerFlags, "-O2") || StringExists(compilerFlags, "-O3"))
        {
            flags.Add("speed");
            unrecognizedFlags.Remove("-O2");
            unrecognizedFlags.Remove("-O3");
        }
        else
            if (StringExists(compilerFlags, "-Os"))
            {
                flags.Add("size");
                unrecognizedFlags.Remove("-Os");
            }
            else
            {
                flags.Add("off");
            }

        if (StringExists(compilerFlags, "-g"))
        {
            flags.Add("on");
            unrecognizedFlags.Remove("-g");
        }
        else
        {
            flags.Add("off");
        }

        if (StringExists(compilerFlags, "-Wall"))
        {
            flags.Add("max");
            unrecognizedFlags.Remove("-Wall");
        }
        else
            if (StringExists(compilerFlags, "-w"))
            {
                flags.Add("no");
                unrecognizedFlags.Remove("-w");
            }
            else
            {
                flags.Add("default");
            }

        if (StringExists(compilerFlags, "-fno-rtti"))
        {
            flags.Add("off");
            unrecognizedFlags.Remove("-fno-rtti");
        }
        else
        {
            flags.Add("on");
        }

        if (StringExists(compilerFlags, "-fno-exceptions"))
        {
            flags.Add("off");
            unrecognizedFlags.Remove("-fno-exceptions");
        }
        else
        {
            flags.Add("on");
        }
    }
    else
        if (compilerID == "msvc10" || compilerID == "msvc8" || compilerID == "msvctk")
        {
            if (StringExists(compilerFlags, "/Ox") || StringExists(compilerFlags, "/Og") || StringExists(compilerFlags, "/O2") || StringExists(compilerFlags, "/Ot"))
            {
                flags.Add("speed");
                unrecognizedFlags.Remove("/Ox");
                unrecognizedFlags.Remove("/Og");
                unrecognizedFlags.Remove("/O2");
                unrecognizedFlags.Remove("/Ot");
            }
            else
                if (StringExists(compilerFlags, "/O1") || StringExists(compilerFlags, "/Os"))
                {
                    flags.Add("size");
                    unrecognizedFlags.Remove("/O1");
                    unrecognizedFlags.Remove("/Os");
                }
                else
                {
                    flags.Add("off");
                }

            if (StringExists(compilerFlags, "/Zi"))
            {
                flags.Add("on");
                unrecognizedFlags.Remove("/Zi");
            }
            else
            {
                flags.Add("off");
            }

            if (StringExists(compilerFlags, "/Wall"))
            {
                flags.Add("max");
                unrecognizedFlags.Remove("/Wall");
            }
            else
                if (StringExists(compilerFlags, "/w"))
                {
                    flags.Add("no");
                    unrecognizedFlags.Remove("/w");
                }
                else
                {
                    flags.Add("default");
                }

            if (StringExists(compilerFlags, "/GR-"))
            {
                flags.Add("off");
                unrecognizedFlags.Remove("/GR-");
            }
            else
            {
                flags.Add("on");
            }

            if (StringExists(compilerFlags, "/EH-"))
            {
                flags.Add("off");
                unrecognizedFlags.Remove("/EH-");
            }
            else
            {
                flags.Add("on");
            }
        }

    return flags;
}

wxString BakefileExporter::GetOptions(const wxString & source)
{
    wxString output = source.AfterFirst('$');

    //Manager::Get()->GetLogManager()->Log( source.AfterFirst('$') );
    while (!output.IsEmpty())
    {
        if (output.Left(1) == "(")
        {
            if (!output.BeforeFirst(')').IsEmpty() && !StringExists(m_options, "$" + output.BeforeFirst(')') + ")"))
            {
                m_options.Add("$" + output.BeforeFirst(')') + ")");
            }
        }
        else
            if (!output.BeforeFirst(' ').IsEmpty())
            {
                if (!StringExists(m_options, "$(" + output.BeforeFirst(' ') + ")"))
                {
                    m_options.Add("$(" + output.BeforeFirst(' ') + ")");
                }
            }

        output = output.AfterFirst('$');
    }

    output = source;
    output.Replace("$(#", "$(");
    return output;
}

void BakefileExporter::RunExport()
{
    //output file
    wxFileName wxfFileName(m_project->GetFilename());
    wxString fn = wxString::Format("%s%s.bkl", wxfFileName.GetPathWithSep(), wxfFileName.GetName());
    TiXmlDocument m_content;
    TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement * element = new TiXmlElement("makefile");
    TiXmlElement * subElement;
    TiXmlElement * subSubElement;
    TiXmlText * text;
    wxArrayString tmpArray;
    wxString tmpString;

    for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
    {
        switch (m_project->GetBuildTarget(i)->GetTargetType())
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

                if (m_project->GetBuildTarget(i)->GetCreateStaticLib() || m_project->GetBuildTarget(i)->GetCreateDefFile())
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
                Manager::Get()->GetLogManager()->LogWarning("Warning: \"" + m_project->GetBuildTarget(i)->GetTitle() + "\" is of an unrecognized target type; skipping...");
                continue;
        }

        wxFileName m_tmpFileName(m_project->GetBuildTarget(i)->GetOutputFilename());
        text = new TiXmlText(RemLib(m_tmpFileName.GetName()).ToAscii());
        subSubElement->LinkEndChild(text);
        subElement->LinkEndChild(subSubElement);
        subElement->SetAttribute("id", ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());

        if (!m_tmpFileName.GetPath().IsEmpty())
        {
            subSubElement = new TiXmlElement("dirname");
            text = new TiXmlText(m_tmpFileName.GetPath(true, wxPATH_UNIX).BeforeLast('/').ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        tmpString.Clear();
        wxString tmpStringB;

        for (FilesList::iterator j = m_project->GetBuildTarget(i)->GetFilesList().begin(); j != m_project->GetBuildTarget(i)->GetFilesList().end(); j++)
        {
            ProjectFile * pf = *j;
            wxString cfn(pf->relativeFilename);

            //is header
            if (cfn.Right(2) == ".h" || cfn.Right(4) == ".hpp" || cfn.Right(4) == ".hxx" || cfn.Right(3) == ".hh")
            {
                tmpStringB << " " << ConvertSlash(cfn);
            }
            else
                if (cfn.Right(4) == ".cpp" || cfn.Right(2) == ".c" || cfn.Right(3) == ".cc" || cfn.Right(4) == ".cxx")
                {
                    if (pf->compile)
                    {
                        tmpString << " " << ConvertSlash(cfn);
                    }
                }
                else
                    if (cfn.Right(3) == ".rc")
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

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            if (!(tmpArray[j].Left(2) == "-D" || tmpArray[j].Left(2) == "/D") && unparsedFlags.Index(tmpArray[j]) == wxNOT_FOUND)
            {
                unparsedFlags.Add(tmpArray[j]);
            }
        }

        tmpArray = EmitFlags(m_project->GetBuildTarget(i)->GetCompilerID(), tmpArray, unparsedFlags);
        size_t 	arrCount = tmpArray.GetCount();

        if (arrCount > 0)
        {
            subSubElement = new TiXmlElement("optimize");
            text = new TiXmlText(tmpArray[0].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        if (arrCount > 1)
        {
            subSubElement = new TiXmlElement("debug-info");
            text = new TiXmlText(tmpArray[1].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        if (arrCount > 2)
        {
            subSubElement = new TiXmlElement("warnings");
            text = new TiXmlText(tmpArray[2].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        if (arrCount > 3)
        {
            subSubElement = new TiXmlElement("cxx-rtti");
            text = new TiXmlText(tmpArray[3].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        if (arrCount > 4)
        {
            subSubElement = new TiXmlElement("cxx-exceptions");
            text = new TiXmlText(tmpArray[4].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        tmpArray = AppendOptionsArray(m_project->GetCompilerOptions(), m_project->GetBuildTarget(i)->GetCompilerOptions(), m_project->GetBuildTarget(i)->GetOptionRelation(ortCompilerOptions));

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            if (tmpArray[j].Left(2) == "-D" || tmpArray[j].Left(2) == "/D")
            {
                subSubElement = new TiXmlElement("define");
                text = new TiXmlText(tmpArray[j].Mid(2).ToAscii());
                subSubElement->LinkEndChild(text);
                subElement->LinkEndChild(subSubElement);
            }
        }

        if (!unparsedFlags.IsEmpty())
        {
            subSubElement = new TiXmlElement("cppflags");
            tmpString.Clear();

            for (unsigned int j = 0; j < unparsedFlags.GetCount(); j++)
            {
                tmpString += " " + unparsedFlags[j];
            }

            text = new TiXmlText(tmpString.Mid(1).ToAscii());
            subSubElement->LinkEndChild(text);
            tmpArray = AppendOptionsArray(m_project->GetLinkerOptions(), m_project->GetBuildTarget(i)->GetLinkerOptions(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));
            tmpString.Clear();

            for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
            {
                tmpString += " " + tmpArray[j];
            }

            text = new TiXmlText(tmpString.Mid(1).ToAscii());
            TiXmlElement * ifElement = new TiXmlElement("if");
            wxString compilerID = m_project->GetBuildTarget(i)->GetCompilerID();

            if (compilerID == "gcc" || compilerID == "cygwin" || compilerID == "arm-elf-gcc" || compilerID == "tricoregcc" || compilerID == "ppcgcc" || compilerID == "msp430gcc")
            {
                ifElement->SetAttribute("cond", "FORMAT=='gnu' or FORMAT=='mingw' or FORMAT=='xcode2'");
                ifElement->LinkEndChild(subSubElement);

                if (!tmpString.IsEmpty())
                {
                    subSubElement = new TiXmlElement("ldflags");
                    subSubElement->LinkEndChild(text);
                    ifElement->LinkEndChild(subSubElement);
                }

                subElement->LinkEndChild(ifElement);
            }
            else
                if (compilerID == "msvc10" || compilerID == "msvc8" || compilerID == "msvctk")
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

                    if (!tmpString.IsEmpty())
                    {
                        subSubElement = new TiXmlElement("ldflags");
                        subSubElement->LinkEndChild(text);
                        subElement->LinkEndChild(subSubElement);
                    }
                }
        }

        //Compiler search directories
        tmpArray = AppendOptionsArray(m_project->GetIncludeDirs(), m_project->GetBuildTarget(i)->GetIncludeDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortIncludeDirs));

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("include");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        //Linker search directories
        tmpArray = AppendOptionsArray(m_project->GetLibDirs(), m_project->GetBuildTarget(i)->GetLibDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLibDirs));

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("lib-path");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        tmpArray = AppendOptionsArray(m_project->GetLinkLibs(), m_project->GetBuildTarget(i)->GetLinkLibs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortLinkerOptions));

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("sys-lib");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        tmpArray = AppendOptionsArray(m_project->GetResourceIncludeDirs(), m_project->GetBuildTarget(i)->GetResourceIncludeDirs(), m_project->GetBuildTarget(i)->GetOptionRelation(ortResDirs));

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("res-include");
            text = new TiXmlText(GetOptions(ConvertSlash(tmpArray[j])).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        tmpArray = AppendOptionsArray(m_project->GetCommandsAfterBuild(), m_project->GetBuildTarget(i)->GetCommandsAfterBuild());

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
        {
            subSubElement = new TiXmlElement("postlink-command");
            text = new TiXmlText(tmpArray[j].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        element->LinkEndChild(subElement);
        tmpArray = m_project->GetBuildTarget(i)->GetCommandsBeforeBuild();

        if (!tmpArray.IsEmpty())
        {
            subElement = new TiXmlElement("action");
            subElement->SetAttribute("id", (ReplSpace(m_project->GetBuildTarget(i)->GetTitle()) + "-pre-build-command").ToAscii());
            subSubElement = new TiXmlElement("dependency-of");
            text = new TiXmlText(ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);

            for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
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

    if (!tmpArray.IsEmpty())
    {
        subElement = new TiXmlElement("action");
        subElement->SetAttribute("id", "pre-build-command");

        for (int i = 0; i < m_project->GetBuildTargetsCount(); i++)
        {
            subSubElement = new TiXmlElement("dependency-of");
            text = new TiXmlText(ReplSpace(m_project->GetBuildTarget(i)->GetTitle()).ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        for (unsigned int i = 0; i < tmpArray.GetCount(); i++)
        {
            subSubElement = new TiXmlElement("command");
            text = new TiXmlText(tmpArray[i].ToAscii());
            subSubElement->LinkEndChild(text);
            subElement->LinkEndChild(subSubElement);
        }

        element->LinkEndChild(subElement);
    }

    for (unsigned int i = 0; i < m_project->GetVirtualBuildTargets().GetCount(); i++)
    {
        tmpString = m_project->GetVirtualBuildTargets()[i];
        subElement = new TiXmlElement("phony");
        subElement->SetAttribute("id", ReplSpace(tmpString).ToAscii());
        tmpArray = m_project->GetVirtualBuildTargetGroup(tmpString);

        for (unsigned int j = 0; j < tmpArray.GetCount(); j++)
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

    for (unsigned int i = 0; i < m_options.GetCount(); i++)
    {
        if (m_options[i].Contains("#"))
        {
            tmpString = m_options[i].BeforeFirst(')').Mid(3);

            if (tmpString.Contains("."))
            {
                subElement = new TiXmlElement("set");
                subElement->SetAttribute("var", tmpString.ToAscii());
                wxString tmpStringB = Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)m_options[i]);
                tmpStringB.Replace(Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)(m_options[i].BeforeFirst('.') + ")")), "");
                text = new TiXmlText(cbU2C("$(" + tmpString.BeforeFirst('.') + ")" + tmpStringB.ToAscii()));
                subElement->LinkEndChild(text);
                TiXmlNode * iterate = element->FirstChild();

                while (wxString::FromAscii(iterate->Value()) == "option")
                {
                    iterate = iterate->NextSiblingElement();
                }

                element->InsertBeforeChild(iterate, *subElement);

                if (!StringExists(m_options, m_options[i].BeforeFirst('.') + ")") && !StringExists(tmpArray, m_options[i].BeforeFirst('.') + ")"))
                {
                    tmpArray.Add(m_options[i].BeforeFirst('.') + ")");
                    subElement = new TiXmlElement("option");
                    subElement->SetAttribute("name", tmpString.BeforeFirst('.').ToAscii());
                    subElement->SetAttribute("category", "path");
                    subSubElement = new TiXmlElement("default-value");
                    text = new TiXmlText(Manager::Get()->GetMacrosManager()->ReplaceMacros((const wxString)(m_options[i].BeforeFirst('.') + ")")).ToAscii());
                    subSubElement->LinkEndChild(text);
                    subElement->LinkEndChild(subSubElement);
                    element->InsertBeforeChild(element->FirstChild(), *subElement);
                }

                continue;
            }
        }
        else
        {
            tmpString = m_options[i].BeforeFirst(')').Mid(2);
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

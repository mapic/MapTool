/***************************************************************
 * Name:      App.cpp
 * Purpose:   Code for Application Class
 * Author:
 * Created:
 * Copyright:
 * License:
 **************************************************************/

#ifdef WX_PRECOMP
#include "wx_pch.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif //__BORLANDC__

#include "App.h"
#include "MapTool.h"

#include <wx/image.h>
#include <wx/regex.h>
#include "wx/stdpaths.h"
#include <wx/utils.h>
#include "wx/propgrid/propgrid.h"

IMPLEMENT_APP(wxWidgets_templateApp);

std::map<int, LevelDetial> *g_NpcLevelList = NULL;

void LoadNpcLevelList();

bool wxWidgets_templateApp::OnInit()
{
	bool wxsOK = true;
	wxInitAllImageHandlers();
	wxPropertyGridInterface::RegisterAdditionalEditors();

	//Tmx settings
    TmxReaderSetting::LoadConfing();

	m_mapTool = new MapTool(NULL);

	//Start tmx map file reader help server
	wxString exepath;
	exepath = wxStandardPaths::Get().GetExecutablePath();
	exepath = wxFileName::FileName(exepath).GetPath(wxPATH_GET_VOLUME);
	wxExecute(exepath+"/resource/TmxReadHelper/TmxReadHelper.exe");

	LoadNpcLevelList();

	m_mapTool->Show(true);

	return wxsOK;
}

int wxWidgets_templateApp::OnExit()
{
	if(g_NpcLevelList != NULL) delete g_NpcLevelList;
	Settings::TheSetting.SaveSetting();
	return 0;
}

//int wxWidgets_templateApp::FilterEvent(wxEvent& event)
//{
//	wxEventType type = event.GetEventType();
//	if(type == wxEVT_KEY_UP ||
//		type == wxEVT_KEY_DOWN)
//	{
//		wxKeyEvent *kEvent = (wxKeyEvent*)&event;
//		if(kEvent->GetKeyCode() == WXK_CONTROL)
//		{
//			m_mapTool->RedrawMapView();
//		}
//	}
//	return Event_Skip;
//}

void LoadNpcLevelList()
{
	g_NpcLevelList = new std::map<int, LevelDetial>;

	wxString filePath = wxStandardPaths::Get().GetExecutablePath();
    filePath = wxFileName::FileName(filePath).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    filePath += wxT("ini\\level\\level-npc.ini");
    wxRegEx regex;
    if(wxFileName::FileExists(filePath))
	{
		wxTextFile file(filePath);
		file.Open();
		if(file.IsOpened())
		{
			long level, value;
			wxString name;
			size_t count = file.GetLineCount();
			size_t i;
			for(i = 0; i < count;)
			{
				LevelDetial detail;
                regex.Compile(wxT("\\[Level([0-9]+)\\]"));
                if(regex.Matches(file[i]) &&
				   !regex.GetMatch(file[i]).IsEmpty())
				{
					regex.GetMatch(file[i], 1).ToLong(&level);
					regex.Compile(wxT("(.*)=([0-9]+)"));
					i++;
					while(i < count &&
						regex.Matches(file[i]) &&
						!regex.GetMatch(file[i]).IsEmpty())
					{
						name = regex.GetMatch(file[i], 1);
						regex.GetMatch(file[i], 2).ToLong(&value);
						if(name == wxT("Life"))
							detail.Life = value;
						else if(name == wxT("Attack"))
							detail.Attack = value;
						else if(name == wxT("Attack2"))
							detail.Attack2 = value;
						else if(name == wxT("Defend"))
							detail.Defend = value;
						else if(name == wxT("Defend2"))
							detail.Defend2 = value;
						else if(name == wxT("Evade"))
							detail.Evade = value;
						i++;
					}
					(*g_NpcLevelList)[level] = detail;
				}
				i++;
			}
			file.Close();
		}
	}
}

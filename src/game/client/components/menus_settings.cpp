/* (c) Magnus Auvinen. See licence.txt in the root of the distribution for more information. */
/* If you are missing that file, acquire a complete release at teeworlds.com.                */

#include <base/math.h>

#include <engine/engine.h>
#include <engine/graphics.h>
#include <engine/storage.h>
#include <engine/textrender.h>
#include <engine/shared/config.h>
#include <engine/shared/linereader.h>

#include <game/generated/protocol.h>
#include <game/generated/client_data.h>

#include <game/client/components/sounds.h>
#include <game/client/ui.h>
#include <game/client/render.h>
#include <game/client/gameclient.h>
#include <game/client/animstate.h>
#include <game/client/webapp.h>
#include <game/localization.h>

#include "binds.h"
#include "countryflags.h"
#include "menus.h"
#include "skins.h"

CMenusKeyBinder CMenus::m_Binder;

CMenusKeyBinder::CMenusKeyBinder()
{
	m_TakeKey = false;
	m_GotKey = false;
}

bool CMenusKeyBinder::OnInput(IInput::CEvent Event)
{
	if(m_TakeKey)
	{
		if(Event.m_Flags&IInput::FLAG_PRESS)
		{
			m_Key = Event;
			m_GotKey = true;
			m_TakeKey = false;
		}
		return true;
	}

	return false;
}

void CMenus::RenderSettingsGeneral(CUIRect MainView)
{
	char aBuf[128];
	CUIRect List, Label, Button, Left, Right, Game, Client;
	MainView.HSplitTop(200.0f, &List, &MainView);
	MainView.HSplitTop(210.0f, &Game, &Client);

	// game
	{
		// headline
		Game.HSplitTop(30.0f, &Label, &Game);
		UI()->DoLabelScaled(&Label, Localize("Game"), 20.0f, -1);
		Game.Margin(5.0f, &Game);
		Game.VSplitMid(&Left, &Right);
		Left.VSplitRight(5.0f, &Left, 0);
		Right.VMargin(5.0f, &Right);

		// dynamic camera
		Left.HSplitTop(20.0f, &Button, &Left);
		static int s_DynamicCameraButton = 0;
		if(DoButton_CheckBox(&s_DynamicCameraButton, Localize("Dynamic Camera"), g_Config.m_ClMouseDeadzone != 0, &Button))
		{
			if(g_Config.m_ClMouseDeadzone)
			{
				g_Config.m_ClMouseFollowfactor = 0;
				g_Config.m_ClMouseMaxDistance = 400;
				g_Config.m_ClMouseDeadzone = 0;
			}
			else
			{
				g_Config.m_ClMouseFollowfactor = 60;
				g_Config.m_ClMouseMaxDistance = 1000;
				g_Config.m_ClMouseDeadzone = 300;
			}
		}

		// weapon pickup
		Left.HSplitTop(2.5f, 0, &Left);
		Left.HSplitTop(20.0f, &Button, &Left);
		if(DoButton_CheckBox(&g_Config.m_ClAutoswitchWeapons, Localize("Switch weapon on pickup"), g_Config.m_ClAutoswitchWeapons, &Button))
			g_Config.m_ClAutoswitchWeapons ^= 1;

		// show hud
		Left.HSplitTop(2.5f, 0, &Left);
		Left.HSplitTop(20.0f, &Button, &Left);
		if(DoButton_CheckBox(&g_Config.m_ClShowhud, Localize("Show ingame HUD"), g_Config.m_ClShowhud, &Button))
			g_Config.m_ClShowhud ^= 1;
		
		// show client id in scoreboard
		Left.HSplitTop(2.5f, 0, &Left);
		Left.HSplitTop(20.0f, &Button, &Left);
		if(DoButton_CheckBox(&g_Config.m_ClScoreboardClientID, Localize("Show Client ID in scoreboard"), g_Config.m_ClScoreboardClientID, &Button))
			g_Config.m_ClScoreboardClientID ^= 1;
		
		// show ghost
		Left.HSplitTop(2.5f, 0, &Left);
		Left.HSplitTop(20.0f, &Button, &Left);
		if(DoButton_CheckBox(&g_Config.m_ClShowGhost, Localize("Show ghost"), g_Config.m_ClShowGhost, &Button))
			g_Config.m_ClShowGhost ^= 1;

		// anti rainbow
		Left.HSplitTop(2.5f, 0, &Left);
		Left.HSplitTop(20.0f, &Button, &Left);
		if(DoButton_CheckBox(&g_Config.m_ClAntiRainbow, Localize("Anti rainbow"), g_Config.m_ClAntiRainbow, &Button))
			g_Config.m_ClAntiRainbow ^= 1;
		
		if(g_Config.m_ClAntiRainbow)
		{
			Left.HSplitTop(2.5f, 0, &Left);
			Left.VSplitLeft(20.0f, 0, &Left);
			Left.HSplitTop(20.0f, &Label, &Left);
			Left.HSplitTop(20.0f, &Button, &Left);
			str_format(aBuf, sizeof(aBuf), "%s: %i", Localize("Allowed color changes"), g_Config.m_ClAntiRainbowCount);
			UI()->DoLabelScaled(&Label, aBuf, 13.0f, -1);
			Button.HMargin(2.0f, &Button);
			g_Config.m_ClAntiRainbowCount = (int)(DoScrollbarH(&g_Config.m_ClAntiRainbowCount, &Button, g_Config.m_ClAntiRainbowCount/10.0f)*10.0f+0.1f);
		}
		
		// name plates
		Right.HSplitTop(20.0f, &Button, &Right);
		if(DoButton_CheckBox(&g_Config.m_ClNameplates, Localize("Show name plates"), g_Config.m_ClNameplates, &Button))
			g_Config.m_ClNameplates ^= 1;

		if(g_Config.m_ClNameplates)
		{
			Right.HSplitTop(2.5f, 0, &Right);
			Right.VSplitLeft(30.0f, 0, &Right);
			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClNameplatesAlways, Localize("Always show name plates"), g_Config.m_ClNameplatesAlways, &Button))
				g_Config.m_ClNameplatesAlways ^= 1;

			Right.HSplitTop(2.5f, 0, &Right);
			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClNameplateClientID, Localize("Show Client ID in name plates"), g_Config.m_ClNameplateClientID, &Button))
				g_Config.m_ClNameplateClientID ^= 1;
			
			Right.HSplitTop(2.5f, 0, &Right);
			Right.HSplitTop(20.0f, &Label, &Right);
			Right.HSplitTop(20.0f, &Button, &Right);
			str_format(aBuf, sizeof(aBuf), "%s: %i", Localize("Name plates size"), g_Config.m_ClNameplatesSize);
			UI()->DoLabelScaled(&Label, aBuf, 13.0f, -1);
			Button.HMargin(2.0f, &Button);
			g_Config.m_ClNameplatesSize = (int)(DoScrollbarH(&g_Config.m_ClNameplatesSize, &Button, g_Config.m_ClNameplatesSize/100.0f)*100.0f+0.1f);

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClNameplatesTeamcolors, Localize("Use team colors for name plates"), g_Config.m_ClNameplatesTeamcolors, &Button))
				g_Config.m_ClNameplatesTeamcolors ^= 1;
		}
	}

	// client
	{
		// headline
		Client.HSplitTop(30.0f, &Label, &Client);
		UI()->DoLabelScaled(&Label, Localize("Client"), 20.0f, -1);
		Client.Margin(5.0f, &Client);
		Client.VSplitMid(&Left, &Right);
		Left.VSplitRight(5.0f, &Left, 0);
		Right.VMargin(5.0f, &Right);

		// auto demo settings
		{
			Left.HSplitTop(20.0f, &Button, &Left);
			if(DoButton_CheckBox(&g_Config.m_ClAutoDemoRecord, Localize("Automatically record demos"), g_Config.m_ClAutoDemoRecord, &Button))
				g_Config.m_ClAutoDemoRecord ^= 1;

			Right.HSplitTop(20.0f, &Button, &Right);
			if(DoButton_CheckBox(&g_Config.m_ClAutoScreenshot, Localize("Automatically take game over screenshot"), g_Config.m_ClAutoScreenshot, &Button))
				g_Config.m_ClAutoScreenshot ^= 1;

			Left.HSplitTop(10.0f, 0, &Left);
			Left.VSplitLeft(20.0f, 0, &Left);
			Left.HSplitTop(20.0f, &Label, &Left);
			Button.VSplitRight(20.0f, &Button, 0);
			char aBuf[64];
			if(g_Config.m_ClAutoDemoMax)
				str_format(aBuf, sizeof(aBuf), "%s: %i", Localize("Max demos"), g_Config.m_ClAutoDemoMax);
			else
				str_format(aBuf, sizeof(aBuf), "%s: %s", Localize("Max demos"), Localize("no limit"));
			UI()->DoLabelScaled(&Label, aBuf, 13.0f, -1);
			Left.HSplitTop(20.0f, &Button, 0);
			Button.HMargin(2.0f, &Button);
			g_Config.m_ClAutoDemoMax = static_cast<int>(DoScrollbarH(&g_Config.m_ClAutoDemoMax, &Button, g_Config.m_ClAutoDemoMax/1000.0f)*1000.0f+0.1f);

			Right.HSplitTop(10.0f, 0, &Right);
			Right.VSplitLeft(20.0f, 0, &Right);
			Right.HSplitTop(20.0f, &Label, &Right);
			Button.VSplitRight(20.0f, &Button, 0);
			if(g_Config.m_ClAutoScreenshotMax)
				str_format(aBuf, sizeof(aBuf), "%s: %i", Localize("Max Screenshots"), g_Config.m_ClAutoScreenshotMax);
			else
				str_format(aBuf, sizeof(aBuf), "%s: %s", Localize("Max Screenshots"), Localize("no limit"));
			UI()->DoLabelScaled(&Label, aBuf, 13.0f, -1);
			Right.HSplitTop(20.0f, &Button, 0);
			Button.HMargin(2.0f, &Button);
			g_Config.m_ClAutoScreenshotMax = static_cast<int>(DoScrollbarH(&g_Config.m_ClAutoScreenshotMax, &Button, g_Config.m_ClAutoScreenshotMax/1000.0f)*1000.0f+0.1f);
		}
	}
	
	// font selector
	RenderFontSelection(List);
}

void CMenus::RenderSettingsPlayer(CUIRect MainView)
{
	CUIRect Button, Label;
	MainView.HSplitTop(10.0f, 0, &MainView);

	// player name
	MainView.HSplitTop(20.0f, &Button, &MainView);
	Button.VSplitLeft(80.0f, &Label, &Button);
	Button.VSplitLeft(150.0f, &Button, 0);
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Name"));
	UI()->DoLabelScaled(&Label, aBuf, 14.0, -1);
	static float s_OffsetName = 0.0f;
	if(DoEditBox(g_Config.m_PlayerName, &Button, g_Config.m_PlayerName, sizeof(g_Config.m_PlayerName), 14.0f, &s_OffsetName))
		m_NeedSendinfo = true;

	// player clan
	MainView.HSplitTop(5.0f, 0, &MainView);
	MainView.HSplitTop(20.0f, &Button, &MainView);
	Button.VSplitLeft(80.0f, &Label, &Button);
	Button.VSplitLeft(150.0f, &Button, 0);
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Clan"));
	UI()->DoLabelScaled(&Label, aBuf, 14.0, -1);
	static float s_OffsetClan = 0.0f;
	if(DoEditBox(g_Config.m_PlayerClan, &Button, g_Config.m_PlayerClan, sizeof(g_Config.m_PlayerClan), 14.0f, &s_OffsetClan))
		m_NeedSendinfo = true;

	// country flag selector
	MainView.HSplitTop(20.0f, 0, &MainView);
	static float s_ScrollValue = 0.0f;
	int OldSelected = -1;
	UiDoListboxStart(&s_ScrollValue, &MainView, 50.0f, Localize("Country"), "", m_pClient->m_pCountryFlags->Num(), 6, OldSelected, s_ScrollValue);

	for(int i = 0; i < m_pClient->m_pCountryFlags->Num(); ++i)
	{
		const CCountryFlags::CCountryFlag *pEntry = m_pClient->m_pCountryFlags->Get(i);
		if(pEntry == 0)
			continue;

		if(pEntry->m_CountryCode == g_Config.m_PlayerCountry)
			OldSelected = i;

		CListboxItem Item = UiDoListboxNextItem(&pEntry->m_CountryCode, OldSelected == i);
		if(Item.m_Visible)
		{
			Item.m_Rect.Margin(10.0f, &Item.m_Rect);
			float OldWidth = Item.m_Rect.w;
			Item.m_Rect.w = Item.m_Rect.h*2;
			Item.m_Rect.x += (OldWidth-Item.m_Rect.w)/ 2.0f;
			Graphics()->TextureSet(pEntry->m_Texture);
			Graphics()->QuadsBegin();
			Graphics()->SetColor(1.0f, 1.0f, 1.0f, 1.0f);
			IGraphics::CQuadItem QuadItem(Item.m_Rect.x, Item.m_Rect.y, Item.m_Rect.w, Item.m_Rect.h);
			Graphics()->QuadsDrawTL(&QuadItem, 1);
			Graphics()->QuadsEnd();
		}
	}

	const int NewSelected = UiDoListboxEnd(&s_ScrollValue, 0);
	if(OldSelected != NewSelected)
	{
		g_Config.m_PlayerCountry = m_pClient->m_pCountryFlags->Get(NewSelected)->m_CountryCode;
		m_NeedSendinfo = true;
	}
}

void CMenus::RenderSettingsTee(CUIRect MainView)
{
	CUIRect Button, Label, Right;
	MainView.HSplitTop(10.0f, 0, &MainView);
	MainView.VSplitMid(0, &Right);

	// skin info
	const CSkins::CSkin *pOwnSkin = m_pClient->m_pSkins->Get(m_pClient->m_pSkins->Find(g_Config.m_PlayerSkin));
	CTeeRenderInfo OwnSkinInfo;
	if(g_Config.m_PlayerUseCustomColor)
	{
		OwnSkinInfo.m_Texture = pOwnSkin->m_ColorTexture;
		OwnSkinInfo.m_ColorBody = m_pClient->m_pSkins->GetColorV4(g_Config.m_PlayerColorBody);
		OwnSkinInfo.m_ColorFeet = m_pClient->m_pSkins->GetColorV4(g_Config.m_PlayerColorFeet);
	}
	else
	{
		OwnSkinInfo.m_Texture = pOwnSkin->m_OrgTexture;
		OwnSkinInfo.m_ColorBody = vec4(1.0f, 1.0f, 1.0f, 1.0f);
		OwnSkinInfo.m_ColorFeet = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	OwnSkinInfo.m_Size = 50.0f*UI()->Scale();

	MainView.HSplitTop(20.0f, &Label, &MainView);
	Label.VSplitLeft(230.0f, &Label, 0);
	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Your skin"));
	UI()->DoLabelScaled(&Label, aBuf, 14.0f, -1);

	MainView.HSplitTop(50.0f, &Label, &MainView);
	Label.VSplitLeft(230.0f, &Label, 0);
	RenderTools()->DrawUIRect(&Label, vec4(1.0f, 1.0f, 1.0f, 0.25f), CUI::CORNER_ALL, 10.0f);
	RenderTools()->RenderTee(CAnimState::GetIdle(), &OwnSkinInfo, 0, vec2(1, 0), vec2(Label.x+30.0f, Label.y+28.0f));
	Label.HSplitTop(15.0f, 0, &Label);;
	Label.VSplitLeft(70.0f, 0, &Label);
	UI()->DoLabelScaled(&Label, g_Config.m_PlayerSkin, 14.0f, -1, 150.0f);

	// custom colour selector
	MainView.HSplitTop(20.0f, 0, &MainView);
	MainView.HSplitTop(20.0f, &Button, &MainView);
	Button.VSplitLeft(230.0f, &Button, 0);
	if(DoButton_CheckBox(&g_Config.m_PlayerColorBody, Localize("Custom colors"), g_Config.m_PlayerUseCustomColor, &Button))
	{
		g_Config.m_PlayerUseCustomColor = g_Config.m_PlayerUseCustomColor?0:1;
		m_NeedSendinfo = true;
	}

	MainView.HSplitTop(5.0f, 0, &MainView);
	MainView.HSplitTop(82.5f, &Label, &MainView);
	if(g_Config.m_PlayerUseCustomColor)
	{
		CUIRect aRects[2];
		Label.VSplitMid(&aRects[0], &aRects[1]);
		aRects[0].VSplitRight(10.0f, &aRects[0], 0);
		aRects[1].VSplitLeft(10.0f, 0, &aRects[1]);

		int *paColors[2];
		paColors[0] = &g_Config.m_PlayerColorBody;
		paColors[1] = &g_Config.m_PlayerColorFeet;

		const char *paParts[] = {
			Localize("Body"),
			Localize("Feet")};
		const char *paLabels[] = {
			Localize("Hue"),
			Localize("Sat."),
			Localize("Lht.")};
		static int s_aColorSlider[2][3] = {{0}};

		for(int i = 0; i < 2; i++)
		{
			aRects[i].HSplitTop(20.0f, &Label, &aRects[i]);
			UI()->DoLabelScaled(&Label, paParts[i], 14.0f, -1);
			aRects[i].VSplitLeft(20.0f, 0, &aRects[i]);
			aRects[i].HSplitTop(2.5f, 0, &aRects[i]);

			int PrevColor = *paColors[i];
			int Color = 0;
			for(int s = 0; s < 3; s++)
			{
				aRects[i].HSplitTop(20.0f, &Label, &aRects[i]);
				Label.VSplitLeft(100.0f, &Label, &Button);
				Button.HMargin(2.0f, &Button);

				float k = ((PrevColor>>((2-s)*8))&0xff) / 255.0f;
				k = DoScrollbarH(&s_aColorSlider[i][s], &Button, k);
				Color <<= 8;
				Color += clamp((int)(k*255), 0, 255);
				UI()->DoLabelScaled(&Label, paLabels[s], 14.0f, -1);
			}

			if(PrevColor != Color)
				m_NeedSendinfo = true;

			*paColors[i] = Color;
		}
	}

	// skin selector
	MainView.HSplitTop(20.0f, 0, &MainView);
	static bool s_InitSkinlist = true;
	static sorted_array<const CSkins::CSkin *> s_paSkinList;
	static float s_ScrollValue = 0.0f;
	if(s_InitSkinlist)
	{
		s_paSkinList.clear();
		for(int i = 0; i < m_pClient->m_pSkins->Num(); ++i)
		{
			const CSkins::CSkin *s = m_pClient->m_pSkins->Get(i);
			// no special skins
			if(s->m_aName[0] == 'x' && s->m_aName[1] == '_')
				continue;
			s_paSkinList.add(s);
		}
		s_InitSkinlist = false;
	}

	int OldSelected = -1;
	UiDoListboxStart(&s_InitSkinlist, &MainView, 50.0f, Localize("Skins"), "", s_paSkinList.size(), 4, OldSelected, s_ScrollValue);

	for(int i = 0; i < s_paSkinList.size(); ++i)
	{
		const CSkins::CSkin *s = s_paSkinList[i];
		if(s == 0)
			continue;

		if(str_comp(s->m_aName, g_Config.m_PlayerSkin) == 0)
			OldSelected = i;

		CListboxItem Item = UiDoListboxNextItem(&s_paSkinList[i], OldSelected == i);
		if(Item.m_Visible)
		{
			CTeeRenderInfo Info;
			if(g_Config.m_PlayerUseCustomColor)
			{
				Info.m_Texture = s->m_ColorTexture;
				Info.m_ColorBody = m_pClient->m_pSkins->GetColorV4(g_Config.m_PlayerColorBody);
				Info.m_ColorFeet = m_pClient->m_pSkins->GetColorV4(g_Config.m_PlayerColorFeet);
			}
			else
			{
				Info.m_Texture = s->m_OrgTexture;
				Info.m_ColorBody = vec4(1.0f, 1.0f, 1.0f, 1.0f);
				Info.m_ColorFeet = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			}

			Info.m_Size = UI()->Scale()*50.0f;
			Item.m_Rect.HSplitTop(5.0f, 0, &Item.m_Rect); // some margin from the top
			RenderTools()->RenderTee(CAnimState::GetIdle(), &Info, 0, vec2(1.0f, 0.0f), vec2(Item.m_Rect.x+Item.m_Rect.w/2, Item.m_Rect.y+Item.m_Rect.h/2));

			if(g_Config.m_Debug)
			{
				vec3 BloodColor = g_Config.m_PlayerUseCustomColor ? m_pClient->m_pSkins->GetColorV3(g_Config.m_PlayerColorBody) : s->m_BloodColor;
				Graphics()->TextureSet(-1);
				Graphics()->QuadsBegin();
				Graphics()->SetColor(BloodColor.r, BloodColor.g, BloodColor.b, 1.0f);
				IGraphics::CQuadItem QuadItem(Item.m_Rect.x, Item.m_Rect.y, 12.0f, 12.0f);
				Graphics()->QuadsDrawTL(&QuadItem, 1);
				Graphics()->QuadsEnd();
			}
		}
	}

	const int NewSelected = UiDoListboxEnd(&s_ScrollValue, 0);
	if(OldSelected != NewSelected)
	{
		mem_copy(g_Config.m_PlayerSkin, s_paSkinList[NewSelected]->m_aName, sizeof(g_Config.m_PlayerSkin));
		m_NeedSendinfo = true;
	}

	// laser color
	{
		CUIRect Laser;
		Right.HSplitTop(100.0f, 0, &Laser);
		Laser.VSplitLeft(40.0f, 0, &Laser);
		Right.VSplitLeft(10.0f, 0, &Right);

		int *pColor;
		pColor = &g_Config.m_ClLaserColor;

		const char *pParts = Localize("Laser color");
		const char *paLabels[] = {
			Localize("Hue"),
			Localize("Sat."),
			Localize("Lht.")};
		static int s_aColorSlider[3] = {0};

		Right.HSplitTop(20.0f, &Label, &Right);
		UI()->DoLabelScaled(&Label, pParts, 14.0f, -1);
		Right.VSplitLeft(20.0f, 0, &Right);
		Right.HSplitTop(2.5f, 0, &Right);

		int PrevColor = *pColor;
		int Color = 0;
		for(int s = 0; s < 3; s++)
		{
			Right.HSplitTop(20.0f, &Label, &Right);
			Label.VSplitLeft(100.0f, &Label, &Button);
			Button.HMargin(2.0f, &Button);

			float k = ((PrevColor>>((2-s)*8))&0xff)  / 255.0f;
			k = DoScrollbarH(&s_aColorSlider[s], &Button, k);
			Color <<= 8;
			Color += clamp((int)(k*255), 0, 255);
			UI()->DoLabelScaled(&Label, paLabels[s], 14.0f, -1);
		}

		*pColor = Color;

		// darw laser
		vec2 From = vec2(Laser.x, Laser.y);
		vec2 Pos = vec2(Laser.x+Laser.w-10.0f, Laser.y);

		Graphics()->TextureSet(-1);
		Graphics()->QuadsBegin();

		// do outline
		vec4 OuterColor(0.075f, 0.075f, 0.25f, 1.0f);
		Graphics()->SetColor(OuterColor.r, OuterColor.g, OuterColor.b, 1.0f);
		vec2 Out = vec2(0.0f, -1.0f) * (3.15f);

		IGraphics::CFreeformItem Freeform(
				From.x-Out.x, From.y-Out.y,
				From.x+Out.x, From.y+Out.y,
				Pos.x-Out.x, Pos.y-Out.y,
				Pos.x+Out.x, Pos.y+Out.y);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);

		// do inner	
		Out = vec2(0.0f, -1.0f) * (2.25f);
		vec3 Rgb = m_pClient->m_pSkins->GetColorV3(g_Config.m_ClLaserColor);
		Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f); // center

		Freeform = IGraphics::CFreeformItem(
				From.x-Out.x, From.y-Out.y,
				From.x+Out.x, From.y+Out.y,
				Pos.x-Out.x, Pos.y-Out.y,
				Pos.x+Out.x, Pos.y+Out.y);
		Graphics()->QuadsDrawFreeform(&Freeform, 1);
	
		Graphics()->QuadsEnd();
		
		// render head
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_PARTICLES].m_Id);
		Graphics()->QuadsBegin();

		RenderTools()->SelectSprite(SPRITE_PART_SPLAT01);
		Graphics()->SetColor(OuterColor.r, OuterColor.g, OuterColor.b, 1.0f);
		IGraphics::CQuadItem QuadItem(Pos.x, Pos.y, 24, 24);
		Graphics()->QuadsDraw(&QuadItem, 1);
		Graphics()->SetColor(Rgb.r, Rgb.g, Rgb.b, 1.0f);
		QuadItem = IGraphics::CQuadItem(Pos.x, Pos.y, 20, 20);
		Graphics()->QuadsDraw(&QuadItem, 1);

		Graphics()->QuadsEnd();

		// draw laser weapon
		Graphics()->TextureSet(g_pData->m_aImages[IMAGE_GAME].m_Id);
		Graphics()->QuadsBegin();

		RenderTools()->SelectSprite(SPRITE_WEAPON_RIFLE_BODY);
		RenderTools()->DrawSprite(Laser.x, Laser.y, 60.0f);

		Graphics()->QuadsEnd();
	}
}


typedef void (*pfnAssignFuncCallback)(CConfiguration *pConfig, int Value);

static CKeyInfo gs_aKeys[] =
{
	{ "Move left", "+left", 0},		// Localize - these strings are localized within CLocConstString
	{ "Move right", "+right", 0 },
	{ "Jump", "+jump", 0 },
	{ "Fire", "+fire", 0 },
	{ "Hook", "+hook", 0 },
	{ "Hammer", "+weapon1", 0 },
	{ "Pistol", "+weapon2", 0 },
	{ "Shotgun", "+weapon3", 0 },
	{ "Grenade", "+weapon4", 0 },
	{ "Rifle", "+weapon5", 0 },
	{ "Next weapon", "+nextweapon", 0 },
	{ "Prev. weapon", "+prevweapon", 0 },
	{ "Vote yes", "vote yes", 0 },
	{ "Vote no", "vote no", 0 },
	{ "Chat", "chat all", 0 },
	{ "Team chat", "chat team", 0 },
	{ "Show chat", "+show_chat", 0 },
	{ "Emoticon", "+emote", 0 },
	{ "Spectator mode", "+spectate", 0 },
	{ "Spectate next", "spectate_next", 0 },
	{ "Spectate previous", "spectate_previous", 0 },
	{ "Console", "toggle_local_console", 0 },
	{ "Remote console", "toggle_remote_console", 0 },
	{ "Screenshot", "screenshot", 0 },
	{ "Scoreboard", "+scoreboard", 0 },
};

/*	This is for scripts/update_localization.py to work, don't remove!
	Localize("Move left");Localize("Move right");Localize("Jump");Localize("Fire");Localize("Hook");Localize("Hammer");
	Localize("Pistol");Localize("Shotgun");Localize("Grenade");Localize("Rifle");Localize("Next weapon");Localize("Prev. weapon");
	Localize("Vote yes");Localize("Vote no");Localize("Chat");Localize("Team chat");Localize("Show chat");Localize("Emoticon");
	Localize("Spectator mode");Localize("Spectate next");Localize("Spectate previous");Localize("Console");Localize("Remote console");Localize("Screenshot");Localize("Scoreboard");
*/

const int g_KeyCount = sizeof(gs_aKeys) / sizeof(CKeyInfo);

void CMenus::UiDoGetButtons(int Start, int Stop, CUIRect View)
{
	for (int i = Start; i < Stop; i++)
	{
		CKeyInfo &Key = gs_aKeys[i];
		CUIRect Button, Label;
		View.HSplitTop(20.0f, &Button, &View);
		Button.VSplitLeft(135.0f, &Label, &Button);

		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%s:", (const char *)Key.m_Name);

		UI()->DoLabelScaled(&Label, aBuf, 13.0f, -1);
		int OldId = Key.m_KeyId;
		int NewId = DoKeyReader((void *)&gs_aKeys[i].m_Name, &Button, OldId);
		if(NewId != OldId)
		{
			if(OldId != 0 || NewId == 0)
				m_pClient->m_pBinds->Bind(OldId, "");
			if(NewId != 0)
				m_pClient->m_pBinds->Bind(NewId, gs_aKeys[i].m_pCommand);
		}
		View.HSplitTop(5.0f, 0, &View);
	}
}

void CMenus::RenderSettingsControls(CUIRect MainView)
{
	// this is kinda slow, but whatever
	for(int i = 0; i < g_KeyCount; i++)
		gs_aKeys[i].m_KeyId = 0;

	for(int KeyId = 0; KeyId < KEY_LAST; KeyId++)
	{
		const char *pBind = m_pClient->m_pBinds->Get(KeyId);
		if(!pBind[0])
			continue;

		for(int i = 0; i < g_KeyCount; i++)
			if(str_comp(pBind, gs_aKeys[i].m_pCommand) == 0)
			{
				gs_aKeys[i].m_KeyId = KeyId;
				break;
			}
	}

	CUIRect MovementSettings, WeaponSettings, VotingSettings, ChatSettings, MiscSettings, ResetButton;
	MainView.VSplitLeft(MainView.w/2-5.0f, &MovementSettings, &VotingSettings);

	// movement settings
	{
		MovementSettings.HSplitTop(MainView.h/3+60.0f, &MovementSettings, &WeaponSettings);
		RenderTools()->DrawUIRect(&MovementSettings, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		MovementSettings.Margin(10.0f, &MovementSettings);

		TextRender()->Text(0, MovementSettings.x, MovementSettings.y, 14.0f*UI()->Scale(), Localize("Movement"), -1);

		MovementSettings.HSplitTop(14.0f+5.0f+10.0f, 0, &MovementSettings);

		{
			CUIRect Button, Label;
			MovementSettings.HSplitTop(20.0f, &Button, &MovementSettings);
			Button.VSplitLeft(135.0f, &Label, &Button);
			UI()->DoLabel(&Label, Localize("Mouse sens."), 14.0f*UI()->Scale(), -1);
			Button.HMargin(2.0f, &Button);
			g_Config.m_InpMousesens = (int)(DoScrollbarH(&g_Config.m_InpMousesens, &Button, (g_Config.m_InpMousesens-5)/500.0f)*500.0f)+5;
			//*key.key = ui_do_key_reader(key.key, &Button, *key.key);
			MovementSettings.HSplitTop(20.0f, 0, &MovementSettings);
		}

		UiDoGetButtons(0, 5, MovementSettings);

	}

	// weapon settings
	{
		WeaponSettings.HSplitTop(10.0f, 0, &WeaponSettings);
		WeaponSettings.HSplitTop(MainView.h/3+50.0f, &WeaponSettings, &ResetButton);
		RenderTools()->DrawUIRect(&WeaponSettings, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		WeaponSettings.Margin(10.0f, &WeaponSettings);

		TextRender()->Text(0, WeaponSettings.x, WeaponSettings.y, 14.0f*UI()->Scale(), Localize("Weapon"), -1);

		WeaponSettings.HSplitTop(14.0f+5.0f+10.0f, 0, &WeaponSettings);
		UiDoGetButtons(5, 12, WeaponSettings);
	}
	
	// defaults
	{
		ResetButton.HSplitTop(10.0f, 0, &ResetButton);
		RenderTools()->DrawUIRect(&ResetButton, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		ResetButton.HMargin(10.0f, &ResetButton);
		ResetButton.VMargin(30.0f, &ResetButton);
		static int s_DefaultButton = 0;
		if(DoButton_Menu((void*)&s_DefaultButton, Localize("Reset to defaults"), 0, &ResetButton))
			m_pClient->m_pBinds->SetDefaults();
	}
	
	// voting settings
	{
		VotingSettings.VSplitLeft(10.0f, 0, &VotingSettings);
		VotingSettings.HSplitTop(MainView.h/3-75.0f, &VotingSettings, &ChatSettings);
		RenderTools()->DrawUIRect(&VotingSettings, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		VotingSettings.Margin(10.0f, &VotingSettings);

		TextRender()->Text(0, VotingSettings.x, VotingSettings.y, 14.0f*UI()->Scale(), Localize("Voting"), -1);

		VotingSettings.HSplitTop(14.0f+5.0f+10.0f, 0, &VotingSettings);
		UiDoGetButtons(12, 14, VotingSettings);
	}

	// chat settings
	{
		ChatSettings.HSplitTop(10.0f, 0, &ChatSettings);
		ChatSettings.HSplitTop(MainView.h/3-45.0f, &ChatSettings, &MiscSettings);
		RenderTools()->DrawUIRect(&ChatSettings, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		ChatSettings.Margin(10.0f, &ChatSettings);

		TextRender()->Text(0, ChatSettings.x, ChatSettings.y, 14.0f*UI()->Scale(), Localize("Chat"), -1);

		ChatSettings.HSplitTop(14.0f+5.0f+10.0f, 0, &ChatSettings);
		UiDoGetButtons(14, 17, ChatSettings);
	}

	// misc settings
	{
		MiscSettings.HSplitTop(10.0f, 0, &MiscSettings);
		RenderTools()->DrawUIRect(&MiscSettings, vec4(1,1,1,0.25f), CUI::CORNER_ALL, 10.0f);
		MiscSettings.Margin(10.0f, &MiscSettings);

		TextRender()->Text(0, MiscSettings.x, MiscSettings.y, 14.0f*UI()->Scale(), Localize("Miscellaneous"), -1);

		MiscSettings.HSplitTop(14.0f+5.0f+10.0f, 0, &MiscSettings);
		UiDoGetButtons(17, 25, MiscSettings);
	}

}

void CMenus::RenderSettingsGraphics(CUIRect MainView)
{
	CUIRect Button;
	char aBuf[128];
	bool CheckSettings = false;

	static const int MAX_RESOLUTIONS = 256;
	static CVideoMode s_aModes[MAX_RESOLUTIONS];
	static int s_NumNodes = Graphics()->GetVideoModes(s_aModes, MAX_RESOLUTIONS);
	static int s_GfxScreenWidth = g_Config.m_GfxScreenWidth;
	static int s_GfxScreenHeight = g_Config.m_GfxScreenHeight;
	static int s_GfxColorDepth = g_Config.m_GfxColorDepth;
	static int s_GfxFullscreen = g_Config.m_GfxFullscreen;
	static int s_GfxVsync = g_Config.m_GfxVsync;
	static int s_GfxFsaaSamples = g_Config.m_GfxFsaaSamples;
	static int s_GfxTextureQuality = g_Config.m_GfxTextureQuality;
	static int s_GfxTextureCompression = g_Config.m_GfxTextureCompression;

	CUIRect ModeList;
	MainView.VSplitLeft(300.0f, &MainView, &ModeList);

	// draw allmodes switch
	ModeList.HSplitTop(20, &Button, &ModeList);
	if(DoButton_CheckBox(&g_Config.m_GfxDisplayAllModes, Localize("Show only supported"), g_Config.m_GfxDisplayAllModes^1, &Button))
	{
		g_Config.m_GfxDisplayAllModes ^= 1;
		s_NumNodes = Graphics()->GetVideoModes(s_aModes, MAX_RESOLUTIONS);
	}

	// display mode list
	static float s_ScrollValue = 0;
	int OldSelected = -1;
	int G = gcd(s_GfxScreenWidth, s_GfxScreenHeight);
	str_format(aBuf, sizeof(aBuf), "%s: %dx%d %d bit (%d:%d)", Localize("Current"), s_GfxScreenWidth, s_GfxScreenHeight, s_GfxColorDepth, s_GfxScreenWidth/G, s_GfxScreenHeight/G);
	UiDoListboxStart(&s_NumNodes , &ModeList, 24.0f, Localize("Display Modes"), aBuf, s_NumNodes, 1, OldSelected, s_ScrollValue);

	for(int i = 0; i < s_NumNodes; ++i)
	{
		const int Depth = s_aModes[i].m_Red+s_aModes[i].m_Green+s_aModes[i].m_Blue > 16 ? 24 : 16;
		if(g_Config.m_GfxColorDepth == Depth &&
			g_Config.m_GfxScreenWidth == s_aModes[i].m_Width &&
			g_Config.m_GfxScreenHeight == s_aModes[i].m_Height)
		{
			OldSelected = i;
		}

		CListboxItem Item = UiDoListboxNextItem(&s_aModes[i], OldSelected == i);
		if(Item.m_Visible)
		{
			int G = gcd(s_aModes[i].m_Width, s_aModes[i].m_Height);
			str_format(aBuf, sizeof(aBuf), " %dx%d %d bit (%d:%d)", s_aModes[i].m_Width, s_aModes[i].m_Height, Depth, s_aModes[i].m_Width/G, s_aModes[i].m_Height/G);
			UI()->DoLabelScaled(&Item.m_Rect, aBuf, 16.0f, -1);
		}
	}

	const int NewSelected = UiDoListboxEnd(&s_ScrollValue, 0);
	if(OldSelected != NewSelected)
	{
		const int Depth = s_aModes[NewSelected].m_Red+s_aModes[NewSelected].m_Green+s_aModes[NewSelected].m_Blue > 16 ? 24 : 16;
		g_Config.m_GfxColorDepth = Depth;
		g_Config.m_GfxScreenWidth = s_aModes[NewSelected].m_Width;
		g_Config.m_GfxScreenHeight = s_aModes[NewSelected].m_Height;
		CheckSettings = true;
	}

	// switches
	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_GfxFullscreen, Localize("Fullscreen"), g_Config.m_GfxFullscreen, &Button))
	{
		g_Config.m_GfxFullscreen ^= 1;
		CheckSettings = true;
	}

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_GfxVsync, Localize("V-Sync"), g_Config.m_GfxVsync, &Button))
	{
		g_Config.m_GfxVsync ^= 1;
		CheckSettings = true;
	}

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox_Number(&g_Config.m_GfxFsaaSamples, Localize("FSAA samples"), g_Config.m_GfxFsaaSamples, &Button))
	{
		g_Config.m_GfxFsaaSamples = (g_Config.m_GfxFsaaSamples+1)%17;
		CheckSettings = true;
	}

	MainView.HSplitTop(40.0f, &Button, &MainView);
	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_GfxTextureQuality, Localize("Quality Textures"), g_Config.m_GfxTextureQuality, &Button))
	{
		g_Config.m_GfxTextureQuality ^= 1;
		CheckSettings = true;
	}

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_GfxTextureCompression, Localize("Texture Compression"), g_Config.m_GfxTextureCompression, &Button))
	{
		g_Config.m_GfxTextureCompression ^= 1;
		CheckSettings = true;
	}

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_GfxHighDetail, Localize("High Detail"), g_Config.m_GfxHighDetail, &Button))
		g_Config.m_GfxHighDetail ^= 1;

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_ClRenderFLow, Localize("Particle flow (experimental)"), g_Config.m_ClRenderFLow, &Button))
		g_Config.m_ClRenderFLow ^= 1;

	// check if the new settings require a restart
	if(CheckSettings)
	{
		if(s_GfxScreenWidth == g_Config.m_GfxScreenWidth &&
			s_GfxScreenHeight == g_Config.m_GfxScreenHeight &&
			s_GfxColorDepth == g_Config.m_GfxColorDepth &&
			s_GfxFullscreen == g_Config.m_GfxFullscreen &&
			s_GfxVsync == g_Config.m_GfxVsync &&
			s_GfxFsaaSamples == g_Config.m_GfxFsaaSamples &&
			s_GfxTextureQuality == g_Config.m_GfxTextureQuality &&
			s_GfxTextureCompression == g_Config.m_GfxTextureCompression)
			m_NeedRestartGraphics = false;
		else
			m_NeedRestartGraphics = true;
	}

	//

	CUIRect Text;
	MainView.HSplitTop(20.0f, 0, &MainView);
	MainView.HSplitTop(20.0f, &Text, &MainView);
	//text.VSplitLeft(15.0f, 0, &text);
	UI()->DoLabelScaled(&Text, Localize("UI Color"), 14.0f, -1);

	const char *paLabels[] = {
		Localize("Hue"),
		Localize("Sat."),
		Localize("Lht."),
		Localize("Alpha")};
	int *pColorSlider[4] = {&g_Config.m_UiColorHue, &g_Config.m_UiColorSat, &g_Config.m_UiColorLht, &g_Config.m_UiColorAlpha};
	for(int s = 0; s < 4; s++)
	{
		CUIRect Text;
		MainView.HSplitTop(19.0f, &Button, &MainView);
		Button.VMargin(15.0f, &Button);
		Button.VSplitLeft(100.0f, &Text, &Button);
		//Button.VSplitRight(5.0f, &Button, 0);
		Button.HSplitTop(4.0f, 0, &Button);

		float k = (*pColorSlider[s]) / 255.0f;
		k = DoScrollbarH(pColorSlider[s], &Button, k);
		*pColorSlider[s] = (int)(k*255.0f);
		UI()->DoLabelScaled(&Text, paLabels[s], 15.0f, -1);
	}
}

void CMenus::RenderSettingsSound(CUIRect MainView)
{
	CUIRect Button;
	MainView.VSplitMid(&MainView, 0);
	static int s_SndEnable = g_Config.m_SndEnable;
	static int s_SndRate = g_Config.m_SndRate;

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_SndEnable, Localize("Use sounds"), g_Config.m_SndEnable, &Button))
	{
		g_Config.m_SndEnable ^= 1;
		if(g_Config.m_SndEnable)
		{
			if(g_Config.m_SndMusic)
				m_pClient->m_pSounds->Play(CSounds::CHN_MUSIC, SOUND_MENU, 1.0f, vec2(0, 0));
		}
		else
			m_pClient->m_pSounds->Stop(SOUND_MENU);
		m_NeedRestartSound = g_Config.m_SndEnable && (!s_SndEnable || s_SndRate != g_Config.m_SndRate);
	}

	if(!g_Config.m_SndEnable)
		return;

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_SndMusic, Localize("Play background music"), g_Config.m_SndMusic, &Button))
	{
		g_Config.m_SndMusic ^= 1;
		if(g_Config.m_SndMusic)
			m_pClient->m_pSounds->Play(CSounds::CHN_MUSIC, SOUND_MENU, 1.0f, vec2(0, 0));
		else
			m_pClient->m_pSounds->Stop(SOUND_MENU);
	}

	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_SndNonactiveMute, Localize("Mute when not active"), g_Config.m_SndNonactiveMute, &Button))
		g_Config.m_SndNonactiveMute ^= 1;

	// sample rate box
	{
		char aBuf[64];
		str_format(aBuf, sizeof(aBuf), "%d", g_Config.m_SndRate);
		MainView.HSplitTop(20.0f, &Button, &MainView);
		UI()->DoLabelScaled(&Button, Localize("Sample rate"), 14.0f, -1);
		Button.VSplitLeft(190.0f, 0, &Button);
		static float Offset = 0.0f;
		DoEditBox(&g_Config.m_SndRate, &Button, aBuf, sizeof(aBuf), 14.0f, &Offset);
		g_Config.m_SndRate = max(1, str_toint(aBuf));
		m_NeedRestartSound = !s_SndEnable || s_SndRate != g_Config.m_SndRate;
	}

	// volume slider
	{
		CUIRect Button, Label;
		MainView.HSplitTop(5.0f, &Button, &MainView);
		MainView.HSplitTop(20.0f, &Button, &MainView);
		Button.VSplitLeft(190.0f, &Label, &Button);
		Button.HMargin(2.0f, &Button);
		UI()->DoLabelScaled(&Label, Localize("Sound volume"), 14.0f, -1);
		g_Config.m_SndVolume = (int)(DoScrollbarH(&g_Config.m_SndVolume, &Button, g_Config.m_SndVolume/100.0f)*100.0f);
		MainView.HSplitTop(20.0f, 0, &MainView);
	}
}

class CLanguage
{
public:
	CLanguage() {}
	CLanguage(const char *n, const char *f) : m_Name(n), m_FileName(f) {}

	string m_Name;
	string m_FileName;

	bool operator<(const CLanguage &Other) { return m_Name < Other.m_Name; }
};

void LoadLanguageIndexfile(IStorage *pStorage, IConsole *pConsole, sorted_array<CLanguage> *pLanguages)
{
	IOHANDLE File = pStorage->OpenFile("languages/index.txt", IOFLAG_READ, IStorage::TYPE_ALL);
	if(!File)
	{
		pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "localization", "couldn't open index file");
		return;
	}

	char aOrigin[128];
	CLineReader LineReader;
	LineReader.Init(File);
	char *pLine;
	while((pLine = LineReader.Get()))
	{
		if(!str_length(pLine) || pLine[0] == '#') // skip empty lines and comments
			continue;

		str_copy(aOrigin, pLine, sizeof(aOrigin));
		char *pReplacement = LineReader.Get();
		if(!pReplacement)
		{
			pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "localization", "unexpected end of index file");
			break;
		}

		if(pReplacement[0] != '=' || pReplacement[1] != '=' || pReplacement[2] != ' ')
		{
			char aBuf[128];
			str_format(aBuf, sizeof(aBuf), "malform replacement for index '%s'", aOrigin);
			pConsole->Print(IConsole::OUTPUT_LEVEL_ADDINFO, "localization", aBuf);
			continue;
		}

		char aFileName[128];
		str_format(aFileName, sizeof(aFileName), "languages/%s.txt", aOrigin);
		pLanguages->add(CLanguage(pReplacement+3, aFileName));
	}
	io_close(File);
}

void CMenus::RenderLanguageSelection(CUIRect MainView)
{
	static int s_LanguageList = 0;
	static int s_SelectedLanguage = 0;
	static sorted_array<CLanguage> s_Languages;
	static float s_ScrollValue = 0;

	if(s_Languages.size() == 0)
	{
		s_Languages.add(CLanguage("English", ""));
		LoadLanguageIndexfile(Storage(), Console(), &s_Languages);
		for(int i = 0; i < s_Languages.size(); i++)
			if(str_comp(s_Languages[i].m_FileName, g_Config.m_ClLanguagefile) == 0)
			{
				s_SelectedLanguage = i;
				break;
			}
	}

	int OldSelected = s_SelectedLanguage;

	UiDoListboxStart(&s_LanguageList , &MainView, 24.0f, Localize("Language"), "", s_Languages.size(), 1, s_SelectedLanguage, s_ScrollValue);

	for(sorted_array<CLanguage>::range r = s_Languages.all(); !r.empty(); r.pop_front())
	{
		CListboxItem Item = UiDoListboxNextItem(&r.front());

		if(Item.m_Visible)
			UI()->DoLabelScaled(&Item.m_Rect, r.front().m_Name, 16.0f, -1);
	}

	s_SelectedLanguage = UiDoListboxEnd(&s_ScrollValue, 0);

	if(OldSelected != s_SelectedLanguage)
	{
		str_copy(g_Config.m_ClLanguagefile, s_Languages[s_SelectedLanguage].m_FileName, sizeof(g_Config.m_ClLanguagefile));
		g_Localization.Load(s_Languages[s_SelectedLanguage].m_FileName, Storage(), Console());
	}
}

class CFontFile
{
public:
	CFontFile() {}
	CFontFile(const char *n, const char *f) : m_Name(n), m_FileName(f) {}

	string m_Name;
	string m_FileName;

	bool operator<(const CFontFile &Other) { return m_Name < Other.m_Name; }
};

 int GatherFonts(const char *pFileName, int IsDir, int Type, void *pUser)
{
	const int PathLength = str_length(pFileName);
	if(IsDir || PathLength <= 4 || pFileName[PathLength-4] != '.' || str_comp_nocase(pFileName+PathLength-3, "ttf") || pFileName[0] == '.')
		return 0;

	sorted_array<CFontFile> &Fonts = *((sorted_array<CFontFile> *)pUser);
	
	char aNiceName[128];
	str_copy(aNiceName, pFileName, PathLength-3);
	aNiceName[0] = str_uppercase(aNiceName[0]);

	// check if the font was already added
	for(int i = 0; i < Fonts.size(); i++)
		if(!str_comp(Fonts[i].m_Name, aNiceName))
			return 0;
	
	Fonts.add(CFontFile(aNiceName, pFileName));
	
	return 0;
}

void CMenus::RenderFontSelection(CUIRect MainView)
{
	static int s_FontList  = 0;
	static int s_SelectedFont = 0;
	static sorted_array<CFontFile> s_Fonts;
	static float s_ScrollValue = 0;

	if(s_Fonts.size() == 0)
	{
		Storage()->ListDirectory(IStorage::TYPE_ALL, "fonts", GatherFonts, &s_Fonts);
		for(int i = 0; i < s_Fonts.size(); i++)
			if(str_comp(s_Fonts[i].m_FileName, g_Config.m_ClFontfile) == 0)
			{
				s_SelectedFont = i;
				break;
			}
	}

	int OldSelectedFont = s_SelectedFont;

	UiDoListboxStart(&s_FontList , &MainView, 24.0f, Localize("Fonts"), "", s_Fonts.size(), 1, s_SelectedFont, s_ScrollValue);

	for(sorted_array<CFontFile>::range r = s_Fonts.all(); !r.empty(); r.pop_front())
	{
		CListboxItem Item = UiDoListboxNextItem(&r.front());

		if(Item.m_Visible)
			UI()->DoLabel(&Item.m_Rect, r.front().m_Name, 16.0f, -1);
	}

	s_SelectedFont = UiDoListboxEnd(&s_ScrollValue, 0);

	if(OldSelectedFont != s_SelectedFont)
	{
		str_copy(g_Config.m_ClFontfile, s_Fonts[s_SelectedFont].m_FileName, sizeof(g_Config.m_ClFontfile));
		char aRelFontPath[512];
		str_format(aRelFontPath, sizeof(aRelFontPath), "fonts/%s", g_Config.m_ClFontfile);
		char aFontPath[512];	
		IOHANDLE File = Storage()->OpenFile(aRelFontPath, IOFLAG_READ, IStorage::TYPE_ALL, aFontPath, sizeof(aFontPath));
		if(File)
			io_close(File);
		TextRender()->SetFont(TextRender()->LoadFont(aFontPath));
	}
}

void CMenus::RenderSettingsChat(CUIRect MainView)
{
	CUIRect Button;

	MainView.VSplitLeft(300.0f, &MainView, 0);
		
	MainView.HSplitTop(20.0f, &Button, &MainView);
	if(DoButton_CheckBox(&g_Config.m_ClAntiSpam, Localize("Block spam"), g_Config.m_ClAntiSpam, &Button))
		g_Config.m_ClAntiSpam ^= 1;
	
	MainView.HSplitTop(10.0f, &Button, &MainView);
	
	MainView.HSplitTop(20.0f, &Button, &MainView);
	char aBuf[64];
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Additional highlighting"));
	UI()->DoLabel(&Button, aBuf, 14.0f, -1);
	float tw = TextRender()->TextWidth(0, 14.0f, aBuf, -1);
	Button.VSplitLeft(tw+10.0f, 0, &Button);
	Button.VSplitLeft(380.0f, &Button, 0);
	static float SearchOffset = 0.0f;
	DoEditBox(g_Config.m_ClSearchName, &Button, g_Config.m_ClSearchName, sizeof(g_Config.m_ClSearchName), 14.0f, &SearchOffset);
	
	// information text
	MainView.HSplitBottom(25.0f, 0, &Button);
	UI()->DoLabel(&Button, Localize("Enter the names you want to look for."), 14.0f, -1);
	MainView.HSplitBottom(12.0f, 0, &Button);
	UI()->DoLabel(&Button, Localize("Seperate them with a simple space."), 14.0f, -1);
}

void CMenus::RenderSettingsHudMod(CUIRect MainView)
{
	CUIRect Button;
	CUIRect LeftView, RightView;

	MainView.VSplitMid(&LeftView, &RightView);
	
	// Left
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	UI()->DoLabel(&Button, Localize("General settings"), 14.0f, -1);
	
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClShowhud, Localize("Show hud"), g_Config.m_ClShowhud, &Button))
		g_Config.m_ClShowhud ^= 1;
		
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClClearAll, Localize("Clear all"), g_Config.m_ClClearAll, &Button))
		g_Config.m_ClClearAll ^= 1;
	
	LeftView.HSplitTop(40.0f, &Button, &LeftView);
		
	if(!g_Config.m_ClClearAll)
	{
		LeftView.HSplitTop(15.0f, &Button, &LeftView);
		UI()->DoLabel(&Button, Localize("Special settings"), 14.0f, -1);
		
		LeftView.HSplitTop(10.0f, &Button, &LeftView);
		
		if(g_Config.m_ClShowhud)
		{
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderTime, Localize("Server time"), g_Config.m_ClRenderTime, &Button))
				g_Config.m_ClRenderTime ^= 1;
				
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderHp, Localize("Health"), g_Config.m_ClRenderHp, &Button))
				g_Config.m_ClRenderHp ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderAmmo, Localize("Ammunition"), g_Config.m_ClRenderAmmo, &Button))
				g_Config.m_ClRenderAmmo ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderCrosshair, Localize("Crosshair"), g_Config.m_ClRenderCrosshair, &Button))
				g_Config.m_ClRenderCrosshair ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderTeamScore, Localize("Team score"), g_Config.m_ClRenderTeamScore, &Button))
				g_Config.m_ClRenderTeamScore ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderDmScore, Localize("DM score"), g_Config.m_ClRenderDmScore, &Button))
				g_Config.m_ClRenderDmScore ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClShowfps, Localize("FPS"), g_Config.m_ClShowfps, &Button))
				g_Config.m_ClShowfps ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderViewmode, Localize("Viewmode"), g_Config.m_ClRenderViewmode, &Button))
				g_Config.m_ClRenderViewmode ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderWarmup, Localize("Warmup"), g_Config.m_ClRenderWarmup, &Button))
				g_Config.m_ClRenderWarmup ^= 1;
			
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			if(DoButton_CheckBox(&g_Config.m_ClRenderVote, Localize("Votes"), g_Config.m_ClRenderVote, &Button))
				g_Config.m_ClRenderVote ^= 1;
		}
		
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderScoreboard, Localize("Scoreboard"), g_Config.m_ClRenderScoreboard, &Button))
			g_Config.m_ClRenderScoreboard ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderBroadcast, Localize("Broadcast"), g_Config.m_ClRenderBroadcast, &Button))
			g_Config.m_ClRenderBroadcast ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderServermsg, Localize("Server messages"), g_Config.m_ClRenderServermsg, &Button))
			g_Config.m_ClRenderServermsg ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderChat, Localize("Chat"), g_Config.m_ClRenderChat, &Button))
			g_Config.m_ClRenderChat ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderKill, Localize("Kill messages"), g_Config.m_ClRenderKill, &Button))
			g_Config.m_ClRenderKill ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClRenderEmotes, Localize("Emoticons"), g_Config.m_ClRenderEmotes, &Button))
			g_Config.m_ClRenderEmotes ^= 1;
			
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		if(DoButton_CheckBox(&g_Config.m_ClWarningTeambalance, Localize("Team balance warning"), g_Config.m_ClWarningTeambalance, &Button))
			g_Config.m_ClWarningTeambalance ^= 1;
	}
	
	// Right
	RightView.HSplitTop(20.0f, &Button, &RightView);
	UI()->DoLabel(&Button, Localize("Sound settings"), 14.0f, -1);
	
	RightView.HSplitTop(20.0f, &Button, &RightView);
	if(DoButton_CheckBox(&g_Config.m_ClServermsgsound, Localize("Activate server message sound"), g_Config.m_ClServermsgsound, &Button))
		g_Config.m_ClServermsgsound ^= 1;
		
	RightView.HSplitTop(20.0f, &Button, &RightView);
	if(DoButton_CheckBox(&g_Config.m_ClChatsound, Localize("Activate chat message sound"), g_Config.m_ClChatsound, &Button))
		g_Config.m_ClChatsound ^= 1;
		
	RightView.HSplitTop(20.0f, &Button, &RightView);
	if(DoButton_CheckBox(&g_Config.m_ClSpreesounds, Localize("Activate spree sounds"), g_Config.m_ClSpreesounds, &Button))
		g_Config.m_ClSpreesounds ^= 1;
	
	RightView.HSplitTop(20.0f, 0, &RightView);
	RightView.HSplitTop(20.0f, &Button, &RightView);
	UI()->DoLabel(&Button, Localize("Key bindings"), 14.0f, -1);
	RightView.HSplitTop(5.0f, &Button, &RightView);
	char aaBuf[2][32];
	str_format(aaBuf[0], sizeof(aaBuf[0]), "%s:", Localize("Zoom out"));
	str_format(aaBuf[1], sizeof(aaBuf[1]), "%s:", Localize("Zoom in"));
	CKeyInfo pKeys[] = {{ aaBuf[0], "+zoomout", 0},
		{ aaBuf[1], "+zoomin", 0}};

	for(int pKeyid=0; pKeyid < KEY_LAST; pKeyid++)
	{
		const char *Bind = m_pClient->m_pBinds->Get(pKeyid);
		if(!Bind[0])
			continue;

		for(unsigned int i=0; i<sizeof(pKeys)/sizeof(CKeyInfo); i++)
			if(str_comp(Bind, pKeys[i].m_pCommand) == 0)
			{
				pKeys[i].m_KeyId = pKeyid;
				break;
			}
	}

	for(unsigned int i=0; i<sizeof(pKeys)/sizeof(CKeyInfo); i++)
		UiDoKeybinder(pKeys[i], &RightView);
		
	// default button
	LeftView.HSplitBottom(20.0f, 0, &Button);
	static int s_DefaultButton = 0;
	if(DoButton_Menu((void*)&s_DefaultButton, Localize("Reset to defaults"), 0, &Button))
	{
		g_Config.m_ClRenderTime = 1;
		g_Config.m_ClRenderWarmup = 1;
		g_Config.m_ClRenderBroadcast = 1;
		g_Config.m_ClRenderHp = 1;
		g_Config.m_ClRenderAmmo = 1;
		g_Config.m_ClRenderCrosshair = 1;
		g_Config.m_ClRenderTeamScore = 1;
		g_Config.m_ClRenderDmScore = 1;
		g_Config.m_ClShowfps = 0;
		g_Config.m_ClRenderScoreboard = 1;
		g_Config.m_ClRenderServermsg = 1;
		g_Config.m_ClRenderChat = 1;
		g_Config.m_ClRenderKill = 1;
		g_Config.m_ClRenderVote = 1;
		g_Config.m_ClRenderEmotes = 1;
		g_Config.m_ClClearAll = 0;
		g_Config.m_ClShowhud = 1;
		g_Config.m_ClServermsgsound = 1;
		g_Config.m_ClChatsound = 1;
		g_Config.m_ClWarningTeambalance = 1;
	}
}

void CMenus::RenderSettingsRace(CUIRect MainView)
{
	CUIRect Button, ApiButton;
	CUIRect LeftView, RightView;

	MainView.VSplitMid(&LeftView, &RightView);
	
	// Left
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	UI()->DoLabel(&Button, Localize("Race specific settings"), 14.0f, -1);
	
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClAutoRaceRecord, Localize("Auto record"), g_Config.m_ClAutoRaceRecord, &Button))
		g_Config.m_ClAutoRaceRecord ^= 1;
	
	if(g_Config.m_ClAutoRaceRecord)
	{
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		Button.VSplitLeft(15.0f, 0, &Button);
		if(DoButton_CheckBox(&g_Config.m_ClDemoName, Localize("Save player name"), g_Config.m_ClDemoName, &Button))
			g_Config.m_ClDemoName ^= 1;
	}
		
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClShowOthers, Localize("Show other players"), g_Config.m_ClShowOthers, &Button))
		g_Config.m_ClShowOthers ^= 1;
		
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClShowCheckpointDiff, Localize("Show checkpoint difference"), g_Config.m_ClShowCheckpointDiff, &Button))
		g_Config.m_ClShowCheckpointDiff ^= 1;
	
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClRaceGhost, Localize("Enable Ghost"), g_Config.m_ClRaceGhost, &Button))
		g_Config.m_ClRaceGhost ^= 1;
		
	if(g_Config.m_ClRaceGhost)
	{
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		Button.VSplitLeft(15.0f, 0, &Button);
		if(DoButton_CheckBox(&g_Config.m_ClRaceSaveGhost, Localize("Save Ghost"), g_Config.m_ClRaceSaveGhost, &Button))
			g_Config.m_ClRaceSaveGhost ^= 1;
		
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		Button.VSplitLeft(15.0f, 0, &Button);
		if(DoButton_CheckBox(&g_Config.m_ClGhostNamePlates, Localize("Show ghost name plates"), g_Config.m_ClGhostNamePlates, &Button))
			g_Config.m_ClGhostNamePlates ^= 1;
		
		if(g_Config.m_ClGhostNamePlates)
		{
			LeftView.HSplitTop(20.0f, &Button, &LeftView);
			Button.VSplitLeft(30.0f, 0, &Button);
			if(DoButton_CheckBox(&g_Config.m_ClGhostNameplatesAlways, Localize("Always show ghost name plates"), g_Config.m_ClGhostNameplatesAlways, &Button))
				g_Config.m_ClGhostNameplatesAlways ^= 1;
		}
	}
		
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	if(DoButton_CheckBox(&g_Config.m_ClShowRecords, Localize("Show records"), g_Config.m_ClShowRecords, &Button))
		g_Config.m_ClShowRecords ^= 1;
		
	if(g_Config.m_ClShowRecords)
	{
		LeftView.HSplitTop(20.0f, &Button, &LeftView);
		Button.VSplitLeft(15.0f, 0, &Button);
		if(DoButton_CheckBox(&g_Config.m_ClShowServerRecord, Localize("Show best time on server"), g_Config.m_ClShowServerRecord, &Button))
			g_Config.m_ClShowServerRecord ^= 1;
	}
	else
		LeftView.HSplitTop(40.0f, &Button, &LeftView);
	
	// username
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	char aBuf[32];
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Username"));
	UI()->DoLabel(&Button, aBuf, 14.0, -1);
	Button.VSplitLeft(80.0f, 0, &Button);
	Button.VSplitLeft(180.0f, &Button, 0);
	static float UserOffset = 0.0f;
	DoEditBox(g_Config.m_ClUsername, &Button, g_Config.m_ClUsername, sizeof(g_Config.m_ClUsername), 14.0f, &UserOffset);

	LeftView.HSplitTop(5.0f, &Button, &LeftView);

	// password
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Password"));
	UI()->DoLabel(&Button, aBuf, 14.0, -1);
	Button.VSplitLeft(80.0f, 0, &Button);
	Button.VSplitLeft(180.0f, &Button, 0);
	static float PassOffset = 0.0f;
	DoEditBox(g_Config.m_ClPassword, &Button, g_Config.m_ClPassword, sizeof(g_Config.m_ClPassword), 14.0f, &PassOffset, true);

	// api token box
	LeftView.HSplitTop(20.0f, &Button, &LeftView);
	str_format(aBuf, sizeof(aBuf), "%s:", Localize("Api token"));
	UI()->DoLabel(&Button, aBuf, 14.0, -1);
	Button.VSplitLeft(80.0f, 0, &Button);
	UI()->DoLabel(&Button, m_pClient->Webapp()->m_ApiTokenError ? Localize("Error requesting api token") : (g_Config.m_ClApiToken[0] == 0) ? Localize("None") : g_Config.m_ClApiToken, 14.0, -1);

	LeftView.HSplitTop(20.0f, &ApiButton, &LeftView);
	ApiButton.VSplitLeft(200.0f, &ApiButton, 0);
	static int s_ApiTokenButton = 0;
	if(DoButton_Menu((void*)&s_ApiTokenButton, m_pClient->Webapp()->m_ApiTokenRequested ? Localize("Checking...") : Localize("Request api token"), m_pClient->Webapp()->m_ApiTokenRequested ? -1 : 0, &ApiButton))
	{
		m_pClient->Webapp()->m_ApiTokenRequested = true;
		CWebApiToken::CParam *pParams = new CWebApiToken::CParam();
		str_copy(pParams->m_aUsername, g_Config.m_ClUsername, sizeof(pParams->m_aUsername));
		str_copy(pParams->m_aPassword, g_Config.m_ClPassword, sizeof(pParams->m_aPassword));
		m_pClient->Webapp()->AddJob(CWebApiToken::GetApiToken, pParams, 0);
	}
}

void CMenus::RenderSettings(CUIRect MainView)
{
	static int s_SettingsPage = 0;

	// render background
	CUIRect Temp, TabBar, RestartWarning;
	MainView.HSplitBottom(15.0f, &MainView, &RestartWarning);
	MainView.VSplitRight(120.0f, &MainView, &TabBar);
	RenderTools()->DrawUIRect(&MainView, ms_ColorTabbarActive, CUI::CORNER_B|CUI::CORNER_TL, 10.0f);
	TabBar.HSplitTop(50.0f, &Temp, &TabBar);
	RenderTools()->DrawUIRect(&Temp, ms_ColorTabbarActive, CUI::CORNER_R, 10.0f);

	MainView.HSplitTop(10.0f, 0, &MainView);

	CUIRect Button;

	const char *aTabs[] = {
		Localize("Language"),
		Localize("General"),
		Localize("Player"),
		("Tee"),
		Localize("Controls"),
		Localize("Graphics"),
		Localize("Sound"),
		Localize("Chat"),
		Localize("Hud"),
		Localize("Race"),
		"TeeComp"};

	int NumTabs = (int)(sizeof(aTabs)/sizeof(*aTabs));

	for(int i = 0; i < NumTabs; i++)
	{
		TabBar.HSplitTop(10, &Button, &TabBar);
		TabBar.HSplitTop(26, &Button, &TabBar);
		if(DoButton_MenuTab(aTabs[i], aTabs[i], s_SettingsPage == i, &Button, CUI::CORNER_R))
			s_SettingsPage = i;
	}

	MainView.Margin(10.0f, &MainView);

	if(s_SettingsPage == 0)
		RenderLanguageSelection(MainView);
	else if(s_SettingsPage == 1)
		RenderSettingsGeneral(MainView);
	else if(s_SettingsPage == 2)
		RenderSettingsPlayer(MainView);
	else if(s_SettingsPage == 3)
		RenderSettingsTee(MainView);
	else if(s_SettingsPage == 4)
		RenderSettingsControls(MainView);
	else if(s_SettingsPage == 5)
		RenderSettingsGraphics(MainView);
	else if(s_SettingsPage == 6)
		RenderSettingsSound(MainView);
	else if(s_SettingsPage == 7)
		RenderSettingsChat(MainView);
	else if(s_SettingsPage == 8)
		RenderSettingsHudMod(MainView);
	else if(s_SettingsPage == 9)
		RenderSettingsRace(MainView);
	else if(s_SettingsPage == 10)
		RenderSettingsTeecomp(MainView);

	if(m_NeedRestartGraphics || m_NeedRestartSound)
		UI()->DoLabel(&RestartWarning, Localize("You must restart the game for all settings to take effect."), 15.0f, -1);
}

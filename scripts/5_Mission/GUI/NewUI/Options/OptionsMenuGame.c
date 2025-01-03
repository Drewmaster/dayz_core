class OptionsMenuGame extends ScriptedWidgetEventHandler
{
	private const float HUD_BRIGHTNESS_MIN = -1.0;
	private const float HUD_BRIGHTNESS_MAX = 0.0;

	protected Widget						m_Root;
	
	protected Widget						m_SettingsRoot;
	protected Widget						m_DetailsRoot;
	protected Widget						m_DetailsBodyDefault;
	protected Widget						m_DetailsBodyConnectivity;
	protected TextWidget					m_DetailsLabel;
	protected RichTextWidget				m_DetailsText;
	protected ButtonWidget					m_QuickbarButton;
	
	protected ref NumericOptionsAccess		m_FOVOption;
	protected ref ListOptionsAccess			m_LanguageOption;
	protected ref ListOptionsAccess			m_PauseOption;
	#ifdef PLATFORM_CONSOLE
	protected ref NumericOptionsAccess		m_BrightnessOption;
	protected ref OptionSelectorSlider		m_BrightnessSelector;
	#endif
	
	protected ref OptionSelectorMultistate	m_LanguageSelector;
	protected ref OptionSelectorSlider		m_FOVSelector;
	protected ref OptionSelectorMultistate	m_ShowHUDSelector;
	protected ref OptionSelectorMultistate	m_ShowCrosshairSelector;
	protected ref OptionSelectorMultistate	m_ShowQuickbarSelector;
	protected ref OptionSelectorMultistate	m_ShowHUDVehicleSelector;
	protected ref OptionSelectorMultistate	m_ShowGameSelector;
	protected ref OptionSelectorMultistate	m_ShowAdminSelector;
	protected ref OptionSelectorMultistate	m_ShowPlayerSelector;
	protected ref OptionSelectorMultistate 	m_ShowServerInfoSelector;
	protected ref OptionSelectorMultistate 	m_BleedingIndication;
	protected ref OptionSelectorMultistate 	m_ConnectivityInfo;
	protected ref OptionSelectorSlider		m_HUDBrightnessSelector;
	protected ref OptionSelectorMultistate	m_PauseSelector;
	
	protected GameOptions					m_Options;
	protected OptionsMenu					m_Menu;
	
	protected ref map<int, ref Param2<string, string>> m_TextMap;
	
	void OptionsMenuGame(Widget parent, Widget details_root, GameOptions options, OptionsMenu menu)
	{
		m_Root						= GetGame().GetWorkspace().CreateWidgets(GetLayoutName(), parent);
		
		m_DetailsRoot				= details_root;
		m_DetailsBodyDefault 		= m_DetailsRoot.FindAnyWidget("settings_details_body");
		m_DetailsBodyConnectivity 	= m_DetailsRoot.FindAnyWidget("settings_details_body_connectivity");
		m_DetailsLabel				= TextWidget.Cast(m_DetailsRoot.FindAnyWidget("details_label"));
		m_DetailsText				= RichTextWidget.Cast(m_DetailsRoot.FindAnyWidget("details_content"));
		
		m_Options					= options;
		m_Menu						= menu;
		
		m_FOVOption					= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW));
		m_LanguageOption			= ListOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
		m_PauseOption				= ListOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_PAUSE));
		
		m_Root.FindAnyWidget("fov_setting_option").SetUserID(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW);
		m_Root.FindAnyWidget("hud_setting_option").SetUserID(OptionIDsScript.OPTION_HUD);
		m_Root.FindAnyWidget("hud_vehicle_setting_option").SetUserID(OptionIDsScript.OPTION_HUD_VEHICLE);
		m_Root.FindAnyWidget("hud_brightness_setting_option").SetUserID(OptionIDsScript.OPTION_HUD_BRIGHTNESS);
		m_Root.FindAnyWidget("crosshair_setting_option").SetUserID(OptionIDsScript.OPTION_CROSSHAIR);
		m_Root.FindAnyWidget("game_setting_option").SetUserID(OptionIDsScript.OPTION_GAME_MESSAGES);
		m_Root.FindAnyWidget("admin_setting_option").SetUserID(OptionIDsScript.OPTION_ADMIN_MESSAGES);
		m_Root.FindAnyWidget("player_setting_option").SetUserID(OptionIDsScript.OPTION_PLAYER_MESSAGES);
		m_Root.FindAnyWidget("language_setting_option").SetUserID(OptionAccessType.AT_OPTIONS_LANGUAGE);
		m_Root.FindAnyWidget("bleeding_indication_setting_option").SetUserID(OptionIDsScript.OPTION_BLEEDINGINDICATION);
		m_Root.FindAnyWidget("connectivity_setting_option").SetUserID(OptionIDsScript.OPTION_CONNECTIVITY_INFO);
		m_Root.FindAnyWidget("quickbar_setting_option").SetUserID(OptionIDsScript.OPTION_QUICKBAR);
		
		#ifdef PLATFORM_CONSOLE
		m_QuickbarButton = ButtonWidget.Cast(m_Root.FindAnyWidget("quickbar_button"));
		m_Root.FindAnyWidget("brightness_setting_option").SetUserID(OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER);
		#else
		m_Root.FindAnyWidget("serverinfo_setting_option").SetUserID(OptionIDsScript.OPTION_SERVER_INFO);
		m_Root.FindAnyWidget("pause_setting_option").SetUserID(OptionAccessType.AT_OPTIONS_PAUSE);
		#endif
		
		FillTextMap();
		
		array<string> opt		= { "#options_controls_disabled", "#options_controls_enabled" };
		array<string> opt2		= { "#options_controls_enabled", "#options_controls_disabled" };
		array<string> opt3		= new array<string>;
		for (int i = 0; i < m_LanguageOption.GetItemsCount(); ++i)
		{
			string text;
			m_LanguageOption.GetItemText(i, text);
			opt3.Insert(text);
		}
		
		array<string> opt4		= { "#options_pc_nopause_0", "#options_pc_nopause_1", "#options_pc_nopause_2" }; // Pause options menu strings
		
		m_LanguageSelector			= new OptionSelectorMultistate(m_Root.FindAnyWidget("language_setting_option"), m_LanguageOption.GetIndex(), this, false, opt3);
		m_FOVSelector				= new OptionSelectorSlider(m_Root.FindAnyWidget("fov_setting_option"), m_FOVOption.ReadValue(), this, false, m_FOVOption.GetMin(), m_FOVOption.GetMax());
		m_ShowHUDSelector			= new OptionSelectorMultistate(m_Root.FindAnyWidget("hud_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD), this, false, opt);
		m_HUDBrightnessSelector		= new OptionSelectorSlider(m_Root.FindAnyWidget("hud_brightness_setting_option"), g_Game.GetProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS), this, false, HUD_BRIGHTNESS_MIN, HUD_BRIGHTNESS_MAX);
		m_ShowCrosshairSelector		= new OptionSelectorMultistate(m_Root.FindAnyWidget("crosshair_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.CROSSHAIR), this, false, opt);
		m_ShowGameSelector			= new OptionSelectorMultistate(m_Root.FindAnyWidget("game_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.GAME_MESSAGES), this, false, opt2);
		m_ShowAdminSelector			= new OptionSelectorMultistate(m_Root.FindAnyWidget("admin_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.ADMIN_MESSAGES), this, false, opt2);
		m_ShowPlayerSelector		= new OptionSelectorMultistate(m_Root.FindAnyWidget("player_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.PLAYER_MESSAGES), this, false, opt2);
		m_BleedingIndication		= new OptionSelectorMultistate(m_Root.FindAnyWidget("bleeding_indication_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION), this, false, opt);
		m_ConnectivityInfo			= new OptionSelectorMultistate(m_Root.FindAnyWidget("connectivity_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO), this, false, opt);
		m_ShowQuickbarSelector		= new OptionSelectorMultistate(m_Root.FindAnyWidget("quickbar_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.QUICKBAR), this, false, opt);
		m_ShowHUDVehicleSelector	= new OptionSelectorMultistate(m_Root.FindAnyWidget("hud_vehicle_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD_VEHICLE), this, false, opt);
		
		m_LanguageSelector.m_OptionChanged.Insert(UpdateLanguageOption);
		m_FOVSelector.m_OptionChanged.Insert(UpdateFOVOption);
		m_ShowHUDSelector.m_OptionChanged.Insert(UpdateHUDOption);
		m_HUDBrightnessSelector.m_OptionChanged.Insert(UpdateHUDBrightnessOption);
		m_ShowCrosshairSelector.m_OptionChanged.Insert(UpdateCrosshairOption);
		m_ShowGameSelector.m_OptionChanged.Insert(UpdateGameOption);
		m_ShowAdminSelector.m_OptionChanged.Insert(UpdateAdminOption);
		m_ShowPlayerSelector.m_OptionChanged.Insert(UpdatePlayerOption);
		m_BleedingIndication.m_OptionChanged.Insert(UpdateBleedingIndication);
		m_ConnectivityInfo.m_OptionChanged.Insert(UpdateConnectivityInfo);
		m_ShowQuickbarSelector.m_OptionChanged.Insert(UpdateQuickbarOption);
		m_ShowHUDVehicleSelector.m_OptionChanged.Insert(UpdateHUDVehicleOption);
		
		#ifdef PLATFORM_CONSOLE
		m_BrightnessOption		= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER));
		m_BrightnessSelector	= new OptionSelectorSlider(m_Root.FindAnyWidget("brightness_setting_option"), m_BrightnessOption.ReadValue(), this, false, m_BrightnessOption.GetMin(), m_BrightnessOption.GetMax());
		m_BrightnessSelector.m_OptionChanged.Insert(UpdateBrightnessOption);
		#else
		m_ShowServerInfoSelector	= new OptionSelectorMultistate(m_Root.FindAnyWidget("serverinfo_setting_option"), g_Game.GetProfileOptionBool(EDayZProfilesOptions.SERVERINFO_DISPLAY), this, false, opt);
		m_PauseSelector				= new OptionSelectorMultistate(m_Root.FindAnyWidget("pause_setting_option"), m_PauseOption.GetIndex(), this, false, opt4);
			
		m_ShowServerInfoSelector.m_OptionChanged.Insert(UpdateServerInfoOption);
		m_PauseSelector.m_OptionChanged.Insert(UpdatePauseOption);
		#endif
		
		float x, y, y2;
		m_Root.FindAnyWidget("game_settings_scroll").GetScreenSize(x, y);
		m_Root.FindAnyWidget("game_settings_root").GetScreenSize(x, y2);
		int f = (y2 > y);
		m_Root.FindAnyWidget("game_settings_scroll").SetAlpha(f);
		
		m_Root.SetHandler(this);
		m_DetailsRoot.Show(false);
		
		InitDependentOptionsVisibility();
	}
	
	void ~OptionsMenuGame()
	{
		if (m_FOVOption)
		{
			m_FOVOption.Revert();
			g_Game.SetUserFOV(m_FOVOption.ReadValue());
		}
		
		RevertHUDBrightness();
	}
	
	string GetLayoutName()
	{
		#ifdef PLATFORM_CONSOLE
		return "gui/layouts/new_ui/options/xbox/game_tab.layout";
		#else
		return "gui/layouts/new_ui/options/pc/game_tab.layout";
		#endif
	}
	
	void Focus()
	{
		#ifdef PLATFORM_CONSOLE
		SetFocus(m_LanguageSelector.GetParent());
		#endif
	}
	
	bool IsChanged()
	{
		bool universal = (m_ShowHUDSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD) || m_ShowCrosshairSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.CROSSHAIR));
		universal = universal || m_HUDBrightnessSelector.GetValue() != g_Game.GetProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS);
		universal = universal || m_ShowHUDVehicleSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD_VEHICLE);
		universal = universal || (m_ShowGameSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.GAME_MESSAGES) || m_ShowAdminSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.ADMIN_MESSAGES));
		universal = universal || m_ShowPlayerSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.PLAYER_MESSAGES);
		universal = universal || m_BleedingIndication.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION);
		universal = universal || m_ConnectivityInfo.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO);
		universal = universal || m_LanguageOption.IsChanged();
		
		#ifdef PLATFORM_CONSOLE
		return universal;
		#else
		return (universal || m_ShowQuickbarSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.QUICKBAR) || m_ShowServerInfoSelector.GetValue() != g_Game.GetProfileOptionBool(EDayZProfilesOptions.SERVERINFO_DISPLAY));
		#endif
	}
	
	void Apply()
	{
		IngameHud hud = GetHud();
		InGameMenu menu = InGameMenu.Cast(GetGame().GetUIManager().FindMenu(MENU_INGAME));
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.HUD, m_ShowHUDSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.CROSSHAIR, m_ShowCrosshairSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.HUD_VEHICLE, m_ShowHUDVehicleSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.GAME_MESSAGES, m_ShowGameSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.ADMIN_MESSAGES, m_ShowAdminSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.PLAYER_MESSAGES, m_ShowPlayerSelector.GetValue());
		g_Game.SetProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS, m_HUDBrightnessSelector.GetValue());
		
		#ifndef PLATFORM_CONSOLE
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.QUICKBAR, m_ShowQuickbarSelector.GetValue());
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.SERVERINFO_DISPLAY, m_ShowServerInfoSelector.GetValue());
		#endif
		
		if (hud)
		{
			#ifndef PLATFORM_CONSOLE
			hud.ShowQuickBar(m_ShowQuickbarSelector.GetValue());
			#else
			hud.ShowQuickBar(GetGame().GetInput().IsEnabledMouseAndKeyboardEvenOnServer());
			#endif
			hud.ShowHud(m_ShowHUDSelector.GetValue());
			hud.SetVehicleHudDisabled(!m_ShowHUDVehicleSelector.GetValue());
		}
		
		bool bleedingIndicationOld = g_Game.GetProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION);
		bool bleedingIndicationNew = m_BleedingIndication.GetValue();
		bool connectivityOld = g_Game.GetProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO);
		bool connectivityNew = m_ConnectivityInfo.GetValue();
		
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION, bleedingIndicationNew);
		g_Game.SetProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO, connectivityNew);
		
		if (mission)
		{
			//BLEEDINGINDICATION changed
			if (bleedingIndicationOld != bleedingIndicationNew)
			{
				OnBleedingIndicationChanged(bleedingIndicationNew);
			}
			
			//CONNECTIVITY_INFO changed
			if (connectivityOld != connectivityNew)
			{
				mission.GetConnectivityInvoker().Invoke(connectivityNew);
			}
		}
		
		#ifndef PLATFORM_CONSOLE
		if (menu)
		{
			menu.SetServerInfoVisibility(m_ShowServerInfoSelector.GetValue());
		}
		#endif
		
		if (m_LanguageOption.IsChanged()) //language changed
		{
			if (mission)
			{
				mission.ResetGUI();
			}
			else
			{
				ErrorEx("mission not found!", ErrorExSeverity.INFO);
			}
		}
	}
	
	void Revert()
	{
		if (m_ShowHUDSelector)
			m_ShowHUDSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD), false);
		
		RevertHUDBrightness();

		if (m_ShowCrosshairSelector)
			m_ShowCrosshairSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.CROSSHAIR), false);
		
		if (m_ShowHUDVehicleSelector)
			m_ShowHUDVehicleSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.HUD_VEHICLE), false);
		
		if (m_FOVOption)
		{
			m_FOVSelector.SetValue(m_FOVOption.ReadValue(), false);
			g_Game.SetUserFOV(m_FOVOption.ReadValue());
		}
		if (m_LanguageOption)
			m_LanguageSelector.SetValue(m_LanguageOption.GetIndex(), false);
		if (m_ShowGameSelector)
			m_ShowGameSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.GAME_MESSAGES), false);
		if (m_ShowAdminSelector)
			m_ShowAdminSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.ADMIN_MESSAGES), false);
		if (m_ShowPlayerSelector)
			m_ShowPlayerSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.PLAYER_MESSAGES), false);
		if (m_BleedingIndication)
			m_BleedingIndication.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.BLEEDINGINDICATION), false);
		if (m_ConnectivityInfo)
			m_ConnectivityInfo.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.CONNECTIVITY_INFO), false);
		
		#ifdef PLATFORM_CONSOLE
			if (m_BrightnessSelector)
				m_BrightnessSelector.SetValue(m_BrightnessOption.ReadValue(), false);
		#else
			if (m_ShowQuickbarSelector)
				m_ShowQuickbarSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.QUICKBAR), false);
			if (m_ShowServerInfoSelector)
				m_ShowServerInfoSelector.SetValue(g_Game.GetProfileOptionBool(EDayZProfilesOptions.SERVERINFO_DISPLAY), false);
			if (m_PauseOption)
			{
				m_PauseSelector.SetValue(m_PauseOption.GetIndex(), false);
			}
		#endif
	}
	
	void SetToDefaults()
	{
		g_Game.ResetProfileOptions();
		
		if (m_ShowHUDSelector)
			m_ShowHUDSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.HUD), false);

		if (m_HUDBrightnessSelector)
		{
			m_HUDBrightnessSelector.SetValue(g_Game.GetProfileOptionDefaultFloat(EDayZProfilesOptions.HUD_BRIGHTNESS), false);
			g_Game.SetHudBrightness(m_HUDBrightnessSelector.GetValue());
		}

		if (m_ShowCrosshairSelector)
			m_ShowCrosshairSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.CROSSHAIR), false);
		
		if (m_ShowHUDVehicleSelector)
			m_ShowHUDVehicleSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.HUD_VEHICLE), false);
		
		if (m_FOVOption)
		{
			m_FOVOption.WriteValue(m_FOVOption.GetDefault());
			m_FOVSelector.SetValue(m_FOVOption.ReadValue(), false);
			g_Game.SetUserFOV(m_FOVOption.GetDefault());
		}
		if (m_ShowGameSelector)
			m_ShowGameSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.GAME_MESSAGES), false);
		if (m_ShowAdminSelector)
			m_ShowAdminSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.ADMIN_MESSAGES), false);
		if (m_ShowPlayerSelector)
			m_ShowPlayerSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.PLAYER_MESSAGES), false);
		if (m_BleedingIndication)
			m_BleedingIndication.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.BLEEDINGINDICATION), false);
		if (m_ConnectivityInfo)
			m_ConnectivityInfo.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.CONNECTIVITY_INFO), false);
		
		#ifdef PLATFORM_CONSOLE
			if (m_BrightnessSelector)
			{
				m_BrightnessOption.WriteValue(m_BrightnessOption.GetDefault());
				m_BrightnessSelector.SetValue(m_BrightnessOption.GetDefault(), false);
			}
		#else
			if (m_ShowQuickbarSelector)
				m_ShowQuickbarSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.QUICKBAR), false);
			if (m_ShowServerInfoSelector)
				m_ShowServerInfoSelector.SetValue(g_Game.GetProfileOptionDefaultBool(EDayZProfilesOptions.SERVERINFO_DISPLAY), false);
			if (m_PauseOption)
			{
				m_PauseOption.SetIndex(m_PauseOption.GetDefaultIndex());
				m_PauseSelector.SetValue(m_PauseOption.GetIndex(), false);
			}
		#endif
	}
	
	void ReloadOptions()
	{
		m_Menu.ReloadOptions();
	}
	
	void SetOptions(GameOptions options)
	{
		m_Options = options;
		
		m_FOVOption			= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW));
		m_LanguageOption	= ListOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_LANGUAGE));
		
		if (m_FOVSelector)
			m_FOVSelector.SetValue(m_FOVOption.ReadValue(), false);
		
		if (m_LanguageOption)
			m_LanguageSelector.SetValue(m_LanguageOption.GetIndex(), false);
		
		#ifdef PLATFORM_CONSOLE
			m_BrightnessOption	= NumericOptionsAccess.Cast(m_Options.GetOptionByType(OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER));
			if (m_BrightnessOption)
				m_BrightnessSelector.SetValue(m_BrightnessOption.ReadValue(), false);
		#endif
	}
	
	void ToggleDependentOptions(int mode, bool state)
	{
	}
	
	void InitDependentOptionsVisibility()
	{
		#ifdef PLATFORM_CONSOLE
		m_QuickbarButton.Show(false);
		#endif
	}
	
	protected void UpdateLanguageOption(int new_index)
	{
		m_LanguageOption.SetIndex(new_index);
		m_Menu.OnChanged();

		TextMapUpdateWidget(OptionAccessType.AT_OPTIONS_LANGUAGE);
	}
	
	protected void UpdatePauseOption(int new_index)
	{
		m_PauseOption.SetIndex(new_index);
		m_Menu.OnChanged();
	}
	
	protected void UpdateFOVOption(float new_value)
	{
		m_FOVOption.WriteValue(new_value);
		g_Game.SetUserFOV(new_value);
		m_Menu.OnChanged();
	}
	
	protected void UpdateHUDBrightnessOption(float newValue)
	{
		g_Game.SetHudBrightness(newValue);
		m_Menu.OnChanged();
	}
	
	protected void UpdateHUDOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateBleedingIndication(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateConnectivityInfo(int newIndex)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateCrosshairOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateQuickbarOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateHUDVehicleOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateGameOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateAdminOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdatePlayerOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	protected void UpdateServerInfoOption(int new_index)
	{
		m_Menu.OnChanged();
	}
	
	IngameHud GetHud()
	{
		Mission mission = GetGame().GetMission();
		if (mission)
		{
			IngameHud hud = IngameHud.Cast(mission.GetHud());
			return hud;
		}
		return null;
	}
	
#ifdef PLATFORM_CONSOLE
	void UpdateBrightnessOption(float value)
	{
		m_BrightnessOption.WriteValue(value);
		m_Menu.OnChanged();
	}
#endif
	
	override bool OnMouseEnter(Widget w, int x, int y)
	{
		if (w && w.IsInherited(ScrollWidget))
		{
			return false;
		}
		
		if (w && w.IsInherited(SliderWidget))
		{
			return false;
		}
		
		m_Menu.ColorHighlight(w);
		
		return true;
	}
	
	override bool OnMouseLeave(Widget w, Widget enterW, int x, int y)
	{
		if (w && w.IsInherited(ScrollWidget))
		{
			return false;
		}
		
		m_Menu.ColorNormal(w);
		return true;
	}
	
	override bool OnFocus(Widget w, int x, int y)
	{
		if (m_Menu)
		{
			m_Menu.OnFocus(w, x, y);
		}
		
		if (w)
		{
			if (TextMapUpdateWidget(w.GetUserID())) 
			{
				return true;
			}
			
			if (w.IsInherited(SliderWidget))
			{
				return true;
			}
		}
		m_DetailsRoot.Show(false);
		return (w != null);
	}
	
	protected void OnBleedingIndicationChanged(bool state)
	{
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (mission)
		{
			Param1<bool> par = new Param1<bool>(state);
			mission.GetEffectWidgets().UpdateWidgets(EffectWidgetsTypes.BLEEDING_LAYER,0,par);
		}
	}
	
	bool TextMapUpdateWidget(int key)
	{
		bool connectivityInfoShown = key == OptionIDsScript.OPTION_CONNECTIVITY_INFO;
		Param2<string, string> p;
		Param tmp = m_TextMap.Get(key);
		
		m_DetailsBodyDefault.Show(!connectivityInfoShown);
		m_DetailsBodyConnectivity.Show(connectivityInfoShown);
		
		if (Class.CastTo(p,tmp))
		{
			m_DetailsRoot.Show(true);
			m_DetailsText.Show(true);
			m_DetailsLabel.SetText(p.param1);
			m_DetailsText.SetText(p.param2);
			
			m_DetailsText.Update();
			m_DetailsLabel.Update();
			m_DetailsRoot.Update();
			m_DetailsBodyConnectivity.Update(); //...
			return true;
		}
		return false;
	}
	
	void FillTextMap()
	{
		m_TextMap = new map<int, ref Param2<string, string>>;
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_LANGUAGE, new Param2<string, string>("#options_game_select_language", "#options_game_select_language_desc"));
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_FIELD_OF_VIEW, new Param2<string, string>("#options_game_field_of_view", "#options_game_field_of_view_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_HUD, new Param2<string, string>("#options_game_show_HUD", "#options_game_show_HUD_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_HUD_VEHICLE, new Param2<string, string>("#options_game_show_HUD_vehicle", "#options_game_show_HUD_vehicle_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_HUD_BRIGHTNESS, new Param2<string, string>("#options_game_hud_brightness", "#options_game_hud_brightness_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_CROSSHAIR, new Param2<string, string>("#options_game_show_crosshair", "#options_game_show_crosshair_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_QUICKBAR, new Param2<string, string>("#options_game_show_quickbar",	"#options_game_show_quickbar_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_SERVER_INFO, new Param2<string, string>("#options_game_show_serverinfo", "#options_game_show_serverinfo_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_BLEEDINGINDICATION, new Param2<string, string>("#options_game_show_bleedingindication", "#options_game_show_bleedingindication_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_CONNECTIVITY_INFO, new Param2<string, string>("#options_game_connectivity", ""));
	
		#ifdef PLATFORM_WINDOWS
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_PAUSE, new Param2<string, string>("#layout_options_pc_nopause_title", "#layout_options_pc_nopause_tooltip"));
		m_TextMap.Insert(OptionIDsScript.OPTION_GAME_MESSAGES, new Param2<string, string>("#options_pc_game_messages",	"#options_game_show_game_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_ADMIN_MESSAGES, new Param2<string, string>("#options_pc_admin_mes",		"#options_game_show_admin_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_PLAYER_MESSAGES, new Param2<string, string>("#options_pc_player_messages",	"#options_game_show_player_msg"));
		#else
		#ifdef PLATFORM_XBOX
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER, new Param2<string, string>("#options_video_brightness", "#options_video_brightness_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_GAME_MESSAGES, new Param2<string, string>("#options_xbox_game_messages",	"#options_game_show_game_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_ADMIN_MESSAGES, new Param2<string, string>("#options_xbox_admin_mes",		"#options_game_show_admin_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_PLAYER_MESSAGES, new Param2<string, string>("#options_xbox_player_messages","#options_game_show_player_msg"));
		#else 
		#ifdef PLATFORM_PS4
		m_TextMap.Insert(OptionAccessType.AT_OPTIONS_BRIGHT_SLIDER, new Param2<string, string>("#ps4_options_video_brightness", "#ps4_options_video_brightness_desc"));
		m_TextMap.Insert(OptionIDsScript.OPTION_GAME_MESSAGES, new Param2<string, string>("#ps4_options_game_messages",	"#ps4_options_game_show_game_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_ADMIN_MESSAGES, new Param2<string, string>("#ps4_options_admin_mes",		"#ps4_options_game_show_admin_msg"));
		m_TextMap.Insert(OptionIDsScript.OPTION_PLAYER_MESSAGES, new Param2<string, string>("#ps4_options_player_messages","#ps4_options_game_show_player_msg"));		
		#endif
		#endif
		#endif
	}
	
	private void RevertHUDBrightness()
	{
		if (m_HUDBrightnessSelector)
		{
			m_HUDBrightnessSelector.SetValue(g_Game.GetProfileOptionFloat(EDayZProfilesOptions.HUD_BRIGHTNESS), false);
			g_Game.SetHudBrightness(m_HUDBrightnessSelector.GetValue());
		}	
	}
}

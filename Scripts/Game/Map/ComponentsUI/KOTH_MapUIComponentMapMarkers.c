class KOTH_MapUIComponentMapMarkers : SCR_MapUIBaseComponent
{
	//! Objective marker config attributes
	[Attribute(defvalue: "0", desc: "If enabled then the defined marker color will be applied on the objective zone marker icon. If not the basic image color will be used.")]
	protected bool m_bUseObjectiveMarkerColor;
	[Attribute("0.000000 0.616999 0.583993 1.000000", UIWidgets.ColorPicker, desc: "Main color that will be used for the main objective zone marker.")]
	protected ref Color m_iObjectiveMarkerColor;
	[Attribute("0.000000 0.616999 0.583993 1.000000", UIWidgets.ColorPicker, desc: "Main color that will be used for the main objective zone marker text.")]
	protected ref Color m_iObjectiveMarkerTextColor;
	[Attribute("", UIWidgets.ResourceNamePicker, desc: "Main icon or imageset that will be used for the the main objective zone marker.", params: "edds imageset")]
	protected ResourceName m_rObjectiveMarkerIcon;
	[Attribute("", UIWidgets.EditBox , desc: "Imageset icon name if imageset is used for the the main objective zone marker.")]
	protected string m_rObjectiveMarkerIconName;
	[Attribute("34.0", UIWidgets.EditBox , desc: "Size of the marker icon used for the the main objective zone marker.")]
	protected float m_fObjectiveMarkerIconSize;
	
	//! Player marker config attributes
	[Attribute("0.000000 0.616999 0.583993 1.000000", UIWidgets.ColorPicker, desc: "Main color that will be used for the player marker.")]
	protected ref Color m_iPlayerMarkerColor;
	[Attribute("{0250208EA4A9AB25}UI/Textures/Icons/icons_wrapperUI-32-glow.imageset", UIWidgets.ResourceNamePicker, desc: "Main icon or imageset that will be used for the the player marker.", params: "edds imageset")]
	protected ResourceName m_rPlayerMarkerIcon;
	[Attribute("compass", UIWidgets.EditBox , desc: "Imageset icon name if imageset is used for the the player marker.")]
	protected string m_rPlayerMarkerIconName;
	[Attribute("34.0", UIWidgets.EditBox , desc: "Size of the marker icon used for the the player marker.")]
	protected float m_fPlayerMarkerIconSize;
	
	//! Safe zone marker config attributes
	[Attribute("{B1E5566B0FA239A4}UI/icons/marker_64x64.edds", UIWidgets.ResourceNamePicker, desc: "Main icon or imageset that will be used for the the safe zone markers.", params: "edds imageset")]
	protected ResourceName m_rSafeZoneMarkerIcon;
	[Attribute("", UIWidgets.EditBox , desc: "Imageset icon name if imageset is used for the the safe zone markers.")]
	protected string m_rSafeZoneMarkerIconName;
	[Attribute("34.0", UIWidgets.EditBox , desc: "Size of the marker icon used for the the safe zone markers.")]
	protected float m_fSafeZoneMarkerIconSize;
	
	protected ref array<ref KOTH_MapMarker> m_MapMarkers = new array<ref KOTH_MapMarker>;
	protected ref KOTH_MapMarker m_ObjectiveMarker;
	protected ref KOTH_PlayerMapMarker m_PlayerMarker;
	protected ref array<ref KOTH_PlayerMapMarker> m_TeamMarkers = new array<ref KOTH_PlayerMapMarker>;
	protected SCR_ChimeraCharacter m_Player;
	protected int m_PlayerID = -1;
	protected bool m_Enabled = true;

	protected KOTH_GameModeBase m_GameMode;
	protected KOTH_ZoneManager m_ZoneManager;

	void Update()
	{
		//super.Update();

		if (!m_Enabled) return;

		if (m_PlayerMarker)
			m_PlayerMarker.Update();
		
		if (m_ObjectiveMarker)
			m_ObjectiveMarker.Update();

		if (m_TeamMarkers && m_TeamMarkers.Count() > 0) {
			foreach (KOTH_PlayerMapMarker teamMarker: m_TeamMarkers) {
				teamMarker.Update();
			}
		}
		
		if (m_MapMarkers && m_MapMarkers.Count() > 0) {
			foreach (KOTH_MapMarker mapMarker: m_MapMarkers) {
				mapMarker.Update();
			}
		}
	}

	override void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);

		if (!m_Enabled) 
			return;

		if (m_PlayerMarker)
			delete m_PlayerMarker;

		if (m_ObjectiveMarker)
			delete m_ObjectiveMarker;

		for (int i = 0; i < m_MapMarkers.Count(); i++) {
			m_MapMarkers.Remove(i);
		}
	}

	override void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);

		if (!m_Enabled) return;

		if (!m_RootWidget)
			return;

		//! Create player position marker
		if (!m_PlayerMarker) {
			m_Player = SCR_ChimeraCharacter.Cast(SCR_PlayerController.GetLocalControlledEntity());
			if (!m_Player) 
				return;
			
			m_PlayerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(m_Player);
			if (m_PlayerID == -1)
				return;
			
			m_PlayerMarker = new KOTH_PlayerMapMarker(m_RootWidget, m_Player);
			if (m_rPlayerMarkerIconName != string.Empty && m_rPlayerMarkerIcon != ResourceName.Empty) {
				m_PlayerMarker.SetIconFromSet(m_rPlayerMarkerIcon, m_rPlayerMarkerIconName);
			}
			else if (m_rPlayerMarkerIconName == string.Empty && m_rPlayerMarkerIcon != ResourceName.Empty) {
				m_PlayerMarker.SetIcon(m_rPlayerMarkerIcon);
			}
			
			m_PlayerMarker.SetColor(m_iPlayerMarkerColor);
			
			int playerID = SCR_PlayerController.GetLocalPlayerId();
			PlayerManager playerManager = GetGame().GetPlayerManager();
			string playerName = playerManager.GetPlayerName(playerID);
			string formatedName = FilterName(playerName);
			m_PlayerMarker.SetLabel(formatedName);
			m_PlayerMarker.SetIconSize(m_fPlayerMarkerIconSize, m_fPlayerMarkerIconSize);
		}
		
		//! Create team-mate markers
		foreach(int playerId, KOTH_PlayerUIData data: m_GameMode.GetPlayerUIDatas())
		{
			if (playerId == m_PlayerID)
				continue;
			
			SCR_ChimeraCharacter player = SCR_ChimeraCharacter.Cast(GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId));
			if (!player) 
				continue;
			
			if (player.GetFactionKey() != m_Player.GetFactionKey())
				continue;
			
			FactionManager factionManager = GetGame().GetFactionManager();
			if (!factionManager)
				return;
			
			Faction faction = factionManager.GetFactionByKey(player.GetFactionKey());
			Color factionColor = faction.GetFactionColor();
			KOTH_PlayerMapMarker playerMarker = new KOTH_PlayerMapMarker(m_RootWidget, player);
			PlayerManager playerManager = GetGame().GetPlayerManager();
			string playerName = playerManager.GetPlayerName(playerId);
			string formatedName = FilterName(playerName);
			playerMarker.SetLabel(formatedName);
			playerMarker.SetIconFromSet("{0250208EA4A9AB25}UI/Textures/Icons/icons_wrapperUI-32-glow.imageset", "compass");
			playerMarker.SetIconSize(32, 32);
			playerMarker.SetColor(factionColor);
			m_TeamMarkers.Insert(playerMarker);
		}
		
		//! Create objective marker
		if (!m_ObjectiveMarker)
		{
			m_ObjectiveMarker = new KOTH_MapMarker(m_RootWidget, m_ZoneManager.GetZone().GetWorldZoneCenter());
			if (m_rObjectiveMarkerIconName != string.Empty && m_rObjectiveMarkerIcon != ResourceName.Empty) {
				m_ObjectiveMarker.SetIconFromSet(m_rObjectiveMarkerIcon, m_rObjectiveMarkerIconName);
			}
			else if (m_rObjectiveMarkerIconName == string.Empty && m_rObjectiveMarkerIcon != ResourceName.Empty) {
				m_ObjectiveMarker.SetIcon(m_rObjectiveMarkerIcon);
			}
			
			if (m_bUseObjectiveMarkerColor)
				m_ObjectiveMarker.SetColor(m_iObjectiveMarkerColor);
			
			m_ObjectiveMarker.SetTextColor(m_iObjectiveMarkerTextColor);
			m_ObjectiveMarker.SetLabel("KOTH");
			m_ObjectiveMarker.SetIconSize(m_fObjectiveMarkerIconSize, m_fObjectiveMarkerIconSize);
		}

		//! Create safe zone markers
		foreach (KOTH_SafeZoneTriggerEntity safe_zone: m_ZoneManager.GetSafeZones()) {
			KOTH_MapMarker safeZoneMarker = new KOTH_MapMarker(m_RootWidget, safe_zone.GetWorldSafeZoneCenter());
			SCR_Faction faction = safe_zone.GetFaction();
			if (faction)
			{
				if (m_rSafeZoneMarkerIconName != string.Empty && m_rSafeZoneMarkerIcon != ResourceName.Empty) {
					safeZoneMarker.SetIconFromSet(m_rSafeZoneMarkerIcon, m_rSafeZoneMarkerIconName);
				}
				else if (m_rSafeZoneMarkerIconName == string.Empty && m_rSafeZoneMarkerIcon != ResourceName.Empty) {
					safeZoneMarker.SetIcon(m_rSafeZoneMarkerIcon);
				}
				
				safeZoneMarker.SetIcon(m_rSafeZoneMarkerIcon);
				safeZoneMarker.SetColor(faction.GetFactionColor());
				safeZoneMarker.SetTextColor(faction.GetFactionColor());
				safeZoneMarker.SetLabel("SAFE ZONE - " + faction.GetFactionName());
				safeZoneMarker.SetIconSize(m_fSafeZoneMarkerIconSize, m_fSafeZoneMarkerIconSize);
			}

			m_MapMarkers.Insert(safeZoneMarker);
		}
	}
	
	string FilterName(string name)
    {
		string partToRemove;
		for (int i = 0; i < name.Length(); i++) {
			string char = name.Get(i);
			if (char == "\\" ) {
				partToRemove = name.Substring(0, i + 1);
				break;
			}
		}
		
		name.Replace(partToRemove, "");
		
        return name;
    }

	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		super.Init();

		m_GameMode = KOTH_GameModeBase.Cast(GetGame().GetGameMode());
		if (!m_GameMode) {
			Print("Could not find game mode!", LogLevel.ERROR);
			return;
		}

		m_ZoneManager = m_GameMode.GetKOTHZoneManager();
		if (!m_ZoneManager) {
			Print("Could not find zone manager!", LogLevel.ERROR);
			return;
		}

		m_Enabled = m_GameMode.UseMapMarkerComponent();
		
		//! Disable the component if not enabled in game mode settings
		if (!m_Enabled)
			SetActive(false);
	}
};

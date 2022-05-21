//------------------------------------------------------------------------------------------------
/*!
	This object serves as a wrapper for individual HUD team score widgets.
*/
class SCR_KOTHTeamScoreDisplayObject
{
	//! Topmost frame of this object
	protected Widget m_wRoot;

	//! Reference to text widget that displays player count
	protected TextWidget m_wPlayerCountText;
	
	//! Reference to fillable bar widget
	protected ImageWidget m_wFactionImage;

	//! Reference to text widget that displays score
	protected TextWidget m_wScoreText;

	//! Faction this object represents
	protected Faction m_pAffiliatedFaction;

	//------------------------------------------------------------------------------------------------
	/*!
		Finds a widget by name, either available variant (default, _Left, _Right)
	*/
	protected Widget FindScoreWidget(notnull Widget parent, string name)
	{
		Widget defaultWidget = parent.FindAnyWidget(name);
		if (defaultWidget)
			return defaultWidget;

		Widget usaWidget = parent.FindAnyWidget(name + "_USA");
		if (usaWidget)
			return usaWidget;

		Widget ussrWidget = parent.FindAnyWidget(name + "_USSR");
		if (ussrWidget)
			return ussrWidget;
		
		Widget fiaWidget = parent.FindAnyWidget(name + "_FIA");
		if (fiaWidget)
			return fiaWidget;

		return null;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Creates new score object wrapper for provided faction.
	*/
	void SCR_KOTHTeamScoreDisplayObject(notnull Widget root, notnull Faction faction)
	{
		m_wRoot = root;
		m_wFactionImage = ImageWidget.Cast(FindScoreWidget(root, "Icon_Faction"));
		m_wScoreText = TextWidget.Cast(FindScoreWidget(root, "ScoreBar_Text"));
		m_wPlayerCountText = TextWidget.Cast(FindScoreWidget(root, "ScoreBar_PlayerCount")); 

		m_pAffiliatedFaction = faction;

		ResourceName iconResource;
		SCR_Faction scrFaction = SCR_Faction.Cast(faction);
		if (scrFaction)
		{
			iconResource = scrFaction.GetFactionFlag();
		}
		else
		{
			// Set icon directly
			UIInfo factionInfo = faction.GetUIInfo();
			if (factionInfo)
			iconResource = factionInfo.GetIconPath();
		}

		if (!iconResource.IsEmpty())
		{
			m_wFactionImage.SetColor(Color.White);
			m_wFactionImage.LoadImageTexture(0, iconResource);
			m_wFactionImage.SetImage(0);
		}
		else
			m_wFactionImage.SetColor(faction.GetFactionColor());

		// Color elements
		m_wRoot.SetColor(faction.GetFactionColor());
	}

	//------------------------------------------------------------------------------------------------
	void ~SCR_KOTHTeamScoreDisplayObject()
	{
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Updates displayed tickets value of affiliated faction
	*/
	void UpdateScore(int score)
	{
		m_wScoreText.SetText(score.ToString());
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Updates displayed player count value of affiliated faction
	*/	
	void UpdatePlayerCount(int count)
	{
		m_wPlayerCountText.SetText(count.ToString());
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns the Faction this object is affiliated with.
	*/
	Faction GetFaction()
	{
		return m_pAffiliatedFaction;
	}
}

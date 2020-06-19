#ifdef INTERFACE
CLASS(NexuizReportDialog) EXTENDS(NexuizRootDialog)
	METHOD(NexuizReportDialog, fill, void(entity)) // to be overridden by user to fill the dialog with controls
	ATTRIB(NexuizReportDialog, title, string, "Report a player")
	ATTRIB(NexuizReportDialog, color, vector, SKINCOLOR_DIALOG_TEAMSELECT)
	ATTRIB(NexuizReportDialog, intendedWidth, float, 0.8)
	ATTRIB(NexuizReportDialog, rows, float, 15)
	ATTRIB(NexuizReportDialog, columns, float, 20)
	ATTRIB(NexuizReportDialog, name, string, "ReportMenu")
ENDCLASS(NexuizReportDialog)
#endif

#ifdef IMPLEMENTATION
entity typeNexuizReportDialog;
entity playerNexuizReportDialog;

void reportNexuizReportDialog(entity btn, entity me)
{
	float type = typeNexuizReportDialog.selectedItem;
	float player = playerNexuizReportDialog.selectedItem;
    localcmd("cmd report ", ftos(type), " ", ftos(player + 1), "\n");
    btn.onClickEntity.close(btn.onClickEntity);
}

void fillNexuizReportDialog(entity me)
{
	entity e, vlist;
	
	me.TR(me);
		me.TD(me, 1, 10, e = makeNexuizTextLabel(0, "Naughty behaviour:"));
		typeNexuizReportDialog = e;
		me.TD(me, 1, 10, e = makeNexuizTextLabel(0, "Player:"));
		playerNexuizReportDialog = e;
	me.TR(me);
        me.TD(me, 13, 10, vlist = makeNexuizReportTypeList());
        me.TD(me, 13, 10, vlist.voteArgsList = makeNexuizReportPlayerList());
	me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
    me.TR(me);
        me.TD(me, 1, 10, e = makeNexuizButton("Report", '0 0 0'));
            e.onClick = reportNexuizReportDialog;
            e.onClickEntity = me;
        me.TD(me, 1, 10, e = makeNexuizCommandButton("Cancel", '0 0 0', "", 1));
    
    me.voteList = vlist;
}
#endif


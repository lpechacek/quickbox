import QtQml 2.0
import qf.core 1.0
import qf.qmlwidgets 1.0
import Competitors 1.0
import "qrc:/qf/core/qml/js/treetable.js" as TreeTable
//import shared.QuickEvent 1.0

QtObject {
	id: root
	property CompetitorsPlugin competitorsPlugin

	property QtObject internals: QtObject {
		property SqlTableModel reportModel: SqlTableModel { id: reportModel }
	}

	function
	runCountByClassesTable(stage_id)
	{
		reportModel.queryBuilder.clear()
		.select2("classes", "name")
		.select("COUNT(runs.id) AS runCount")
		.select("MAX(classdefs.mapCount) as mapCount") // classdefs.mapCount must be in any agregation function in PSQL, MIN can be here as well
		.from("classes")
		.joinRestricted("classes.id", "classdefs.classid", "classdefs.stageId={{stage_id}}")
		.join("classes.id", "competitors.classId")
		.joinRestricted("competitors.id", "runs.competitorId", "runs.isRunning AND runs.stageId={{stage_id}}")
		.groupBy("classes.name")
		.orderBy("classes.name");
		reportModel.setQueryParameters({stage_id: stage_id})
		reportModel.reload();
		var tt = new TreeTable.Table();
		tt.setData(reportModel.toTreeTableData());
		return tt;
	}

	function printCompetitorsStatistics()
	{
		Log.info("printCompetitorsStatistics triggered");
		var event_plugin = FrameWork.plugin("Event");
		var stage_id = event_plugin.currentStageId;

		var tt = new TreeTable.Table();

		reportModel.queryBuilder.clear()
			.select2('classes', 'id, name').from('classes').orderBy('classes.name');
		reportModel.reload();
		tt.setData(reportModel.toTreeTableData());
		tt.setValue("event", event_plugin.eventConfig.value("event"));
		for(var e=0; e<event_plugin.stageCount; e++) {
			var prefix = "e" + (e+1) + "_"
			var col_runs_count = prefix + "runCount";
			var col_map_count = prefix + "mapCount";
			tt.addColumn(col_runs_count, "int");
			tt.addColumn(col_map_count, "int");
			var tt2 = runCountByClassesTable(e+1);
			//console.warn(e, "tt2", tt2.toString());
			for(var i=0; i<tt2.rowCount(); i++) {
				//console.debug("class id:", class_id);
				tt.setValue(i, col_runs_count, tt2.value(i, "runCount"));
				tt.setValue(i, col_map_count, tt2.value(i, "mapCount"));
			}
		}
		//console.warn("tt", tt.toString());
		QmlWidgetsSingleton.showReport(competitorsPlugin.manifest.homeDir + "/reports/competitorsStatistics.qml"
									   , tt.data()
									   , qsTr("Competitors statistics")
									   , ""
									   , {stageCount: event_plugin.stageCount});
	}
}

import qf.qmlreports 1.0
import shared.QuickEvent.reports 1.0
import "qrc:/quickevent/js/ogtime.js" as OGTime

Report {
	id: root
	objectName: "root"

	property string reportTitle: qsTr("Start list by classes")
	property bool isBreakAfterEachClass: false
	property bool isColumnBreak: false
	property bool isPrintStartNumbers: false

	//debugLevel: 1
	styleSheet: StyleSheet {
		objectName: "portraitStyleSheet"
		basedOn: ReportStyleCommon { id: myStyle }
		colors: [
		]
		pens: [
			Pen {name: "red1dot"
				basedOn: "black1"
				color: Color {def:"red"}
				style: Pen.DotLine
			},
			Pen {
				id: pen_black1
				basedOn: "black1"
			}
		]
	}
	textStyle: myStyle.textStyleDefault

	width: 210
	height: 297
	hinset: 5
	vinset: 5
	Frame {
		width: "%"
		height: "%"
		layout: Frame.LayoutStacked
		QuickEventHeaderFooter {
			reportTitle: root.reportTitle
		}
		Frame {
			width: "%"
			height: "%"
			columns: "%,%"
			vinset: 10
			Band {
				id: band
				objectName: "band"
				width: "%"
				height: "%"
				QuickEventReportHeader {
					dataBand: band
					reportTitle: root.reportTitle
				}
				Space { height: 5 }
				Detail {
					id: detail
					objectName: "detail"
					width: "%"
					//keepAll: true
					layout: Frame.LayoutVertical
					function dataFn(field_name) {return function() {return rowData(field_name);}}
					Break {
						breakType: root.isColumnBreak? Break.Column: Break.Page;
						visible: root.isBreakAfterEachClass;
						skipFirst: true
					}
					Frame {
						width: "%"
						layout: Frame.LayoutHorizontal
						fill: Brush {color: Color {def: "khaki"} }
						Cell {
							width: "%"
							textFn: detail.dataFn("classes.name");
							textStyle: myStyle.textStyleBold
						}
						Cell {
							textFn: function() { return qsTr("length: ") + detail.rowData("courses.length");}
						}
						Cell {
							textFn: function() { return qsTr("climb: ") + detail.rowData("courses.climb");}
						}
					}
					//expandChildrenFrames: true
					Band {
						id: runnersBand
						objectName: "runnersBand"
						keepFirst: 3
						keepWithPrev: true
						htmlExportAsTable: true
						Detail {
							id: runnersDetail
							objectName: "runnersDetail"
							width: "%"
							layout: Frame.LayoutHorizontal
							function dataFn(field_name) {return function() {return rowData(field_name);}}
							Cell {
								width: 15
								halign: Frame.AlignRight
								textFn: function() { return OGTime.msecToString_mmss(runnersDetail.rowData("startTimeMs"));}
							}
							Para {
								visible: root.isPrintStartNumbers
								width: 8
								halign: Frame.AlignRight
								textFn: runnersDetail.dataFn("startNumber");
							}
							Cell {
								width: "%"
								textFn: runnersDetail.dataFn("competitorName");
							}
							Para {
								width: 18
								textFn: runnersDetail.dataFn("registration");
							}
							Cell {
								width: 18
								halign: Frame.AlignRight
								textFn: runnersDetail.dataFn("runs.siId");
							}
						}
					}
				}
			}
		}
	}
}



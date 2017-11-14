#include "cardcheckerclassiccpp.h"
#include "readcard.h"

#include <quickevent/si/punchrecord.h>

#include <qf/core/log.h>

namespace CardReader {

CardCheckerClassicCpp::CardCheckerClassicCpp(QObject *parent)
	: Super(parent)
{
	setCaption(tr("Classic C++ (experimental)"));
}

CheckedCard CardCheckerClassicCpp::checkCard(const ReadCard &read_card)
{
	qfDebug() << "read card:" << read_card.toString();

	int run_id = read_card.runId();
	QVariantMap course;
	if(run_id > 0)
		course = courseCodesForRunId(run_id);

	CheckedCard checked_card;
	if(!course.isEmpty())
		return checked_card;

	checked_card.setCourseId(course.value(QStringLiteral("id")).toInt());
	checked_card.setRunId(course.value(QStringLiteral("runId")).toInt());
	//checked_card.setPunches();
	//Log.info("course:", JSON.stringify(read_card, null, 2));
	int stage_id = stageIdForRun(run_id);

	bool error_mis_punch = false;
	QList<CheckedPunch> checked_punches;
	QVariantList course_codes = course.value(QStringLiteral("codes")).toList();
	QList<ReadPunch> read_punches = read_card.punchList();

	//........... normalize times .....................
	// checked card times are in msec relative to run start time
	// startTime, checkTime and finishTime in in msec relative to event start time 00
	int start00sec = stageStartSec(stage_id);
	checked_card.setStageStartTimeMs(start00sec * 1000);
	//checked_card.setCheckTimeMs = null;
	//checked_card.startTimeMs = null;
	if(read_card.checkTime() != 0xEEEE) {
		checked_card.setCheckTimeMs(msecIntervalAM(start00sec * 1000, read_card.checkTime() * 1000));
	}
	//var start_time_sec = null;
	if(read_card.startTime() == 0xEEEE) {        //take start record from start list
		if(run_id > 0) {
			checked_card.setStartTimeMs(startTimeSec(run_id) * 1000);
			//console.warn(start_time_sec);
		}
		bool is_debug = false;
		if(is_debug && checked_card.checkTimeMs() > 0) {
			// take start from check if zero, for testing only
			checked_card.setStartTimeMs((checked_card.checkTimeMs() / 60000 + 1) * 60000);
			qfWarning() << "Taking start time from check for debugging purposes only, start time:" << (checked_card.startTimeMs() / 60000);
		}
	}
	else {
		checked_card.setStartTimeMs(msecIntervalAM(start00sec * 1000, read_card.startTime() * 1000));
	}

	//checked_card.finishTimeMs = null;
	if(read_card.finishTime() == 0xEEEE) {
		error_mis_punch = true;
	}
	else {
		checked_card.setFinishTimeMs(msecIntervalAM(start00sec * 1000, read_card.finishTime() * 1000));
		if(read_card.finishTimeMs())
			checked_card.setFinishTimeMs(checked_card.finishTimeMs() + read_card.finishTimeMs());
	}

	int read_punch_check_ix = 0;
	for(int j=0; j<course_codes.length(); j++) {
		QVariantMap course_code = course_codes[j].toMap();
		CheckedPunch checked_punch;
		checked_punch.setCode(course_code.value(QStringLiteral("code")).toInt());
		checked_punches << checked_punch;
		int k;
		for(k=read_punch_check_ix; k<read_punches.length(); k++) { //scan card
			const ReadPunch &read_punch = read_punches[k];
			int code = course_code.value(QStringLiteral("code")).toInt();
			int alt_code = course_code.value(QStringLiteral("altcode")).toInt();
			qfDebug() << j << k << "looking for:" << checked_punch.code() << "on card:" << read_punch.code() << "vs. code:" << code << "alt:" << alt_code;
			//console.info("code:", JSON.stringify(course_code, null, 2));
			if(read_punch.code() == code || read_punch.code() == alt_code) {
				int read_punch_time_ms = read_punch.time() * 1000;
				if(read_punch.msec())
					read_punch_time_ms += read_punch.msec();
				checked_punch.setStpTimeMs(msecIntervalAM(checked_card.stageStartTimeMs() + checked_card.startTimeMs(), read_punch_time_ms));
				qfDebug() << j << "OK";
				break;
			}
		}
		if(k == read_punches.length()) {
			// code not found
			qfDebug() << j << "NOT FOUND";
			bool code_ooo = course_code.value(QStringLiteral("outoforder")).toBool();
			if(!code_ooo) // for postgres, Query.values() should return lower case keys
				error_mis_punch = true;
		}
		else {
			read_punch_check_ix = k + 1;
		}
	}
	checked_card.setMisPunch(error_mis_punch);

	CheckedPunch finish_punch;
	finish_punch.setCode(quickevent::si::PunchRecord::FINISH_PUNCH_CODE);
	finish_punch.setStpTimeMs(msecIntervalAM(checked_card.startTimeMs(), checked_card.finishTimeMs()));
	checked_punches << finish_punch;

	int prev_stp_time_ms = 0;
	for(int k=0; k<checked_punches.length(); k++) {
		CheckedPunch &checked_punch = checked_punches[k];
		if(checked_punch.stpTimeMs()) {
			if(prev_stp_time_ms > 0)
				checked_punch.setLapTimeMs(checked_punch.stpTimeMs() - prev_stp_time_ms);
			prev_stp_time_ms = checked_punch.stpTimeMs();
		}
		else {
			prev_stp_time_ms = 0;
		}
	}

	qfDebug() << "check result:" << checked_card.toString();
	return checked_card;

}

} // namespace CardReader

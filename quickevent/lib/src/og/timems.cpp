#include "timems.h"

#include <qf/core/log.h>

#include <QString>

using namespace quickevent::og;

TimeMs::TimeMs()
	: m_msec(0), m_isValid(false)
{

}

TimeMs::TimeMs(int msec)
	: m_msec(msec), m_isValid(true)
{

}

QString TimeMs::toString(QChar sec_sep, QChar msec_sep) const
{
	if(!isValid())
		return QString();

	int msec = m_msec % 1000;
	int sec = (m_msec / 1000) % 60;
	int min = m_msec / (1000 * 60);
	QString ret = QString::number(min) + sec_sep;
	if(sec < 10)
		ret += '0';
	ret += QString::number(sec);
	if(!msec_sep.isNull()) {
		ret += msec_sep;
		if(msec < 100)
			ret += '0';
		if(msec < 10)
			ret += '0';
		ret += QString::number(msec);
	}
	return ret;
}

static int str2int(const QString &str)
{
	int ret = 0;
	QString s = str.trimmed();
	if(!s.isEmpty()) {
		bool ok;
		ret = s.toInt(&ok);
		if(!ok)
			qfWarning() << "Invalid OGTime string" << str;
	}
	return ret;
}

TimeMs TimeMs::fromString(const QString &time_str)
{
	if(time_str.isEmpty())
		return TimeMs();

	int msec = 0;
	int sec = 0;
	int min = 0;
	int ix1 = 0;

	int ix2 = time_str.indexOf('.', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(':', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf('-', ix1);
	if(ix2 < 0)
		ix2 = time_str.indexOf(',', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	min = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	ix2 = time_str.indexOf('/', ix1);
	if(ix2 < 0)
		ix2 = time_str.length();
	sec = str2int(time_str.mid(ix1, ix2));
	ix1 = ix2 + 1;

	msec = str2int(time_str.mid(ix1));

	return TimeMs(msec + (sec + (min * 60)) * 1000);
}

int TimeMs::fixTimeWrapAM(int time1_msec, int time2_msec)
{
	constexpr int hr12ms = 12 * 60 * 60 * 1000;
	while(time2_msec < time1_msec)
		time2_msec += hr12ms;
	while(time1_msec <= time2_msec - hr12ms)
		time2_msec -= hr12ms;
	return time2_msec;
}

int TimeMs::msecIntervalAM(int from_time_msec, int to_time_msec)
{
	return fixTimeWrapAM(from_time_msec, to_time_msec) - from_time_msec;
}


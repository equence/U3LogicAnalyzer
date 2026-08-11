#include "softwareupdate.hpp"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStringList>
#include <QVector>

namespace pv {
namespace softwareupdate {

static QVector<int> numeric_version(QString version)
{
	version = version.trimmed();
	if (version.startsWith('v', Qt::CaseInsensitive))
		version.remove(0, 1);
	version = version.section('-', 0, 0);

	QVector<int> parts;
	for (const QString &part : version.split('.')) {
		bool ok = false;
		const int value = part.toInt(&ok);
		if (!ok)
			return {};
		parts.push_back(value);
	}
	return parts;
}

bool is_newer_version(const QString &latest, const QString &current)
{
	QVector<int> latest_parts = numeric_version(latest);
	QVector<int> current_parts = numeric_version(current);
	if (latest_parts.isEmpty() || current_parts.isEmpty())
		return false;

	const int count = qMax(latest_parts.size(), current_parts.size());
	latest_parts.resize(count);
	current_parts.resize(count);
	for (int i = 0; i < count; ++i) {
		if (latest_parts[i] != current_parts[i])
			return latest_parts[i] > current_parts[i];
	}
	return false;
}

bool parse_github_release(const QByteArray &json, ReleaseInfo *release,
	QString *error)
{
	QJsonParseError parse_error;
	const QJsonDocument document = QJsonDocument::fromJson(json, &parse_error);
	if (parse_error.error != QJsonParseError::NoError || !document.isObject()) {
		if (error)
			*error = parse_error.errorString();
		return false;
	}

	const QJsonObject object = document.object();
	const QString tag = object.value("tag_name").toString();
	const QString url = object.value("html_url").toString();
	if (tag.isEmpty() || url.isEmpty()) {
		if (error)
			*error = QStringLiteral("Missing tag_name or html_url");
		return false;
	}

	if (release) {
		release->tag_name = tag;
		release->name = object.value("name").toString(tag);
		release->html_url = url;
	}
	return true;
}

} // namespace softwareupdate
} // namespace pv

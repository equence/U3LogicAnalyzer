#ifndef PULSEVIEW_PV_SOFTWAREUPDATE_HPP
#define PULSEVIEW_PV_SOFTWAREUPDATE_HPP

#include <QByteArray>
#include <QString>

namespace pv {
namespace softwareupdate {

struct ReleaseInfo {
	QString tag_name;
	QString name;
	QString html_url;
};

bool is_newer_version(const QString &latest, const QString &current);
bool parse_github_release(const QByteArray &json, ReleaseInfo *release,
	QString *error = nullptr);

} // namespace softwareupdate
} // namespace pv

#endif // PULSEVIEW_PV_SOFTWAREUPDATE_HPP

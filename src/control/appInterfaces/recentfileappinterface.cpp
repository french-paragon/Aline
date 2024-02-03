#include "recentfileappinterface.h"

#include <QSettings>
#include <QFileInfo>

namespace Aline {

const char* RecentFileAppInterface::RecentFileInterfaceCode = "Aline_AppInterfaces_RecentFiles";
const int RecentFileAppInterface::defaultRecentFilesSaved = 6;

const char* RecentFileAppInterface::RecentFileListSetting = "Aline/RecentFiles/Paths";
const char* RecentFileAppInterface::RecentFileNumberSetting = "Aline/RecentFiles/NFiles";

RecentFileAppInterface::RecentFileAppInterface(QObject *parent)
	: QObject{parent}
{

}

QStringList RecentFileAppInterface::getRecentFilesPaths() const {

	QSettings settings;

	if (!settings.contains(RecentFileListSetting)) {
		return QStringList();
	}

	QStringList lst = settings.value(RecentFileListSetting).toStringList();

	QStringList ret;
	ret.reserve(lst.size());

	for (QString const& path : lst) {
		QFileInfo info(path);

		if (!info.exists()) {
			continue;
		}

		ret.push_back(path);
	}

	return ret;

}
int RecentFileAppInterface::getNRecentFiles() const {

	QSettings settings;

	if (!settings.contains(RecentFileNumberSetting)) {
		return defaultRecentFilesSaved;
	}

	bool ok = true;
	int val = settings.value(RecentFileNumberSetting).toInt(&ok);

	if (!ok) {
		settings.setValue(RecentFileNumberSetting, defaultRecentFilesSaved);
		return defaultRecentFilesSaved;
	}

	return val;

}

void RecentFileAppInterface::addNewRecentFile(QString filePath) {

	QFileInfo infos(filePath);

	QSettings settings;

	QStringList lst = settings.value(RecentFileListSetting).toStringList();
	int maxSize = getNRecentFiles();

	QStringList modified;
	modified.reserve(std::max(lst.size()+1, maxSize));
	modified.push_back(infos.canonicalFilePath());

	for (QString const& path : lst) {
		QFileInfo prevInfos(path);

		if (!prevInfos.exists()) {
			continue;
		}

		if (prevInfos.canonicalFilePath() == infos.canonicalFilePath()) {
			continue;
		}

		modified.push_back(path);
	}

	settings.setValue(RecentFileListSetting, modified);

}

void RecentFileAppInterface::setNumberOfRecentFiles(int num) {

	if (num <= 0) {
		return;
	}

	QSettings settings;
	settings.setValue(RecentFileNumberSetting, num);

	if (!settings.contains(RecentFileListSetting)) {
		return;
	}

	QStringList lst = settings.value(RecentFileListSetting).toStringList();

	QStringList modified;
	modified.reserve(lst.size());

	for (QString const& path : lst) {
		QFileInfo info(path);

		if (!info.exists()) {
			continue;
		}

		modified.push_back(path);
	}

	if (modified.size() > num) {
		modified = modified.mid(0, num);
	}

	settings.setValue(RecentFileListSetting, modified);

}

} // namespace Aline

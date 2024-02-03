#ifndef ALINE_RECENTFILEAPPINTERFACE_H
#define ALINE_RECENTFILEAPPINTERFACE_H

#include <QObject>

namespace Aline {

/*!
 * \brief The RecentFileAppInterface class provide an interface to manage a list of recent files
 *
 * This interface store a persistent list of recent files. It checks that files exists when adding them and as well as before returning the list.
 *
 * The storage mechanism rely on QSettings, so the organisation name and dommain need to be configured in the application.
 */
class RecentFileAppInterface : public QObject
{
	Q_OBJECT
public:
	static const char* RecentFileInterfaceCode;
	static const int defaultRecentFilesSaved;

	static const char* RecentFileListSetting;
	static const char* RecentFileNumberSetting;

	explicit RecentFileAppInterface(QObject *parent = nullptr);

	QStringList getRecentFilesPaths() const;
	int getNRecentFiles() const;

	void addNewRecentFile(QString filePath);
	void setNumberOfRecentFiles(int num);

Q_SIGNALS:

};

} // namespace Aline

#endif // ALINE_RECENTFILEAPPINTERFACE_H

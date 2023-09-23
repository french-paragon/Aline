#ifndef ALINE_APP_H
#define ALINE_APP_H

/*This file is part of the project Aline

Copyright (C) 2022 Paragon <french.paragon@gmail.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "../aline_global.h"

#include <QApplication>

class QTemporaryDir;

namespace Aline {

class EditableItemActionsManager;

class ALINE_EXPORT App : public QApplication
{
	Q_OBJECT
public:

	static App* getAppInstance();

	App(int &argc, char** argv);
	~App();

	virtual bool start(QString appCode = "default");

	/*!
	 * \brief hasActionManager indicate if a dedicated action manager is installed for a type
	 * \param typeRef the type ref
	 * \return true if a manager is installed, false otherwise.
	 */
	bool hasActionManager(QString const& typeRef) const;

	/*!
	 * \brief getManagerForType return the action manager for a given type
	 * \param typeRef the type ref
	 * \return the installed action manager for the type, or the default manager if none is available.
	 */
	EditableItemActionsManager* getActionsManagerForType(QString const& typeRef) const;

	/*!
	 * \brief installManagerForType install a manager for a given type, in no manager has been installed yet.
	 * \param typeRef the type to install the manager for
	 * \param manager the manager to install
	 * \return true if th manager was installed, false otherwise
	 *
	 * The app takes ownership of the manager
	 */
	bool installActionsManagerForType(QString const& typeRef, EditableItemActionsManager* manager);

	/*!
	 * \brief installSpecialInterface install a special interface
	 * \param code the registration code to indentify the interface
	 * \param interface the interface
	 * \return true if the interface has been installed, false otherwise.
	 *
	 * Note that the app take ownership of the interface.
	 *
	 * Interfaces are QObjects implementing special functions that can be queried by parts of the application
	 * or application modules to perform special tasks.
	 *
	 * Interfaces can be registered, and even queried dynamically at runtime, which makes it convinient
	 * for sharing some functions across additional modules.
	 *
	 * Interfaces can be queried with the meta object system, or casted using qobject_cast.
	 *
	 */
	inline bool installSpecialInterface(QString const& code, QObject* interface) {
		if (_appSpecialInterfaces.contains(code)) {
			return false;
		}

		interface->setParent(this);
		_appSpecialInterfaces.insert(code, interface);
		return true;
	}

	/*!
	 * \brief getSpecialInterface get a special interface of the app.
	 * \param interfaceCode the code of the interface.
	 * \return the interface, or nullptr if is does not exist.
	 */
	inline QObject* getSpecialInterface(QString const& interfaceCode) {
		return _appSpecialInterfaces.value(interfaceCode, nullptr);
	}


	/*!
	 * \brief getAppTempDirectory return a temporary directory that will be cleaned up when the application is closed.
	 * \return the path to the directory;
	 */
	QString getAppTempDirectory() const;

protected:

	QString _appCode;

	EditableItemActionsManager* _defaultActionManager;
	QMap<QString, EditableItemActionsManager*> _installedActionManager;

	QMap<QString, QObject*> _appSpecialInterfaces;

	mutable QTemporaryDir* _tmp_dir;
};

} // namespace Aline

#endif // APP_H

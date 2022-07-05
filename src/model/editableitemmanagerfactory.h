#ifndef ALINE_EDITABLEITEMMANAGERFACTORY_H
#define ALINE_EDITABLEITEMMANAGERFACTORY_H

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

#include <QObject>

#include "../aline_global.h"

#include <functional>
#include <QMap>

namespace Aline {

class EditableItemManager;
class MainWindow;

class ALINE_EXPORT EditableItemManagerFactory : public QObject
{
	Q_OBJECT
public:

	struct funcDescr {
		QString _projectDescr;
		QString _longDescr;
	};

	enum funcType {
		SIMPLE_FUNC,
		GUI_FUNC,
		STRING_FUNC
	};

	explicit EditableItemManagerFactory(QObject *parent = nullptr);

	EditableItemManager* createManagerWithRegistredFunction(QString const& fName, QObject* parent = nullptr) const;
	EditableItemManager* createManagerWithRegistredFunctionAndGuiContext(QString const& fName, MainWindow* win, QObject* parent = nullptr, bool fallBackToUnaware = false) const;
	EditableItemManager* createManagerWithRegistredFunctionAndStringSource(QString const& fName, QString source, QObject* parent = nullptr) const;

	funcDescr getSimpleFuncDescr(QString const& fName) const;
	funcDescr getGuiAwareFuncDescr(QString const& fName) const;
	funcDescr getStringSourceFuncDescr(QString const& fName) const;

	void registerSimpleFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*)> const& func);
	void registerGuiAwareFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*, MainWindow*)> const& func);
	void registerStringSourceFunction(QString const& fName, funcDescr const& descr, std::function<EditableItemManager*(QObject*, QString)> const& func);

	bool containFunc(QString const& fname, int type) const;

Q_SIGNALS:

public Q_SLOTS:

protected:

		QMap<QString, std::function<EditableItemManager*(QObject*)> > _simpleFuncs;
		QMap<QString, std::function<EditableItemManager*(QObject*, MainWindow*)> > _guiAwareFuncs;
		QMap<QString, std::function<EditableItemManager*(QObject*, QString)> > _stringSourceFuncs;

		QMap<QString, funcDescr> _simpleFuncsDescriptions;
		QMap<QString, funcDescr> _guiAwareFuncsDescriptions;
		QMap<QString, funcDescr> _stringSourceFuncsDescriptions;

		funcDescr _noFunctionDescr;

};

} // namespace Aline

#endif // ALINE_EDITABLEITEMMANAGERFACTORY_H

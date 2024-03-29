#ifndef EDITORFACTORYMANAGER_H
#define EDITORFACTORYMANAGER_H

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

#include <QAbstractListModel>
#include <functional>

namespace Aline {

class Editor;
class EditorFactory;
class EditableItem;
class MainWindow;

class ALINE_EXPORT EditorFactoryManager : public QAbstractListModel
{
	Q_OBJECT
public:

	using SpecialEditFunction = std::function<void(MainWindow*, EditableItem*)>;

	enum InternalDataRole{
		ItemRefRole = Qt::UserRole + 1
	};

	explicit EditorFactoryManager(QObject *parent = nullptr);

	void installFactory(EditorFactory* factory, bool takeOwnership = true);
	/*!
	 * \brief installQmlEditorFactory install a qml editor factory
	 * \param shadowTypeEditor the shadow type
	 * \param shadowNameEditor the shadow name
	 * \param qmlSource the qml source
	 * \param editableTypes the editable types
	 * The manager will instance its own QmlEditorFactory based on the given parameters. Later, it might also store more information, if, for example, a distance program needs to acess the qml source.
	 */
	void installQmlEditorFactory(QString const& shadowTypeEditor,
								 QString const& shadowNameEditor,
								 QString const& qmlSource,
								 QStringList const& editableTypes);

	bool hasFactoryInstalled(QString type_id) const;
	bool hasFactoryInstalledForItem(QString type_id) const;
	QString factoryInstalledForItem(QString item_type_id) const;
	Editor* createItem(QString type_id, QWidget* parent) const;
	Editor* createItemForEditableItem(EditableItem* item, QWidget* parent) const;

	/*!
	 * \brief registerSpecialEditFunction register a special function called when editing a special type of item
	 * \param type_id the type the function is registered for
	 * \param function the special edit function
	 *
	 * Sometimes we would like some to overing default behavior when trying to edit a certain class of items (opening more than one editor, opening an editor for a different item with a specific configuration, ...).
	 * To enable this, it is possible to register, in addition to editors, special functions that can be called by the main windows when trying to edit an item.
	 */
	void registerSpecialEditFunction(QString type_id, SpecialEditFunction const& function);

	/*!
	 * \brief hasSpecialEditFunctionInstalled check if a special edit function is installed for a given type.
	 * \param type_id the type id.
	 * \return true if a special function
	 */
	bool hasSpecialEditFunctionInstalled(QString type_id) const;
	SpecialEditFunction specialEditFunction(QString type_id) const;

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:

	QMap<QString, QString> _editableTypes;
	QMap<QString, EditorFactory*> _installedFactories;
	QVector<QString> _installedFactoriesKeys;

	QMap<QString, SpecialEditFunction> _specialEditFunctions;
};

} // namespace Aline

#endif // EDITORFACTORYMANAGER_H

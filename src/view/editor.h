#ifndef EDITOR_H
#define EDITOR_H

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

#include <QWidget>
#include <QString>

namespace Aline {

class MainWindow;
class EditableItem;

class ALINE_EXPORT Editor : public QWidget
{
	Q_OBJECT
public:
	explicit Editor(QWidget *parent = nullptr);

	QString title() const;

	/*!
	 * \brief getTypeId return a string id for the editor type.
	 * \return the editor type id.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief getTypeName allow to get the name of the editor item type as a string.
	 * \return the name
	 */
	virtual QString getTypeName() const = 0;

	/*!
	 * \brief editableType allow to get the types the editor might support
	 * \return the type id string, or empty string if none is supported.
	 */
	virtual QStringList editableTypes() const;

	/*!
	 * \brief setEditedItem set the item to edit (by default will do nothing, as generally speaking editor might not support editable item edition).
	 * \param item the item to edit
	 */
	virtual void setEditedItem(EditableItem* item);

	/*!
	 * \brief getEditorMainWindow search the editor parents hierarchy to find the main windows
	 * \return the main windows, or nullptr if none could be found.
	 */
	MainWindow* getEditorMainWindow() const;

	/*!
	 * \brief getContextActions build actions that are currently available for the editor
	 * \return a list of actions
	 *
	 * Editor can have a contextual list of actions, presented as a toolbar.
	 * To do so, they need to override this function (default implementation return an empty list).
	 */
	virtual QList<QAction*> getContextActions();

Q_SIGNALS:

	void titleChanged(Aline::Editor* himself, QString title);
	void contextActionListChanged();

public Q_SLOTS:

	void setTitle(QString title);
	void setHasUnsavedChanges(bool saveState);

	virtual void saveAction() = 0;

protected:

	void updateEffectiveTitle();

	QString _title;
	bool _saveState;
};

} // namespace Aline

#endif // EDITOR_H

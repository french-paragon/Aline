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

#include "qmleditorfactory.h"

#include "editableitemqmleditor.h"

#include <QUrl>

namespace Aline {

QmlEditorFactory::QmlEditorFactory(const QString &shadowTypeEditor, const QString &shadowNameEditor, const QString &qmlSource, const QStringList &editableTypes, QObject *parent) :
	EditorFactory(parent),
	_shadowType(shadowTypeEditor),
	_shadowName(shadowNameEditor),
	_qmlSource(qmlSource)
{
	_hasEditableTypesCached = true;
	_editableTypes = editableTypes;
}

Editor* QmlEditorFactory::createItem(QWidget* parent) const {

	EditableItemQmlEditor* editor = new EditableItemQmlEditor(parent);

	editor->_shadowEditorType = _shadowType;
	editor->_shadowEditorName = _shadowName;
	editor->_editableTypes = _editableTypes;

	editor->setQmlSource(QUrl(_qmlSource));

	return editor;

}

} // namespace Aline

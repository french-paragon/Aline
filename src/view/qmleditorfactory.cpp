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

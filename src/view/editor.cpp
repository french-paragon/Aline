#include "editor.h"

namespace Aline {

Editor::Editor(QWidget *parent) : QWidget(parent)
{

}

void Editor::setTitle(QString title) {
	_title = title;
	updateEffectiveTitle();
}

void Editor::setHasUnsavedChanges(bool saveState) {
	_saveState = saveState;
	updateEffectiveTitle();
}

void Editor::updateEffectiveTitle() {
	Q_EMIT titleChanged(this, _title + ((_saveState) ? " *" : ""));
}

QString Editor::title() const
{
	return _title;
}

QStringList Editor::editableTypes() const {
	return QStringList();
}

void Editor::setEditedItem(EditableItem* item) {
	Q_UNUSED(item);
}

} // namespace Aline

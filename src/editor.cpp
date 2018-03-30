#include "editor.h"

namespace Aline {

Editor::Editor(QWidget *parent) : QWidget(parent)
{

}

void Editor::setTitle(QString title) {
	_title = title;
	emit titleChanged(this, _title);
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

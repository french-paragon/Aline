#include "editor.h"

namespace Aline {

Editor::Editor(QWidget *parent) : QWidget(parent)
{

}

void Editor::setTitle(QString title) {
	_title = title;
	emit titleChanged(_title);
}

QString Editor::title() const
{
	return _title;
}

} // namespace Aline

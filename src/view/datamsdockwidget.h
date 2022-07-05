#ifndef SABRINA_DATAMSDOCKWIDGET_H
#define SABRINA_DATAMSDOCKWIDGET_H

#include <QDockWidget>

#include "../aline_global.h"

namespace Aline {

class MainWindow;
class EditableItemManager;
class ProjectFilterByLabelProxyModel;

namespace Ui {
	class DataItemsDockWidget;
}

class ALINE_EXPORT DataItemsDockWidget : public QDockWidget
{
	Q_OBJECT

public:
	explicit DataItemsDockWidget(MainWindow *parent = 0);
	~DataItemsDockWidget();

Q_SIGNALS:

	void itemDoubleClicked(QString itemRef);

private:

	void selectionChanged();

	void receiveDoubleClick(const QModelIndex &index);

	MainWindow * _mw_parent;

	Ui::DataItemsDockWidget *ui;

	Aline::ProjectFilterByLabelProxyModel* _proxy;

	Aline::EditableItemManager* _currentProject;
};


} // namespace Sabrina
#endif // SABRINA_DATAMSDOCKWIDGET_H

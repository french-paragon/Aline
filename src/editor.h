#ifndef EDITOR_H
#define EDITOR_H

#include "aline_global.h"

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

signals:

	void titleChanged(Editor* himself, QString title);

public slots:

	void setTitle(QString title);

	virtual void saveAction() = 0;

protected:

	QString _title;
};

} // namespace Aline

#endif // EDITOR_H

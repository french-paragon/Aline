#ifndef ALINE_EDITABLEITEM_H
#define ALINE_EDITABLEITEM_H

#include <QObject>

namespace Aline {

class EditableItem : public QObject
{
	Q_OBJECT
public:
	explicit EditableItem(QObject *parent = nullptr);

	/*!
	 * \brief getTypeId allow to get info on the type of the editable item.
	 * \return a type id as a string.
	 */
	virtual QString getTypeId() const = 0;

	/*!
	 * \brief getTypeName allow to get the name of the editable item type as a string.
	 * \return the name
	 */
	virtual QString getTypeName() const = 0;

signals:

public slots:
};

} // namespace Aline

#endif // ALINE_EDITABLEITEM_H

#ifndef ALINE_EDITABLEITEM_H
#define ALINE_EDITABLEITEM_H

#include <QObject>

#include "aline_global.h"

namespace Aline {

class ALINE_EXPORT EditableItem : public QObject
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

	virtual bool getHasUnsavedChanged() const = 0;

	virtual bool autoSave() const;

signals:

	void unsavedStateChanged(bool saveState);

public slots:

	virtual bool save() = 0;
};

} // namespace Aline

#endif // ALINE_EDITABLEITEM_H

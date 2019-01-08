#ifndef ALINE_ITEMIOEXCEPTION_H
#define ALINE_ITEMIOEXCEPTION_H

#include "../../aline_global.h"

#include <QException>

namespace Aline {

class EditableItemManager;

class ALINE_EXPORT ItemIOException : public QException
{

public:

	ItemIOException (QString ref, QString infos, EditableItemManager const* manager);
	ItemIOException (ItemIOException const& other);

	const char* what() const throw();

	void raise() const;
	ItemIOException *clone() const;

	QString ref() const;
	QString infos() const;
	const EditableItemManager *manager() const;

protected:

	QString _ref;
	QString _infos;
	std::string _what;
	EditableItemManager const* _manager;
};

} // namespace Aline

#endif // ALINE_ITEMIOEXCEPTION_H

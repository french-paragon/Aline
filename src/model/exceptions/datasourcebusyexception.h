#ifndef ALINE_DATASOURCEBUSYEXCEPTION_H
#define ALINE_DATASOURCEBUSYEXCEPTION_H

#include "../../aline_global.h"

#include <QException>

namespace Aline {

class DataSourceBusyException : public QException
{
public:
	DataSourceBusyException(QString const& what);
	DataSourceBusyException (DataSourceBusyException const& other);

	const char* what() const throw();

	void raise() const;
	DataSourceBusyException* clone() const;

protected:

	QString _what;
};

} // namespace Aline

#endif // ALINE_DATASOURCEBUSYEXCEPTION_H

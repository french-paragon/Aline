#include "datasourcebusyexception.h"

namespace Aline {

DataSourceBusyException::DataSourceBusyException(const QString &what) :
	_what(what)
{

}
DataSourceBusyException::DataSourceBusyException(DataSourceBusyException const& other) :
	_what(other.what())
{

}

const char* DataSourceBusyException::what() const throw() {
	return _what.toStdString().c_str();
}

void DataSourceBusyException::raise() const {
	throw *this;
}
DataSourceBusyException* DataSourceBusyException::clone() const {
	return new DataSourceBusyException(*this);
}

} // namespace Aline

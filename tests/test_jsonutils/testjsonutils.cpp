#include <QTest>

#include "../../src/utils/jsonutils.h"
#include "../../src/model/editableitem.h"

#include <QJsonDocument>

class TestJsonDatablockItem : public Aline::EditableItem {
    Q_OBJECT
public:
    Q_PROPERTY(QString string_data READ string_data WRITE setString_data NOTIFY string_dataChanged FINAL)
    Q_PROPERTY(int int_data READ int_data WRITE setInt_data NOTIFY int_dataChanged FINAL)
    Q_PROPERTY(double float_data READ float_data WRITE setFloat_data NOTIFY float_dataChanged FINAL)
    Q_PROPERTY(bool bool_data READ bool_data WRITE setBool_data NOTIFY bool_dataChanged FINAL)

    TestJsonDatablockItem(QString const& ref = "", Aline::EditableItemManager* parent = nullptr) :
        Aline::EditableItem(ref, parent)
    {

    }
    virtual QString getTypeId() const override {
        return "TestJsonDatablockItem";
    }
    virtual QString getTypeName() const override {
        return "TestJsonDatablockItem";
    }
    virtual QString iconInternalUrl() const override {
        return "";
    }

    QString string_data() const;
    void setString_data(const QString &newString_data);

    int int_data() const;
    void setInt_data(int newInt_data);

    double float_data() const;
    void setFloat_data(double newFloat_data);

    bool bool_data() const;
    void setBool_data(bool newBool_data);

Q_SIGNALS:

    void string_dataChanged();
    void int_dataChanged();
    void float_dataChanged();
    void bool_dataChanged();

protected:
    QString _string_data;
    int _int_data;
    double _float_data;
    bool _bool_data;

};

class TestJsonUtils : public QObject {

    Q_OBJECT
private Q_SLOTS:

    void initTestCase();
    void testJsonEncode();
    void testJsonDecode();

};

void TestJsonUtils::initTestCase() {

}

void TestJsonUtils::testJsonEncode() {

    TestJsonDatablockItem item;

    item.setString_data("Toto tata");
    item.setInt_data(4269);
    item.setFloat_data(3.1415926535);
    item.setBool_data(true);

    QJsonObject obj = Aline::JsonUtils::encapsulateItemToJson(&item);

    const char* stringProp = "string_data";
    const char* intProp = "int_data";
    const char* floatProp = "float_data";
    const char* boolProp = "bool_data";

    QVERIFY(obj.contains(stringProp));
    QCOMPARE(obj.value(stringProp).toString(), item.string_data());

    QVERIFY(obj.contains(intProp));
    QCOMPARE(obj.value(intProp).toInt(), item.int_data());

    QVERIFY(obj.contains(floatProp));
    QCOMPARE(obj.value(floatProp).toDouble(), item.float_data());

    QVERIFY(obj.contains(boolProp));
    QCOMPARE(obj.value(boolProp).toBool(), item.bool_data());

    item.setString_data("Lorem Ipsum");
    item.setInt_data(3327);
    item.setFloat_data(1.0);
    item.setBool_data(false);

    obj = Aline::JsonUtils::encapsulateItemToJson(&item);

    QVERIFY(obj.contains(stringProp));
    QCOMPARE(obj.value(stringProp).toString(), item.string_data());

    QVERIFY(obj.contains(intProp));
    QCOMPARE(obj.value(intProp).toInt(), item.int_data());

    QVERIFY(obj.contains(floatProp));
    QCOMPARE(obj.value(floatProp).toDouble(), item.float_data());

    QVERIFY(obj.contains(boolProp));
    QCOMPARE(obj.value(boolProp).toBool(), item.bool_data());

}
void TestJsonUtils::testJsonDecode() {

    const char* stringProp = "string_data";
    const char* intProp = "int_data";
    const char* floatProp = "float_data";
    const char* boolProp = "bool_data";

    QByteArray jsonData1 = ""
                           "{\n"
                           "\"string_data\" : \"Toto tata\",\n"
                           "\"int_data\" : 4269,\n"
                           "\"float_data\" : 3.1415926535,\n"
                           "\"bool_data\" : true\n"
                           "}";

    QByteArray jsonData2 = ""
                           "{\n"
                           "\"string_data\" : \"Lorem ipsum\",\n"
                           "\"int_data\" : 2733,\n"
                           "\"float_data\" : 1.0,\n"
                           "\"bool_data\" : false\n"
                           "}";

    TestJsonDatablockItem item;

    QJsonParseError error;

    QJsonDocument doc1 = QJsonDocument::fromJson(jsonData1, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);

    QJsonDocument doc2 = QJsonDocument::fromJson(jsonData2, &error);
    QCOMPARE(error.error, QJsonParseError::NoError);

    QVERIFY(doc1.isObject());
    QVERIFY(doc2.isObject());

    QJsonObject obj1 = doc1.object();
    QJsonObject obj2 = doc2.object();

    Aline::EditableItemFactoryManager* factoryManager = nullptr;

    Aline::JsonUtils::extractItemData(&item, obj1, factoryManager);

    QCOMPARE(item.string_data(), "Toto tata");
    QCOMPARE(item.int_data(), 4269);
    QCOMPARE(item.float_data(), 3.1415926535);
    QCOMPARE(item.bool_data(), true);

    Aline::JsonUtils::extractItemData(&item, obj2, factoryManager);

    QCOMPARE(item.string_data(), "Lorem ipsum");
    QCOMPARE(item.int_data(), 2733);
    QCOMPARE(item.float_data(), 1.0);
    QCOMPARE(item.bool_data(), false);
}




QString TestJsonDatablockItem::string_data() const
{
    return _string_data;
}



void TestJsonDatablockItem::setString_data(const QString &newString_data)
{
    if (_string_data == newString_data)
        return;
    _string_data = newString_data;
    Q_EMIT string_dataChanged();
}

int TestJsonDatablockItem::int_data() const
{
    return _int_data;
}

void TestJsonDatablockItem::setInt_data(int newInt_data)
{
    if (_int_data == newInt_data)
        return;
    _int_data = newInt_data;
    Q_EMIT int_dataChanged();
}

double TestJsonDatablockItem::float_data() const
{
    return _float_data;
}

void TestJsonDatablockItem::setFloat_data(double newFloat_data)
{
    if (qFuzzyCompare(_float_data, newFloat_data))
        return;
    _float_data = newFloat_data;
    Q_EMIT float_dataChanged();
}

bool TestJsonDatablockItem::bool_data() const
{
    return _bool_data;
}

void TestJsonDatablockItem::setBool_data(bool newBool_data)
{
    if (_bool_data == newBool_data)
        return;
    _bool_data = newBool_data;
    Q_EMIT bool_dataChanged();
}

QTEST_MAIN(TestJsonUtils)
#include "testjsonutils.moc"

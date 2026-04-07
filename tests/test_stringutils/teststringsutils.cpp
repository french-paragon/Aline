#include <QTest>

#include "../../src/utils/stringutils.h"


class TestStringUtils : public QObject {

    Q_OBJECT
private Q_SLOTS:

    void initTestCase();
    void testRemoveAccent();

private:

};

void TestStringUtils::initTestCase() {

}
void TestStringUtils::testRemoveAccent() {

    QCOMPARE(Aline::StringUtils::removeAccents("L'heure du thé à l'échêché!"),"L'heure du the a l'echeche!");
    QCOMPARE(Aline::StringUtils::removeAccents("Échasse à l'Aïkido"),"Echasse a l'Aikido");
    QCOMPARE(Aline::StringUtils::removeAccents("ÀàÄäÈèÉéÖöÜüÂâÎî"),"AaAaEeEeOoUuAaIi");

}

QTEST_MAIN(TestStringUtils)
#include "teststringsutils.moc"
